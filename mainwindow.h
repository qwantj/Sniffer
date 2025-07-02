#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <pcap.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureThread(QObject *parent = nullptr, const QString &deviceName = QString());
    ~CaptureThread();
    void stop();

signals:
    void packetCaptured(QString timestamp, QString src, QString dst, QString protocol, int length);
    void errorOccurred(QString message);

protected:
    void run() override;

private:
    pcap_t *handle;     // Порядок объявления важен!
    bool abort;
    QString deviceName;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void updateTable(QString timestamp, QString src, QString dst, QString protocol, int length);
    void handleError(QString message);

private:
    Ui::MainWindow *ui;
    CaptureThread *captureThread;
    void loadDevices();
};

#endif // MAINWINDOW_H
