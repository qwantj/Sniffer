#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    // Создаем приложение
    QApplication app(argc, argv);
    app.setApplicationName("Сетевой Сниффер");
    app.setOrganizationName("QwantJ");

    // Создаем и отображаем главное окно
    MainWindow mainWindow;
    mainWindow.show();

    // Запускаем цикл событий
    return app.exec();
}
