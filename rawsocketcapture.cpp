#include "rawsocketcapture.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QHostInfo>

RawSocketCapture::RawSocketCapture(QObject *parent)
    : QObject(parent)
    , tcpServer(nullptr)
    , udpSocket(nullptr)
    , running(false)
    , packetCount(0)
{
}

RawSocketCapture::~RawSocketCapture()
{
    stopCapture();
}

bool RawSocketCapture::startCapture(const QString &interfaceName)
{
    Q_UNUSED(interfaceName) // В данной реализации не используется конкретный интерфейс
    
    if (running) {
        return false;
    }

    try {
        setupTcpCapture();
        setupUdpCapture();
        running = true;
        packetCount = 0;
        
        qDebug() << "Raw socket capture started";
        return true;
    } catch (const std::exception &e) {
        emit error(QString("Не удалось запустить захват: %1").arg(e.what()));
        return false;
    }
}

void RawSocketCapture::stopCapture()
{
    if (!running) {
        return;
    }

    running = false;

    // Остановка TCP сервера
    if (tcpServer) {
        tcpServer->close();
        delete tcpServer;
        tcpServer = nullptr;
    }

    // Закрытие всех TCP соединений
    for (QTcpSocket *socket : tcpConnections) {
        socket->close();
        socket->deleteLater();
    }
    tcpConnections.clear();

    // Остановка UDP сокета
    if (udpSocket) {
        udpSocket->close();
        delete udpSocket;
        udpSocket = nullptr;
    }

    qDebug() << "Raw socket capture stopped";
}

void RawSocketCapture::setFilter(const QString &filter)
{
    filterExpression = filter;
}

void RawSocketCapture::setupTcpCapture()
{
    tcpServer = new QTcpServer(this);
    
    connect(tcpServer, &QTcpServer::newConnection,
            this, &RawSocketCapture::onTcpConnection);

    // Слушаем на всех доступных адресах на порту 8080 (HTTP)
    if (!tcpServer->listen(QHostAddress::Any, 8080)) {
        // Если порт 8080 занят, пробуем другой порт
        if (!tcpServer->listen(QHostAddress::Any, 0)) {
            throw std::runtime_error("Не удалось запустить TCP сервер");
        }
    }

    qDebug() << "TCP server listening on port:" << tcpServer->serverPort();
}

void RawSocketCapture::setupUdpCapture()
{
    udpSocket = new QUdpSocket(this);
    
    connect(udpSocket, &QUdpSocket::readyRead,
            this, &RawSocketCapture::onUdpDataReady);

    // Слушаем UDP на порту 8053 (альтернативный DNS порт)
    if (!udpSocket->bind(QHostAddress::Any, 8053)) {
        // Если порт занят, пробуем bind на любом доступном порту
        if (!udpSocket->bind(QHostAddress::Any, 0)) {
            throw std::runtime_error("Не удалось запустить UDP сокет");
        }
    }

    qDebug() << "UDP socket listening on port:" << udpSocket->localPort();
}

void RawSocketCapture::onTcpConnection()
{
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket *socket = tcpServer->nextPendingConnection();
        tcpConnections.append(socket);

        connect(socket, &QTcpSocket::readyRead,
                this, &RawSocketCapture::onTcpDataReady);
        connect(socket, &QTcpSocket::disconnected,
                this, &RawSocketCapture::onTcpDisconnected);

        // Эмулируем пакет подключения
        PacketInfo packet;
        packet.protocol = "TCP";
        packet.srcIp = socket->peerAddress().toString();
        packet.srcPort = QString::number(socket->peerPort());
        packet.dstIp = socket->localAddress().toString();
        packet.dstPort = QString::number(socket->localPort());
        packet.dataLength = 0;
        packet.srcIPInt = stringToIp(packet.srcIp);
        packet.dstIPInt = stringToIp(packet.dstIp);
        packet.srcPortInt = socket->peerPort();
        packet.dstPortInt = socket->localPort();
        packet.seqNum = packetCount++;

        emit packetCaptured(packet);
    }
}

void RawSocketCapture::onTcpDataReady()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    processTcpData(socket);
}

void RawSocketCapture::onTcpDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    tcpConnections.removeAll(socket);
    socket->deleteLater();
}

void RawSocketCapture::onUdpDataReady()
{
    processUdpData();
}

void RawSocketCapture::processTcpData(QTcpSocket *socket)
{
    QByteArray data = socket->readAll();
    
    PacketInfo packet;
    packet.protocol = "TCP";
    packet.srcIp = socket->peerAddress().toString();
    packet.srcPort = QString::number(socket->peerPort());
    packet.dstIp = socket->localAddress().toString();
    packet.dstPort = QString::number(socket->localPort());
    packet.dataLength = data.size();
    packet.data = data;
    packet.srcIPInt = stringToIp(packet.srcIp);
    packet.dstIPInt = stringToIp(packet.dstIp);
    packet.srcPortInt = socket->peerPort();
    packet.dstPortInt = socket->localPort();
    packet.seqNum = packetCount++;

    emit packetCaptured(packet);

    // Отправляем простой HTTP ответ для демонстрации
    if (data.contains("GET") || data.contains("POST")) {
        QString response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: 13\r\n"
                          "\r\n"
                          "Hello World!\n";
        socket->write(response.toUtf8());
    }
}

void RawSocketCapture::processUdpData()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray data;
        QHostAddress sender;
        quint16 senderPort;
        
        data.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(data.data(), data.size(), &sender, &senderPort);

        PacketInfo packet;
        packet.protocol = "UDP";
        packet.srcIp = sender.toString();
        packet.srcPort = QString::number(senderPort);
        packet.dstIp = udpSocket->localAddress().toString();
        packet.dstPort = QString::number(udpSocket->localPort());
        packet.dataLength = data.size();
        packet.data = data;
        packet.srcIPInt = stringToIp(packet.srcIp);
        packet.dstIPInt = stringToIp(packet.dstIp);
        packet.srcPortInt = senderPort;
        packet.dstPortInt = udpSocket->localPort();
        packet.seqNum = packetCount++;

        emit packetCaptured(packet);
    }
}

QString RawSocketCapture::ipToString(uint32_t ip)
{
    return QHostAddress(ip).toString();
}

uint32_t RawSocketCapture::stringToIp(const QString &ip)
{
    QHostAddress addr(ip);
    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        return addr.toIPv4Address();
    }
    return 0;
}