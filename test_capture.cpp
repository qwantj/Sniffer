#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <iostream>
#include "rawsocketcapture.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    qDebug() << "Testing RawSocketCapture...";
    
    // Create capture object
    RawSocketCapture capture;
    
    // Connect signals
    QObject::connect(&capture, &RawSocketCapture::packetCaptured,
                     [](const RawSocketCapture::PacketInfo &packet) {
        qDebug() << "Packet captured:" << packet.protocol 
                 << packet.srcIp << ":" << packet.srcPort 
                 << "->" << packet.dstIp << ":" << packet.dstPort
                 << "(" << packet.dataLength << "bytes)";
    });
    
    QObject::connect(&capture, &RawSocketCapture::error,
                     [](const QString &message) {
        qDebug() << "Capture error:" << message;
    });
    
    // Start capture
    if (!capture.startCapture("localhost")) {
        qDebug() << "Failed to start capture";
        return 1;
    } else {
        qDebug() << "Capture started successfully!";
        qDebug() << "Test servers listening - you can test by connecting to:";
        qDebug() << "TCP: telnet localhost 8080";
        qDebug() << "UDP: nc -u localhost 8053";
    }
    
    // Stop after 10 seconds for testing
    QTimer::singleShot(10000, [&]() {
        qDebug() << "Stopping capture...";
        capture.stopCapture();
        app.quit();
    });
    
    return app.exec();
}