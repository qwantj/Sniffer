#ifndef RAWSOCKETCAPTURE_H
#define RAWSOCKETCAPTURE_H

#include <QObject>
#include <QSocketNotifier>
#include <QTimer>
#include <QString>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QHostAddress>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

class RawSocketCapture : public QObject
{
    Q_OBJECT

public:
    explicit RawSocketCapture(QObject *parent = nullptr);
    ~RawSocketCapture();

    bool startCapture(const QString &interfaceName = QString());
    void stopCapture();
    void setFilter(const QString &filter);

    struct PacketInfo {
        QString protocol;
        QString srcIp;
        QString srcPort;
        QString dstIp;
        QString dstPort;
        int dataLength;
        QByteArray data;
        uint32_t srcIPInt;
        uint32_t dstIPInt;
        uint16_t srcPortInt;
        uint16_t dstPortInt;
        uint32_t seqNum;
    };

signals:
    void packetCaptured(const PacketInfo &packet);
    void error(const QString &message);

private slots:
    void onTcpConnection();
    void onTcpDataReady();
    void onUdpDataReady();
    void onTcpDisconnected();

private:
    QTcpServer *tcpServer;
    QUdpSocket *udpSocket;
    QList<QTcpSocket*> tcpConnections;
    QString filterExpression;
    bool running;
    int packetCount;

    void setupTcpCapture();
    void setupUdpCapture();
    void processTcpData(QTcpSocket *socket);
    void processUdpData();
    QString ipToString(uint32_t ip);
    uint32_t stringToIp(const QString &ip);
};

#endif // RAWSOCKETCAPTURE_H