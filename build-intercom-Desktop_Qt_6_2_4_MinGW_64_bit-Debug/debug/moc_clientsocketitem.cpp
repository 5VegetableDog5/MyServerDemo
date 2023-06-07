/****************************************************************************
** Meta object code from reading C++ file 'clientsocketitem.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../intercom/clientsocketitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clientsocketitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ClientSocketItem_t {
    const uint offsetsAndSize[26];
    char stringdata0[149];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ClientSocketItem_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ClientSocketItem_t qt_meta_stringdata_ClientSocketItem = {
    {
QT_MOC_LITERAL(0, 16), // "ClientSocketItem"
QT_MOC_LITERAL(17, 13), // "requestToSend"
QT_MOC_LITERAL(31, 0), // ""
QT_MOC_LITERAL(32, 4), // "data"
QT_MOC_LITERAL(37, 6), // "length"
QT_MOC_LITERAL(44, 18), // "onlineClientSingal"
QT_MOC_LITERAL(63, 6), // "ipAddr"
QT_MOC_LITERAL(70, 6), // "status"
QT_MOC_LITERAL(77, 13), // "statusChanged"
QT_MOC_LITERAL(91, 9), // "newStatus"
QT_MOC_LITERAL(101, 13), // "offLineSingal"
QT_MOC_LITERAL(115, 11), // "readTcpData"
QT_MOC_LITERAL(127, 21) // "handleCloseConnection"

    },
    "ClientSocketItem\0requestToSend\0\0data\0"
    "length\0onlineClientSingal\0ipAddr\0"
    "status\0statusChanged\0newStatus\0"
    "offLineSingal\0readTcpData\0"
    "handleCloseConnection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ClientSocketItem[] = {

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
       1,    2,   50,    2, 0x06,    1 /* Public */,
       5,    2,   55,    2, 0x06,    4 /* Public */,
       8,    2,   60,    2, 0x06,    7 /* Public */,
      10,    1,   65,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,   68,    2, 0x08,   12 /* Private */,
      12,    0,   69,    2, 0x08,   13 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Short,    6,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::Short,    6,    9,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ClientSocketItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ClientSocketItem *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->requestToSend((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->onlineClientSingal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<short>>(_a[2]))); break;
        case 2: _t->statusChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<short>>(_a[2]))); break;
        case 3: _t->offLineSingal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->readTcpData(); break;
        case 5: _t->handleCloseConnection(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ClientSocketItem::*)(const QByteArray , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClientSocketItem::requestToSend)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ClientSocketItem::*)(const QString , const short );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClientSocketItem::onlineClientSingal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ClientSocketItem::*)(const QString , const short );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClientSocketItem::statusChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ClientSocketItem::*)(const QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ClientSocketItem::offLineSingal)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject ClientSocketItem::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ClientSocketItem.offsetsAndSize,
    qt_meta_data_ClientSocketItem,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ClientSocketItem_t
, QtPrivate::TypeAndForceComplete<ClientSocketItem, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QByteArray, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString, std::false_type>, QtPrivate::TypeAndForceComplete<const short, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString, std::false_type>, QtPrivate::TypeAndForceComplete<const short, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *ClientSocketItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ClientSocketItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ClientSocketItem.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ClientSocketItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ClientSocketItem::requestToSend(const QByteArray _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClientSocketItem::onlineClientSingal(const QString _t1, const short _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ClientSocketItem::statusChanged(const QString _t1, const short _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ClientSocketItem::offLineSingal(const QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
