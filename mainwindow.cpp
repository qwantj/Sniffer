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
#include <iostream>
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
    running(false), capture(nullptr), packetCount(0), tcpCount(0),
    udpCount(0), httpCount(0), tcpAssembler(nullptr) {
}

CaptureThread::~CaptureThread() {
    stopCapture();
    wait(); // Ожидаем завершения потока

    if (tcpAssembler) {
        delete tcpAssembler;
    }
    
    if (capture) {
        delete capture;
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
    if (capture) {
        capture->stopCapture();
    }
}

void CaptureThread::onPacketCaptured(const RawSocketCapture::PacketInfo &packet)
{
    packetCount++;
    
    // Подсчёт по протоколам
    if (packet.protocol == "TCP") {
        tcpCount++;
        
        // Передаем пакет в TCP сборщик для анализа HTTP
        if (tcpAssembler && !packet.data.isEmpty()) {
            tcpAssembler->processPacket(
                packet.srcIPInt,
                packet.dstIPInt,
                packet.srcPortInt,
                packet.dstPortInt,
                packet.seqNum,
                reinterpret_cast<const uint8_t*>(packet.data.constData()),
                packet.data.size()
            );
        }
    } else if (packet.protocol == "UDP") {
        udpCount++;
    }
    
    // Отправляем сигнал о захваченном пакете
    emit packetCaptured(packet.protocol, packet.srcIp, packet.srcPort,
                        packet.dstIp, packet.dstPort, packet.dataLength);
    
    // Периодическое обновление статистики
    if (packetCount % 10 == 0) {
        emit statisticsUpdated(packetCount, tcpCount, udpCount, httpCount);
        
        // Очистка старых потоков
        if (tcpAssembler) {
            tcpAssembler->clearOldStreams(300); // 5 минут
        }
    }
}

void CaptureThread::onCaptureError(const QString &message)
{
    emit error(message);
}

// Функция HTTP-обработчика для сборщика TCP-потоков
void CaptureThread::onHttpMessage(const StreamKey &key, const std::vector<uint8_t> &data) {
    if (HTTPParser::isHTTP(data.data(), data.size())) {
        httpCount++;

        HTTPMessage message = HTTPParser::parseHTTP(data.data(), data.size());

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
            case HTTP_GET: method = "GET"; break;
            case HTTP_POST: method = "POST"; break;
            case HTTP_PUT: method = "PUT"; break;
            case HTTP_DELETE: method = "DELETE"; break;
            case HTTP_HEAD: method = "HEAD"; break;
            case HTTP_OPTIONS: method = "OPTIONS"; break;
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

void CaptureThread::run() {
    packetCount = 0;
    tcpCount = 0;
    udpCount = 0;
    httpCount = 0;
    running = true;

    // Создаем обработчик HTTP сообщений
    tcpAssembler = new TCPStreamAssembler([this](const StreamKey &key, const std::vector<uint8_t> &data) {
        this->onHttpMessage(key, data);
    });

    // Создаем объект захвата
    capture = new RawSocketCapture();
    capture->setFilter(filterExpr);
    
    // Подключаем сигналы
    connect(capture, &RawSocketCapture::packetCaptured,
            this, &CaptureThread::onPacketCaptured);
    connect(capture, &RawSocketCapture::error,
            this, &CaptureThread::onCaptureError);

    // Запускаем захват
    if (!capture->startCapture(interfaceName)) {
        emit error("Не удалось запустить захват пакетов");
        return;
    }

    // Основной цикл потока
    while (running) {
        msleep(100); // Пауза для снижения нагрузки на процессор
    }

    // Останавливаем захват
    if (capture) {
        capture->stopCapture();
    }

    // Отправляем финальную статистику
    emit statisticsUpdated(packetCount, tcpCount, udpCount, httpCount);
}

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

    // В нашей реализации используем системные интерфейсы через Qt
    interfaceCombo->addItem("localhost (Симуляция)", "localhost");
    interfaces["localhost (Симуляция)"] = "localhost";
    
    // Добавляем некоторые стандартные интерфейсы для демонстрации
    interfaceCombo->addItem("Any Interface (Все интерфейсы)", "any");
    interfaces["Any Interface (Все интерфейсы)"] = "any";
    
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
