QT += core gui widgets
CONFIG += c++17
TARGET = Sniffer

SOURCES += main.cpp mainwindow.cpp
HEADERS += mainwindow.h
FORMS += mainwindow.ui

# Для Windows
win32 {
    # Автопоиск Npcap
    NPCAP_DIR = $$(NPCAP_DIR)
    isEmpty(NPCAP_DIR) {
        exists(C:/npcap) {
            NPCAP_DIR = C:/npcap
        } else:exists(C:/Program Files/Npcap) {
            NPCAP_DIR = "C:/Program Files/Npcap"
        }
    }

    exists($$NPCAP_DIR) {
        INCLUDEPATH += "$$NPCAP_DIR/include"
        LIBS += -L"$$NPCAP_DIR/Lib/x64" -lwpcap -lPacket
    }

    # Обязательные системные библиотеки
    LIBS += -lWs2_32 -lIphlpapi
}

# Для MSVC
win32-msvc {
    QMAKE_CXXFLAGS += /std:c++17
    LIBS += ws2_32.lib iphlpapi.lib

    contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += wpcap.lib Packet.lib
    }
}

# Для MinGW
win32-g++ {
    LIBS += -lwpcap -lPacket -lws2_32
}
