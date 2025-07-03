/****************************************************************************
** Meta object code from reading C++ file 'rawsocketcapture.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "rawsocketcapture.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rawsocketcapture.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_RawSocketCapture_t {
    uint offsetsAndSizes[22];
    char stringdata0[17];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[7];
    char stringdata5[6];
    char stringdata6[8];
    char stringdata7[16];
    char stringdata8[15];
    char stringdata9[15];
    char stringdata10[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RawSocketCapture_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RawSocketCapture_t qt_meta_stringdata_RawSocketCapture = {
    {
        QT_MOC_LITERAL(0, 16),  // "RawSocketCapture"
        QT_MOC_LITERAL(17, 14),  // "packetCaptured"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 10),  // "PacketInfo"
        QT_MOC_LITERAL(44, 6),  // "packet"
        QT_MOC_LITERAL(51, 5),  // "error"
        QT_MOC_LITERAL(57, 7),  // "message"
        QT_MOC_LITERAL(65, 15),  // "onTcpConnection"
        QT_MOC_LITERAL(81, 14),  // "onTcpDataReady"
        QT_MOC_LITERAL(96, 14),  // "onUdpDataReady"
        QT_MOC_LITERAL(111, 17)   // "onTcpDisconnected"
    },
    "RawSocketCapture",
    "packetCaptured",
    "",
    "PacketInfo",
    "packet",
    "error",
    "message",
    "onTcpConnection",
    "onTcpDataReady",
    "onUdpDataReady",
    "onTcpDisconnected"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RawSocketCapture[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,
       5,    1,   53,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,   56,    2, 0x08,    5 /* Private */,
       8,    0,   57,    2, 0x08,    6 /* Private */,
       9,    0,   58,    2, 0x08,    7 /* Private */,
      10,    0,   59,    2, 0x08,    8 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RawSocketCapture::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_RawSocketCapture.offsetsAndSizes,
    qt_meta_data_RawSocketCapture,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RawSocketCapture_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RawSocketCapture, std::true_type>,
        // method 'packetCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const PacketInfo &, std::false_type>,
        // method 'error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onTcpConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTcpDataReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onUdpDataReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTcpDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RawSocketCapture::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RawSocketCapture *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->packetCaptured((*reinterpret_cast< std::add_pointer_t<PacketInfo>>(_a[1]))); break;
        case 1: _t->error((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onTcpConnection(); break;
        case 3: _t->onTcpDataReady(); break;
        case 4: _t->onUdpDataReady(); break;
        case 5: _t->onTcpDisconnected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RawSocketCapture::*)(const PacketInfo & );
            if (_t _q_method = &RawSocketCapture::packetCaptured; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RawSocketCapture::*)(const QString & );
            if (_t _q_method = &RawSocketCapture::error; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *RawSocketCapture::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RawSocketCapture::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RawSocketCapture.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RawSocketCapture::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void RawSocketCapture::packetCaptured(const PacketInfo & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RawSocketCapture::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
