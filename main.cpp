#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
#ifdef _WIN32
    // Инициализация Winsock для Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return EXIT_FAILURE;
    }
#else
    // Проверка прав суперпользователя в Linux/Unix
    if (geteuid() != 0) {
        QApplication tempApp(argc, argv);
        QMessageBox::critical(nullptr, "Ошибка",
                              "Для захвата пакетов необходимы права суперпользователя!\n"
                              "Запустите программу с использованием sudo.");
        return EXIT_FAILURE;
    }
#endif

    // Создаем приложение
    QApplication app(argc, argv);
    app.setApplicationName("Сетевой Сниффер");
    app.setOrganizationName("QwantJ");

    // Создаем и отображаем главное окно
    MainWindow mainWindow;
    mainWindow.show();

    // Запускаем цикл событий
    int result = app.exec();

#ifdef _WIN32
    // Очищаем ресурсы Winsock
    WSACleanup();
#endif

    return result;
}
