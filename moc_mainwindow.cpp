/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_CaptureThread_t {
    uint offsetsAndSizes[50];
    char stringdata0[14];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[6];
    char stringdata5[8];
    char stringdata6[6];
    char stringdata7[8];
    char stringdata8[11];
    char stringdata9[20];
    char stringdata10[5];
    char stringdata11[5];
    char stringdata12[8];
    char stringdata13[5];
    char stringdata14[6];
    char stringdata15[8];
    char stringdata16[18];
    char stringdata17[6];
    char stringdata18[4];
    char stringdata19[4];
    char stringdata20[5];
    char stringdata21[17];
    char stringdata22[29];
    char stringdata23[7];
    char stringdata24[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CaptureThread_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CaptureThread_t qt_meta_stringdata_CaptureThread = {
    {
        QT_MOC_LITERAL(0, 13),  // "CaptureThread"
        QT_MOC_LITERAL(14, 14),  // "packetCaptured"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 8),  // "protocol"
        QT_MOC_LITERAL(39, 5),  // "srcIp"
        QT_MOC_LITERAL(45, 7),  // "srcPort"
        QT_MOC_LITERAL(53, 5),  // "dstIp"
        QT_MOC_LITERAL(59, 7),  // "dstPort"
        QT_MOC_LITERAL(67, 10),  // "dataLength"
        QT_MOC_LITERAL(78, 19),  // "httpMessageCaptured"
        QT_MOC_LITERAL(98, 4),  // "type"
        QT_MOC_LITERAL(103, 4),  // "info"
        QT_MOC_LITERAL(108, 7),  // "headers"
        QT_MOC_LITERAL(116, 4),  // "body"
        QT_MOC_LITERAL(121, 5),  // "error"
        QT_MOC_LITERAL(127, 7),  // "message"
        QT_MOC_LITERAL(135, 17),  // "statisticsUpdated"
        QT_MOC_LITERAL(153, 5),  // "total"
        QT_MOC_LITERAL(159, 3),  // "tcp"
        QT_MOC_LITERAL(163, 3),  // "udp"
        QT_MOC_LITERAL(167, 4),  // "http"
        QT_MOC_LITERAL(172, 16),  // "onPacketCaptured"
        QT_MOC_LITERAL(189, 28),  // "RawSocketCapture::PacketInfo"
        QT_MOC_LITERAL(218, 6),  // "packet"
        QT_MOC_LITERAL(225, 14)   // "onCaptureError"
    },
    "CaptureThread",
    "packetCaptured",
    "",
    "protocol",
    "srcIp",
    "srcPort",
    "dstIp",
    "dstPort",
    "dataLength",
    "httpMessageCaptured",
    "type",
    "info",
    "headers",
    "body",
    "error",
    "message",
    "statisticsUpdated",
    "total",
    "tcp",
    "udp",
    "http",
    "onPacketCaptured",
    "RawSocketCapture::PacketInfo",
    "packet",
    "onCaptureError"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CaptureThread[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    6,   50,    2, 0x06,    1 /* Public */,
       9,    8,   63,    2, 0x06,    8 /* Public */,
      14,    1,   80,    2, 0x06,   17 /* Public */,
      16,    4,   83,    2, 0x06,   19 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      21,    1,   92,    2, 0x08,   24 /* Private */,
      24,    1,   95,    2, 0x08,   26 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::Int,    3,    4,    5,    6,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   10,    4,    5,    6,    7,   11,   12,   13,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   17,   18,   19,   20,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

Q_CONSTINIT const QMetaObject CaptureThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_CaptureThread.offsetsAndSizes,
    qt_meta_data_CaptureThread,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CaptureThread_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CaptureThread, std::true_type>,
        // method 'packetCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'httpMessageCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'statisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onPacketCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const RawSocketCapture::PacketInfo &, std::false_type>,
        // method 'onCaptureError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void CaptureThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CaptureThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->packetCaptured((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[6]))); break;
        case 1: _t->httpMessageCaptured((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[8]))); break;
        case 2: _t->error((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->statisticsUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 4: _t->onPacketCaptured((*reinterpret_cast< std::add_pointer_t<RawSocketCapture::PacketInfo>>(_a[1]))); break;
        case 5: _t->onCaptureError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CaptureThread::*)(const QString & , const QString & , const QString & , const QString & , const QString & , int );
            if (_t _q_method = &CaptureThread::packetCaptured; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CaptureThread::*)(const QString & , const QString & , const QString & , const QString & , const QString & , const QString & , const QString & , const QString & );
            if (_t _q_method = &CaptureThread::httpMessageCaptured; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CaptureThread::*)(const QString & );
            if (_t _q_method = &CaptureThread::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CaptureThread::*)(int , int , int , int );
            if (_t _q_method = &CaptureThread::statisticsUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *CaptureThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CaptureThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CaptureThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int CaptureThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void CaptureThread::packetCaptured(const QString & _t1, const QString & _t2, const QString & _t3, const QString & _t4, const QString & _t5, int _t6)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CaptureThread::httpMessageCaptured(const QString & _t1, const QString & _t2, const QString & _t3, const QString & _t4, const QString & _t5, const QString & _t6, const QString & _t7, const QString & _t8)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t8))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CaptureThread::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CaptureThread::statisticsUpdated(int _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
namespace {
struct qt_meta_stringdata_MainWindow_t {
    uint offsetsAndSizes[62];
    char stringdata0[11];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[18];
    char stringdata5[10];
    char stringdata6[16];
    char stringdata7[12];
    char stringdata8[13];
    char stringdata9[17];
    char stringdata10[9];
    char stringdata11[6];
    char stringdata12[8];
    char stringdata13[6];
    char stringdata14[8];
    char stringdata15[11];
    char stringdata16[22];
    char stringdata17[5];
    char stringdata18[5];
    char stringdata19[8];
    char stringdata20[5];
    char stringdata21[15];
    char stringdata22[8];
    char stringdata23[20];
    char stringdata24[6];
    char stringdata25[4];
    char stringdata26[4];
    char stringdata27[5];
    char stringdata28[18];
    char stringdata29[12];
    char stringdata30[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MainWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 12),  // "startCapture"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 11),  // "stopCapture"
        QT_MOC_LITERAL(37, 17),  // "refreshInterfaces"
        QT_MOC_LITERAL(55, 9),  // "showAbout"
        QT_MOC_LITERAL(65, 15),  // "displaySettings"
        QT_MOC_LITERAL(81, 11),  // "savePackets"
        QT_MOC_LITERAL(93, 12),  // "clearPackets"
        QT_MOC_LITERAL(106, 16),  // "onPacketCaptured"
        QT_MOC_LITERAL(123, 8),  // "protocol"
        QT_MOC_LITERAL(132, 5),  // "srcIp"
        QT_MOC_LITERAL(138, 7),  // "srcPort"
        QT_MOC_LITERAL(146, 5),  // "dstIp"
        QT_MOC_LITERAL(152, 7),  // "dstPort"
        QT_MOC_LITERAL(160, 10),  // "dataLength"
        QT_MOC_LITERAL(171, 21),  // "onHttpMessageCaptured"
        QT_MOC_LITERAL(193, 4),  // "type"
        QT_MOC_LITERAL(198, 4),  // "info"
        QT_MOC_LITERAL(203, 7),  // "headers"
        QT_MOC_LITERAL(211, 4),  // "body"
        QT_MOC_LITERAL(216, 14),  // "onCaptureError"
        QT_MOC_LITERAL(231, 7),  // "message"
        QT_MOC_LITERAL(239, 19),  // "onStatisticsUpdated"
        QT_MOC_LITERAL(259, 5),  // "total"
        QT_MOC_LITERAL(265, 3),  // "tcp"
        QT_MOC_LITERAL(269, 3),  // "udp"
        QT_MOC_LITERAL(273, 4),  // "http"
        QT_MOC_LITERAL(278, 17),  // "showPacketDetails"
        QT_MOC_LITERAL(296, 11),  // "QModelIndex"
        QT_MOC_LITERAL(308, 5)   // "index"
    },
    "MainWindow",
    "startCapture",
    "",
    "stopCapture",
    "refreshInterfaces",
    "showAbout",
    "displaySettings",
    "savePackets",
    "clearPackets",
    "onPacketCaptured",
    "protocol",
    "srcIp",
    "srcPort",
    "dstIp",
    "dstPort",
    "dataLength",
    "onHttpMessageCaptured",
    "type",
    "info",
    "headers",
    "body",
    "onCaptureError",
    "message",
    "onStatisticsUpdated",
    "total",
    "tcp",
    "udp",
    "http",
    "showPacketDetails",
    "QModelIndex",
    "index"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x08,    1 /* Private */,
       3,    0,   87,    2, 0x08,    2 /* Private */,
       4,    0,   88,    2, 0x08,    3 /* Private */,
       5,    0,   89,    2, 0x08,    4 /* Private */,
       6,    0,   90,    2, 0x08,    5 /* Private */,
       7,    0,   91,    2, 0x08,    6 /* Private */,
       8,    0,   92,    2, 0x08,    7 /* Private */,
       9,    6,   93,    2, 0x08,    8 /* Private */,
      16,    8,  106,    2, 0x08,   15 /* Private */,
      21,    1,  123,    2, 0x08,   24 /* Private */,
      23,    4,  126,    2, 0x08,   26 /* Private */,
      28,    1,  135,    2, 0x08,   31 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::Int,   10,   11,   12,   13,   14,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   17,   11,   12,   13,   14,   18,   19,   20,
    QMetaType::Void, QMetaType::QString,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   24,   25,   26,   27,
    QMetaType::Void, 0x80000000 | 29,   30,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSizes,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'startCapture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopCapture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'refreshInterfaces'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showAbout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displaySettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'savePackets'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clearPackets'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onPacketCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onHttpMessageCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onCaptureError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onStatisticsUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'showPacketDetails'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->startCapture(); break;
        case 1: _t->stopCapture(); break;
        case 2: _t->refreshInterfaces(); break;
        case 3: _t->showAbout(); break;
        case 4: _t->displaySettings(); break;
        case 5: _t->savePackets(); break;
        case 6: _t->clearPackets(); break;
        case 7: _t->onPacketCaptured((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[6]))); break;
        case 8: _t->onHttpMessageCaptured((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[8]))); break;
        case 9: _t->onCaptureError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->onStatisticsUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 11: _t->showPacketDetails((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
