#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

// Конструктор CaptureThread
CaptureThread::CaptureThread(QObject *parent, const QString &deviceName)
    : QThread(parent),
    handle(nullptr),
    abort(false),
    deviceName(deviceName)
{
}

CaptureThread::~CaptureThread()
{
    stop();
}

void CaptureThread::stop()
{
    abort = true;
    wait();
    if (handle) pcap_close(handle);
}

void CaptureThread::run()
{
    if (deviceName.isEmpty()) {
        emit errorOccurred("Устройство не выбрано!");
        return;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    handle = pcap_open_live(deviceName.toUtf8().constData(), 65536, 1, 1000, errbuf);

    if (!handle) {
        emit errorOccurred(QString("Ошибка открытия устройства: %1").arg(errbuf));
        return;
    }

    // Установка фильтра: только TCP, UDP и HTTP (порт 80)
    struct bpf_program fp;
    const char *filter_exp = "tcp or udp or (tcp port 80)";
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        emit errorOccurred(QString("Ошибка компиляции фильтра: %1").arg(pcap_geterr(handle)));
        return;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        emit errorOccurred(QString("Ошибка установки фильтра: %1").arg(pcap_geterr(handle)));
        pcap_freecode(&fp);
        return;
    }
    pcap_freecode(&fp);

    struct pcap_pkthdr *header;
    const u_char *packet;

    while (!abort) {
        int res = pcap_next_ex(handle, &header, &packet);

        if (res == -1) {
            emit errorOccurred(pcap_geterr(handle));
            break;
        }

        if (res == 0) continue; // Таймаут

        QString src, dst, protocol;
        const uint32_t ethernet_header_length = 14;

        if (static_cast<uint32_t>(header->len) < ethernet_header_length)
            continue;

        // MAC-адреса
        dst = QString("%1:%2:%3:%4:%5:%6")
                  .arg(packet[0], 2, 16, QLatin1Char('0'))
                  .arg(packet[1], 2, 16, QLatin1Char('0'))
                  .arg(packet[2], 2, 16, QLatin1Char('0'))
                  .arg(packet[3], 2, 16, QLatin1Char('0'))
                  .arg(packet[4], 2, 16, QLatin1Char('0'))
                  .arg(packet[5], 2, 16, QLatin1Char('0')).toUpper();

        src = QString("%1:%2:%3:%4:%5:%6")
                  .arg(packet[6], 2, 16, QLatin1Char('0'))
                  .arg(packet[7], 2, 16, QLatin1Char('0'))
                  .arg(packet[8], 2, 16, QLatin1Char('0'))
                  .arg(packet[9], 2, 16, QLatin1Char('0'))
                  .arg(packet[10], 2, 16, QLatin1Char('0'))
                  .arg(packet[11], 2, 16, QLatin1Char('0')).toUpper();

        // Тип Ethernet-фрейма
        uint16_t eth_type = (packet[12] << 8) | packet[13];

        // Анализ IP-пакетов
        if (eth_type == 0x0800 && header->len >= 34) { // IPv4 и достаточно данных
            const u_char *ip_header = packet + 14;
            uint8_t ip_proto = ip_header[9];
            uint8_t ip_header_len = (ip_header[0] & 0x0F) * 4;

            if (header->len >= 14 + ip_header_len + 4) {
                const u_char *transport_header = ip_header + ip_header_len;

                // Анализ TCP/UDP
                if (ip_proto == 6) { // TCP
                    uint16_t src_port = (transport_header[0] << 8) | transport_header[1];
                    uint16_t dst_port = (transport_header[2] << 8) | transport_header[3];

                    if (dst_port == 80 || src_port == 80) {
                        protocol = "HTTP";
                    } else {
                        protocol = "TCP";
                    }
                }
                else if (ip_proto == 17) { // UDP
                    protocol = "UDP";
                }
            }
        }

        // Если не IP-пакет, используем базовое определение
        if (protocol.isEmpty()) {
            switch (eth_type) {
            case 0x0800: protocol = "IPv4"; break;
            case 0x0806: protocol = "ARP"; break;
            case 0x86DD: protocol = "IPv6"; break;
            default: protocol = QString("0x%1").arg(eth_type, 4, 16, QLatin1Char('0'));
            }
        }

        // Временная метка
        QString timestamp = QDateTime::fromSecsSinceEpoch(header->ts.tv_sec)
                                .addMSecs(static_cast<qint64>(header->ts.tv_usec) / 1000)
                                .toString("hh:mm:ss.zzz");

        emit packetCaptured(timestamp, src, dst, protocol, header->len);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), captureThread(nullptr)
{
    ui->setupUi(this);

    // Настройка таблицы пакетов
    QStringList headers;
    headers << "Время" << "Источник" << "Назначение" << "Протокол" << "Длина";
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    loadDevices();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (captureThread) {
        captureThread->stop();
        delete captureThread;
    }
}

void MainWindow::loadDevices()
{
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить устройства: " + QString(errbuf));
        return;
    }

    for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {
        // Явное преобразование char* -> QString
        QString description = d->description ?
                                  QString::fromUtf8(d->description) :
                                  QString("Unknown Device");

        QString name = QString::fromUtf8(d->name);

        // Сохраняем имя устройства как пользовательские данные
        ui->deviceComboBox->addItem(description, name);
    }

    pcap_freealldevs(alldevs);
}

void MainWindow::on_startButton_clicked()
{
    if (captureThread) {
        captureThread->stop();
        delete captureThread;
        captureThread = nullptr;
    }

    int index = ui->deviceComboBox->currentIndex();
    if (index < 0) return;

    QString deviceName = ui->deviceComboBox->itemData(index).toString();
    if (deviceName.isEmpty()) return;

    captureThread = new CaptureThread(this, deviceName);

    connect(captureThread, &CaptureThread::packetCaptured,
            this, &MainWindow::updateTable);
    connect(captureThread, &CaptureThread::errorOccurred,
            this, &MainWindow::handleError);

    ui->tableWidget->setRowCount(0);
    captureThread->start();
}

void MainWindow::on_stopButton_clicked()
{
    if (captureThread) captureThread->stop();
}

void MainWindow::updateTable(QString timestamp, QString src, QString dst, QString protocol, int length)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(timestamp));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(src));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(dst));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(protocol));
    ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(length)));

    // Автопрокрутка к последней строке
    ui->tableWidget->scrollToBottom();
}

void MainWindow::handleError(QString message)
{
    QMessageBox::warning(this, "Ошибка захвата", message);
    on_stopButton_clicked();
}
