#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTableView>
#include <QTextEdit>
#include <QHeaderView>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QSettings>
#include <QInputDialog>
#include <QSplitter>
#include <QGroupBox>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <unistd.h>
#endif

#include "mainwindow.h"

// ------------------ Реализация CaptureThread ------------------

CaptureThread::CaptureThread(QObject *parent) : QThread(parent),
    running(false), handle(nullptr), packetCount(0), tcpCount(0),
    udpCount(0), httpCount(0), tcpAssembler(nullptr) {
}

CaptureThread::~CaptureThread() {
    stopCapture();
    wait(); // Ожидаем завершения потока

    if (tcpAssembler) {
        delete tcpAssembler;
    }
}

void CaptureThread::setInterface(const QString &name) {
    interfaceName = name;
}

void CaptureThread::setFilter(const QString &filter) {
    filterExpr = filter;
}

void CaptureThread::stopCapture() {
    running = false;
}

// Адаптер для вызова метода экземпляра из статической функции обратного вызова
void CaptureThread::packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    CaptureThread *thread = reinterpret_cast<CaptureThread*>(userData);
    thread->processPacket(pkthdr, packet);
}

// Функция HTTP-обработчика для сборщика TCP-потоков
void CaptureThread::onHttpMessage(const StreamKey &key, const std::vector<uint8_t> &data) {
    if (HTTPParser::isHTTP(data.data(), data.size())) {
        httpCount++;

        HTTPParser::HTTPMessage message = HTTPParser::parseHTTP(data.data(), data.size());

        // Получаем IP-адреса в читаемом формате
        char srcIP[INET_ADDRSTRLEN];
        char dstIP[INET_ADDRSTRLEN];
        struct in_addr src_addr = {htonl(key.srcIP)};
        struct in_addr dst_addr = {htonl(key.dstIP)};
        inet_ntop(AF_INET, &src_addr, srcIP, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &dst_addr, dstIP, INET_ADDRSTRLEN);

        // Тип сообщения (запрос/ответ)
        QString type = message.isRequest ? "Запрос" : "Ответ";

        // Основная информация для заголовка
        QString info;
        if (message.isRequest) {
            QString method;
            switch (message.method) {
            case HTTPParser::GET: method = "GET"; break;
            case HTTPParser::POST: method = "POST"; break;
            case HTTPParser::PUT: method = "PUT"; break;
            case HTTPParser::DELETE: method = "DELETE"; break;
            case HTTPParser::HEAD: method = "HEAD"; break;
            case HTTPParser::OPTIONS: method = "OPTIONS"; break;
            default: method = "UNKNOWN"; break;
            }
            info = method + " " + QString::fromStdString(message.uri) + " " +
                   QString::fromStdString(message.version);
        } else {
            info = QString::fromStdString(message.version) + " " +
                   QString::number(message.statusCode) + " " +
                   QString::fromStdString(message.statusText);
        }

        // Собираем заголовки
        QString headers;
        for (const auto& header : message.headers) {
            headers += QString::fromStdString(header.first) + ": " +
                       QString::fromStdString(header.second) + "\n";
        }

        // Тело сообщения
        QString body = QString::fromStdString(message.body);

        // Отправляем сигнал в основной поток с информацией о HTTP-сообщении
        emit httpMessageCaptured(
            type,
            QString(srcIP), QString::number(key.srcPort),
            QString(dstIP), QString::number(key.dstPort),
            info, headers, body
            );

        // Обновляем статистику
        emit statisticsUpdated(packetCount, tcpCount, udpCount, httpCount);
    }
}

void CaptureThread::processPacket(const pcap_pkthdr *pkthdr, const u_char *packet) {
    packetCount++;

#ifdef _WIN32
    // Структуры для Windows
    struct ether_header {
        u_char ether_dhost[6];
        u_char ether_shost[6];
        u_short ether_type;
    };

    struct ip_header {
        u_char  ip_vhl;
        u_char  ip_tos;
        u_short ip_len;
        u_short ip_id;
        u_short ip_off;
        u_char  ip_ttl;
        u_char  ip_p;
        u_short ip_sum;
        struct in_addr ip_src;
        struct in_addr ip_dst;
    };

    struct tcp_header {
        u_short th_sport;
        u_short th_dport;
        u_int   th_seq;
        u_int   th_ack;
        u_char  th_offx2;
        u_char  th_flags;
        u_short th_win;
        u_short th_sum;
        u_short th_urp;
    };

    struct udp_header {
        u_short uh_sport;
        u_short uh_dport;
        u_short uh_len;
        u_short uh_sum;
    };

// Константы для Windows
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17

    // Обработка для Windows
    struct ether_header* ethHeader = (struct ether_header*)packet;
    struct ip_header* ipHeader = (struct ip_header*)(packet + sizeof(struct ether_header));

    if (ipHeader->ip_p == IPPROTO_TCP) {
        tcpCount++;

        // Получаем длину IP-заголовка
        int ipHeaderLength = (ipHeader->ip_vhl & 0x0f) * 4;
        struct tcp_header* tcpHeader = (struct tcp_header*)(packet + sizeof(struct ether_header) + ipHeaderLength);

        // Получаем длину TCP-заголовка
        int tcpHeaderLength = ((tcpHeader->th_offx2 & 0xf0) >> 4) * 4;

        // Данные и их длина
        const u_char* tcpData = packet + sizeof(struct ether_header) + ipHeaderLength + tcpHeaderLength;
        int dataLength = ntohs(ipHeader->ip_len) - ipHeaderLength - tcpHeaderLength;

        if (dataLength > 0) {
            // IP-адреса и порты
            QString srcIp = QString(inet_ntoa(ipHeader->ip_src));
            QString dstIp = QString(inet_ntoa(ipHeader->ip_dst));
            QString srcPort = QString::number(ntohs(tcpHeader->th_sport));
            QString dstPort = QString::number(ntohs(tcpHeader->th_dport));

            // Отправляем информацию о TCP пакете в основной поток
            emit packetCaptured("TCP", srcIp, srcPort, dstIp, dstPort, dataLength);

            // Передаем пакет в TCP сборщик для анализа HTTP
            if (tcpAssembler) {
                tcpAssembler->processPacket(
                    ntohl(ipHeader->ip_src.s_addr),
                    ntohl(ipHeader->ip_dst.s_addr),
                    ntohs(tcpHeader->th_sport),
                    ntohs(tcpHeader->th_dport),
                    ntohl(tcpHeader->th_seq),
                    tcpData,
                    dataLength
                    );
            }
        }
    }
    else if (ipHeader->ip_p == IPPROTO_UDP) {
        udpCount++;

        // Получаем длину IP-заголовка
        int ipHeaderLength = (ipHeader->ip_vhl & 0x0f) * 4;
        struct udp_header* udpHeader = (struct udp_header*)(packet + sizeof(struct ether_header) + ipHeaderLength);

        // Данные и их длина
        const u_char* udpData = packet + sizeof(struct ether_header) + ipHeaderLength + sizeof(struct udp_header);
        int dataLength = ntohs(udpHeader->uh_len) - sizeof(struct udp_header);

        if (dataLength > 0) {
            // IP-адреса и порты
            QString srcIp = QString(inet_ntoa(ipHeader->ip_src));
            QString dstIp = QString(inet_ntoa(ipHeader->ip_dst));
            QString srcPort = QString::number(ntohs(udpHeader->uh_sport));
            QString dstPort = QString::number(ntohs(udpHeader->uh_dport));

            // Отправляем информацию о UDP пакете в основной поток
            emit packetCaptured("UDP", srcIp, srcPort, dstIp, dstPort, dataLength);
        }
    }
#else
    // Обработка для Unix/Linux
    struct ethhdr* ethHeader = (struct ethhdr*)packet;
    struct iphdr* ipHeader = (struct iphdr*)(packet + sizeof(struct ethhdr));

    if (ipHeader->protocol == IPPROTO_TCP) {
        tcpCount++;

        // Рассчитываем смещение для TCP заголовка
        int ipHeaderLength = ipHeader->ihl * 4;
        struct tcphdr* tcpHeader = (struct tcphdr*)(packet + sizeof(struct ethhdr) + ipHeaderLength);

        // Длина TCP заголовка
        int tcpHeaderLength = tcpHeader->doff * 4;

        // Рассчитываем указатель на данные и их длину
        const u_char* tcpData = packet + sizeof(struct ethhdr) + ipHeaderLength + tcpHeaderLength;
        int dataLength = ntohs(ipHeader->tot_len) - ipHeaderLength - tcpHeaderLength;

        if (dataLength > 0) {
            // IP-адреса и порты
            struct in_addr src_addr, dst_addr;
            src_addr.s_addr = ipHeader->saddr;
            dst_addr.s_addr = ipHeader->daddr;

            QString srcIp = QString(inet_ntoa(src_addr));
            QString dstIp = QString(inet_ntoa(dst_addr));
            QString srcPort = QString::number(ntohs(tcpHeader->source));
            QString dstPort = QString::number(ntohs(tcpHeader->dest));

            // Отправляем информацию о TCP пакете в основной поток
            emit packetCaptured("TCP", srcIp, srcPort, dstIp, dstPort, dataLength);

            // Передаем пакет в TCP сборщик для анализа HTTP
            if (tcpAssembler) {
                tcpAssembler->processPacket(
                    ntohl(ipHeader->saddr),
                    ntohl(ipHeader->daddr),
                    ntohs(tcpHeader->source),
                    ntohs(tcpHeader->dest),
                    ntohl(tcpHeader->seq),
                    tcpData,
                    dataLength
                    );
            }
        }
    }
    else if (ipHeader->protocol == IPPROTO_UDP) {
        udpCount++;

        // Рассчитываем смещение для UDP заголовка
        int ipHeaderLength = ipHeader->ihl * 4;
        struct udphdr* udpHeader = (struct udphdr*)(packet + sizeof(struct ethhdr) + ipHeaderLength);

        // Рассчитываем указатель на данные и их длину
        const u_char* udpData = packet + sizeof(struct ethhdr) + ipHeaderLength + sizeof(struct udphdr);
        int dataLength = ntohs(udpHeader->len) - sizeof(struct udphdr);

        if (dataLength > 0) {
            // IP-адреса и порты
            struct in_addr src_addr, dst_addr;
            src_addr.s_addr = ipHeader->saddr;
            dst_addr.s_addr = ipHeader->daddr;

            QString srcIp = QString(inet_ntoa(src_addr));
            QString dstIp = QString(inet_ntoa(dst_addr));
            QString srcPort = QString::number(ntohs(udpHeader->source));
            QString dstPort = QString::number(ntohs(udpHeader->dest));

            // Отправляем информацию о UDP пакете в основной поток
            emit packetCaptured("UDP", srcIp, srcPort, dstIp, dstPort, dataLength);
        }
    }
#endif

    // Периодическое обновление статистики
    if (packetCount % 10 == 0) {
        emit statisticsUpdated(packetCount, tcpCount, udpCount, httpCount);

        // Очистка старых потоков
        if (tcpAssembler) {
            tcpAssembler->clearOldStreams(300); // 5 минут
        }
    }
}

void CaptureThread::run() {
    packetCount = 0;
    tcpCount = 0;
    udpCount = 0;
    httpCount = 0;
    running = true;

    char errbuf[PCAP_ERRBUF_SIZE];

    // Создаем обработчик HTTP сообщений
    tcpAssembler = new TCPStreamAssembler([this](const StreamKey &key, const std::vector<uint8_t> &data) {
        this->onHttpMessage(key, data);
    });

    // Открываем интерфейс для захвата
    handle = pcap_open_live(interfaceName.toLocal8Bit().constData(), 65536, 1, 1000, errbuf);
    if (!handle) {
        emit error(QString("Не удалось открыть интерфейс %1: %2").arg(interfaceName).arg(errbuf));
        return;
    }

    // Компиляция фильтра
    struct bpf_program fp;
    if (!filterExpr.isEmpty()) {
        if (pcap_compile(handle, &fp, filterExpr.toLocal8Bit().constData(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
            emit error(QString("Не удалось скомпилировать фильтр: %1").arg(pcap_geterr(handle)));
            pcap_close(handle);
            handle = nullptr;
            return;
        }

        if (pcap_setfilter(handle, &fp) == -1) {
            emit error(QString("Не удалось установить фильтр: %1").arg(pcap_geterr(handle)));
            pcap_freecode(&fp);
            pcap_close(handle);
            handle = nullptr;
            return;
        }

        pcap_freecode(&fp);
    }

    // Основной цикл захвата пакетов
    while (running) {
        if (pcap_dispatch(handle, -1, packetHandler, reinterpret_cast<u_char*>(this)) == -1) {
            if (running) { // Проверяем, что мы не остановились намеренно
                emit error(QString("Ошибка при захвате пакетов: %1").arg(pcap_geterr(handle)));
            }
            break;
        }
    }

    // Закрываем устройство захвата
    if (handle) {
        pcap_close(handle);
        handle = nullptr;
    }

    // Отправляем финальную статистику
    emit statisticsUpdated(packetCount, tcpCount, udpCount, httpCount);
}

// ------------------ Реализация MainWindow ------------------

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), captureThread(nullptr) {
    setupUi();
    createActions();
    createMenus();
    createStatusBar();

    // Загружаем список интерфейсов
    loadInterfaces();

    // Создаем поток захвата
    captureThread = new CaptureThread(this);

    // Подключаем сигналы потока
    connect(captureThread, &CaptureThread::packetCaptured, this, &MainWindow::onPacketCaptured);
    connect(captureThread, &CaptureThread::httpMessageCaptured, this, &MainWindow::onHttpMessageCaptured);
    connect(captureThread, &CaptureThread::error, this, &MainWindow::onCaptureError);
    connect(captureThread, &CaptureThread::statisticsUpdated, this, &MainWindow::onStatisticsUpdated);

    // Настраиваем размер окна
    resize(900, 600);
    setWindowTitle("Сетевой Сниффер с Поддержкой HTTP");
}

MainWindow::~MainWindow() {
    if (captureThread && captureThread->isRunning()) {
        captureThread->stopCapture();
        captureThread->wait();
    }
}

void MainWindow::setupUi() {
    // Создаем центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Основной вертикальный layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Секция управления
    QHBoxLayout *controlLayout = new QHBoxLayout();

    QLabel *interfaceLabel = new QLabel("Интерфейс:", this);
    interfaceCombo = new QComboBox(this);
    interfaceCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton *refreshButton = new QPushButton("Обновить", this);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshInterfaces);

    QLabel *filterLabel = new QLabel("Фильтр:", this);
    filterEdit = new QLineEdit(this);
    filterEdit->setPlaceholderText("Например: tcp port 80 or port 443");

    startButton = new QPushButton("Начать", this);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startCapture);

    stopButton = new QPushButton("Остановить", this);
    stopButton->setEnabled(false);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopCapture);

    // Добавляем элементы управления
    controlLayout->addWidget(interfaceLabel);
    controlLayout->addWidget(interfaceCombo);
    controlLayout->addWidget(refreshButton);
    controlLayout->addWidget(filterLabel);
    controlLayout->addWidget(filterEdit);
    controlLayout->addWidget(startButton);
    controlLayout->addWidget(stopButton);

    mainLayout->addLayout(controlLayout);

    // Разделитель для таблицы и детализации
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    // Таблица пакетов
    packetsTable = new QTableView(this);
    packetsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    packetsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    packetsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    packetsTable->setSortingEnabled(true);
    packetsTable->horizontalHeader()->setStretchLastSection(true);
    packetsTable->verticalHeader()->setVisible(false);

    // Модель данных для таблицы
    packetsModel = new QStandardItemModel(this);
    packetsModel->setColumnCount(7);
    packetsModel->setHeaderData(0, Qt::Horizontal, "№");
    packetsModel->setHeaderData(1, Qt::Horizontal, "Время");
    packetsModel->setHeaderData(2, Qt::Horizontal, "Протокол");
    packetsModel->setHeaderData(3, Qt::Horizontal, "Отправитель");
    packetsModel->setHeaderData(4, Qt::Horizontal, "Порт");
    packetsModel->setHeaderData(5, Qt::Horizontal, "Получатель");
    packetsModel->setHeaderData(6, Qt::Horizontal, "Порт");
    packetsModel->setHeaderData(7, Qt::Horizontal, "Размер (байт)");
    packetsTable->setModel(packetsModel);

    // Обработка выбора строки
    connect(packetsTable, &QTableView::clicked, this, &MainWindow::showPacketDetails);

    // Текстовое поле для деталей пакета
    detailsText = new QTextEdit(this);
    detailsText->setReadOnly(true);

    // Добавляем в разделитель
    splitter->addWidget(packetsTable);
    splitter->addWidget(detailsText);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    mainLayout->addWidget(splitter);
}

void MainWindow::createActions() {
    // Здесь можно добавить дополнительные действия для меню
}

void MainWindow::createMenus() {
    // Меню "Файл"
    QMenu *fileMenu = menuBar()->addMenu("&Файл");

    // Действие "Сохранить пакеты"
    QAction *saveAction = fileMenu->addAction("&Сохранить пакеты...");
    connect(saveAction, &QAction::triggered, this, &MainWindow::savePackets);

    // Действие "Очистить"
    QAction *clearAction = fileMenu->addAction("О&чистить");
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearPackets);

    fileMenu->addSeparator();

    // Действие "Выход"
    QAction *exitAction = fileMenu->addAction("&Выход");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Меню "Настройки"
    QMenu *settingsMenu = menuBar()->addMenu("&Настройки");

    // Действие "Параметры"
    QAction *settingsAction = settingsMenu->addAction("&Параметры...");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::displaySettings);

    // Меню "Справка"
    QMenu *helpMenu = menuBar()->addMenu("&Справка");

    // Действие "О программе"
    QAction *aboutAction = helpMenu->addAction("&О программе");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::createStatusBar() {
    statusLabel = new QLabel("Готов", this);
    statusBar()->addWidget(statusLabel);

    statsLabel = new QLabel("Пакетов: 0, TCP: 0, UDP: 0, HTTP: 0", this);
    statusBar()->addPermanentWidget(statsLabel);
}

void MainWindow::loadInterfaces() {
    interfaceCombo->clear();
    interfaces.clear();

    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        QMessageBox::warning(this, "Ошибка", QString("Не удалось получить список интерфейсов: %1").arg(errbuf));
        return;
    }

    for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {
        QString name = d->name;
        QString description = d->description ? d->description : "Нет описания";

        interfaceCombo->addItem(description, name);
        interfaces[description] = name;
    }

    pcap_freealldevs(alldevs);

    if (interfaceCombo->count() == 0) {
        interfaceCombo->addItem("Интерфейсы не найдены");
        startButton->setEnabled(false);
    } else {
        startButton->setEnabled(true);
    }
}

void MainWindow::refreshInterfaces() {
    loadInterfaces();
}

void MainWindow::startCapture() {
    if (captureThread && captureThread->isRunning()) {
        return;
    }

    if (interfaceCombo->count() == 0) {
        QMessageBox::warning(this, "Ошибка", "Нет доступных интерфейсов.");
        return;
    }

    // Получаем имя выбранного интерфейса и фильтр
    QString interfaceName = interfaceCombo->currentData().toString();
    QString filter = filterEdit->text().trimmed();

    // Настраиваем и запускаем поток
    captureThread->setInterface(interfaceName);
    captureThread->setFilter(filter);
    captureThread->start();

    // Обновляем состояние UI
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
    interfaceCombo->setEnabled(false);
    filterEdit->setEnabled(false);

    statusLabel->setText("Захват пакетов...");
}

void MainWindow::stopCapture() {
    if (captureThread && captureThread->isRunning()) {
        captureThread->stopCapture();
        captureThread->wait(); // Ждем завершения потока
    }

    // Обновляем состояние UI
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    interfaceCombo->setEnabled(true);
    filterEdit->setEnabled(true);

    statusLabel->setText("Готов");
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "О программе",
                       "Сетевой Сниффер с поддержкой HTTP\n\n"
                       "Версия 1.0\n\n"
                       "Приложение для перехвата и анализа сетевого трафика,\n"
                       "включая UDP, TCP и HTTP пакеты.\n\n"
                       "© 2025 - Все права защищены");
}

void MainWindow::displaySettings() {
    // Простое диалоговое окно настроек
    int timeout = QInputDialog::getInt(this, "Настройки",
                                       "Таймаут очистки TCP-потоков (секунды):",
                                       300, 10, 3600);

    // Можно сохранить настройки для использования в программе
    QSettings settings;
    settings.setValue("tcp_stream_timeout", timeout);
}

void MainWindow::savePackets() {
    if (packetsModel->rowCount() == 0) {
        QMessageBox::information(this, "Информация", "Нет пакетов для сохранения.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить пакеты",
                                                    QDir::homePath() + "/packets.csv",
                                                    "CSV файлы (*.csv)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }

    QTextStream out(&file);

    // Заголовок CSV
    out << "№,Время,Протокол,Отправитель,Порт,Получатель,Порт,Размер\n";

    // Данные
    for (int row = 0; row < packetsModel->rowCount(); ++row) {
        for (int col = 0; col < packetsModel->columnCount(); ++col) {
            out << packetsModel->data(packetsModel->index(row, col)).toString();
            if (col < packetsModel->columnCount() - 1) {
                out << ",";
            }
        }
        out << "\n";
    }

    file.close();
    statusLabel->setText(QString("Пакеты сохранены в %1").arg(fileName));
}

void MainWindow::clearPackets() {
    packetsModel->removeRows(0, packetsModel->rowCount());
    detailsText->clear();
    statusLabel->setText("Готов");
}

void MainWindow::onPacketCaptured(const QString &protocol,
                                  const QString &srcIp, const QString &srcPort,
                                  const QString &dstIp, const QString &dstPort,
                                  int dataLength) {
    int row = packetsModel->rowCount();

    // Добавляем строку с данными пакета
    packetsModel->insertRow(row);
    packetsModel->setData(packetsModel->index(row, 0), row + 1);
    packetsModel->setData(packetsModel->index(row, 1), QTime::currentTime().toString("hh:mm:ss.zzz"));
    packetsModel->setData(packetsModel->index(row, 2), protocol);
    packetsModel->setData(packetsModel->index(row, 3), srcIp);
    packetsModel->setData(packetsModel->index(row, 4), srcPort);
    packetsModel->setData(packetsModel->index(row, 5), dstIp);
    packetsModel->setData(packetsModel->index(row, 6), dstPort);
    packetsModel->setData(packetsModel->index(row, 7), dataLength);

    // Прокрутка к последней строке
    packetsTable->scrollToBottom();
}

void MainWindow::onHttpMessageCaptured(const QString &type,
                                       const QString &srcIp, const QString &srcPort,
                                       const QString &dstIp, const QString &dstPort,
                                       const QString &info, const QString &headers,
                                       const QString &body) {
    int row = packetsModel->rowCount();

    // Добавляем строку с данными HTTP пакета
    packetsModel->insertRow(row);
    packetsModel->setData(packetsModel->index(row, 0), row + 1);
    packetsModel->setData(packetsModel->index(row, 1), QTime::currentTime().toString("hh:mm:ss.zzz"));
    packetsModel->setData(packetsModel->index(row, 2), "HTTP " + type);
    packetsModel->setData(packetsModel->index(row, 3), srcIp);
    packetsModel->setData(packetsModel->index(row, 4), srcPort);
    packetsModel->setData(packetsModel->index(row, 5), dstIp);
    packetsModel->setData(packetsModel->index(row, 6), dstPort);
    packetsModel->setData(packetsModel->index(row, 7), headers.size() + body.size());

    // Сохраняем полные данные для отображения в деталях
    QMap<QString, QVariant> details;
    details["type"] = type;
    details["info"] = info;
    details["headers"] = headers;
    details["body"] = body;
    packetsModel->setData(packetsModel->index(row, 0), QVariant::fromValue(details), Qt::UserRole);

    // Прокрутка к последней строке
    packetsTable->scrollToBottom();
}

void MainWindow::onCaptureError(const QString &message) {
    QMessageBox::critical(this, "Ошибка захвата", message);
    stopCapture();
}

void MainWindow::onStatisticsUpdated(int total, int tcp, int udp, int http) {
    statsLabel->setText(QString("Пакетов: %1, TCP: %2, UDP: %3, HTTP: %4")
                            .arg(total).arg(tcp).arg(udp).arg(http));
}

void MainWindow::showPacketDetails(const QModelIndex &index) {
    int row = index.row();

    // Проверяем, есть ли расширенные детали для HTTP
    QVariant detailsVariant = packetsModel->data(packetsModel->index(row, 0), Qt::UserRole);
    if (detailsVariant.isValid()) {
        QMap<QString, QVariant> details = detailsVariant.value<QMap<QString, QVariant>>();

        QString htmlDetails = "<h3>HTTP " + details["type"].toString() + "</h3>";
        htmlDetails += "<p><b>" + details["info"].toString() + "</b></p>";
        htmlDetails += "<h4>Заголовки:</h4>";
        htmlDetails += "<pre>" + details["headers"].toString() + "</pre>";

        if (!details["body"].toString().isEmpty()) {
            htmlDetails += "<h4>Тело сообщения:</h4>";
            htmlDetails += "<pre>" + details["body"].toString().replace("<", "&lt;").replace(">", "&gt;") + "</pre>";
        }

        detailsText->setHtml(htmlDetails);
    } else {
        // Стандартные детали для обычных пакетов
        QString protocol = packetsModel->data(packetsModel->index(row, 2)).toString();
        QString srcIp = packetsModel->data(packetsModel->index(row, 3)).toString();
        QString srcPort = packetsModel->data(packetsModel->index(row, 4)).toString();
        QString dstIp = packetsModel->data(packetsModel->index(row, 5)).toString();
        QString dstPort = packetsModel->data(packetsModel->index(row, 6)).toString();
        int size = packetsModel->data(packetsModel->index(row, 7)).toInt();

        QString details = QString("<h3>%1 пакет</h3>").arg(protocol);
        details += "<p><b>Отправитель:</b> " + srcIp + ":" + srcPort + "</p>";
        details += "<p><b>Получатель:</b> " + dstIp + ":" + dstPort + "</p>";
        details += "<p><b>Размер данных:</b> " + QString::number(size) + " байт</p>";

        detailsText->setHtml(details);
    }
}
