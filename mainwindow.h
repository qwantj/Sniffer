#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <QStandardItemModel>
#include <QMap>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QTextEdit>

// Подключаем WinPcap/Npcap с учётом платформы
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define HAVE_REMOTE
#include <pcap.h>
#else
#include <pcap.h>
#include <unistd.h>
#endif

#include "tcp_stream_assembler.h"
#include "http_parser.h"

// Объявляем поток для захвата пакетов
class CaptureThread : public QThread {
    Q_OBJECT

public:
    explicit CaptureThread(QObject *parent = nullptr);
    ~CaptureThread();

    void setInterface(const QString &interfaceName);
    void setFilter(const QString &filter);
    void stopCapture();

signals:
    void packetCaptured(const QString &protocol,
                        const QString &srcIp, const QString &srcPort,
                        const QString &dstIp, const QString &dstPort,
                        int dataLength);
    void httpMessageCaptured(const QString &type,
                             const QString &srcIp, const QString &srcPort,
                             const QString &dstIp, const QString &dstPort,
                             const QString &info, const QString &headers,
                             const QString &body);
    void error(const QString &message);
    void statisticsUpdated(int total, int tcp, int udp, int http);

protected:
    void run() override;

private:
    QString interfaceName;
    QString filterExpr;
    volatile bool running;
    pcap_t *handle;
    int packetCount;
    int tcpCount;
    int udpCount;
    int httpCount;
    TCPStreamAssembler *tcpAssembler;

    static void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet);
    void processPacket(const pcap_pkthdr *pkthdr, const u_char *packet);
    void onHttpMessage(const StreamKey &key, const std::vector<uint8_t> &data);
};

// Главное окно приложения
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startCapture();
    void stopCapture();
    void refreshInterfaces();
    void showAbout();
    void displaySettings();
    void savePackets();
    void clearPackets();

    void onPacketCaptured(const QString &protocol,
                          const QString &srcIp, const QString &srcPort,
                          const QString &dstIp, const QString &dstPort,
                          int dataLength);
    void onHttpMessageCaptured(const QString &type,
                               const QString &srcIp, const QString &srcPort,
                               const QString &dstIp, const QString &dstPort,
                               const QString &info, const QString &headers,
                               const QString &body);
    void onCaptureError(const QString &message);
    void onStatisticsUpdated(int total, int tcp, int udp, int http);
    void showPacketDetails(const QModelIndex &index);

private:
    // Интерфейс
    QComboBox *interfaceCombo;
    QLineEdit *filterEdit;
    QPushButton *startButton;
    QPushButton *stopButton;
    QTableView *packetsTable;
    QTextEdit *detailsText;
    QLabel *statusLabel;
    QLabel *statsLabel;

    // Модель данных
    QStandardItemModel *packetsModel;

    // Поток захвата
    CaptureThread *captureThread;

    // Список интерфейсов
    QMap<QString, QString> interfaces;

    // Инициализация UI
    void setupUi();
    void createActions();
    void createMenus();
    void createStatusBar();

    // Загрузка списка интерфейсов
    void loadInterfaces();
};

#endif // MAINWINDOW_H
