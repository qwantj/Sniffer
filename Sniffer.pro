# Sniffer.pro - QMake проектный файл для сниффера с поддержкой HTTP

QT += core gui widgets network

TARGET = sniffer
CONFIG += c++17
CONFIG += warn_on

TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           tcp_stream_assembler.cpp \
           http_parser.cpp \
           rawsocketcapture.cpp

HEADERS += mainwindow.h \
           tcp_stream_assembler.h \
           http_parser.h \
           rawsocketcapture.h

# Флаги компилятора в зависимости от платформы
win32 {
    # Флаги для MSVC (Windows)
    QMAKE_CXXFLAGS += /W4
    LIBS += -lws2_32
    # Windows-специфичные определения
    DEFINES += WIN32 _CONSOLE _WINSOCK_DEPRECATED_NO_WARNINGS
} else {
    # Флаги для GCC/Clang (Unix-подобные системы)
    QMAKE_CXXFLAGS += -Wall
}

# Дополнительные опции для отладки
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
    message("Debug build")
}

CONFIG(release, debug|release) {
    DEFINES += NDEBUG QT_NO_DEBUG
    win32:QMAKE_CXXFLAGS += /O2
    else:QMAKE_CXXFLAGS += -O2
    message("Release build")
}

# Инструкции для установки
unix {
    target.path = /usr/local/bin
    INSTALLS += target
}
