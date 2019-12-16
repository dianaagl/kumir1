/****************************************************************************
** Meta object code from reading C++ file 'nxtdummyconnection.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "nxtdummyconnection.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nxtdummyconnection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NXTDummyConnection[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      34,   20,   19,   19, 0x0a,
      67,   58,   19,   19, 0x0a,
      83,   58,   19,   19, 0x0a,
     108,  100,   19,   19, 0x0a,
     123,  100,   19,   19, 0x0a,
     139,   19,   19,   19, 0x0a,
     159,  153,  148,   19, 0x0a,
     188,   19,   19,   19, 0x0a,
     205,   19,  197,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NXTDummyConnection[] = {
    "NXTDummyConnection\0\0speed,time_ms\0"
    "penMotorStart(int,uint)\0distance\0"
    "goForward(uint)\0goBackward(uint)\0"
    "degrees\0turnLeft(uint)\0turnRight(uint)\0"
    "doBeep()\0bool\0error\0isSensorPressedNow(QString&)\0"
    "isWall()\0QString\0reset()\0"
};

void NXTDummyConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NXTDummyConnection *_t = static_cast<NXTDummyConnection *>(_o);
        switch (_id) {
        case 0: _t->penMotorStart((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 1: _t->goForward((*reinterpret_cast< const uint(*)>(_a[1]))); break;
        case 2: _t->goBackward((*reinterpret_cast< const uint(*)>(_a[1]))); break;
        case 3: _t->turnLeft((*reinterpret_cast< const uint(*)>(_a[1]))); break;
        case 4: _t->turnRight((*reinterpret_cast< const uint(*)>(_a[1]))); break;
        case 5: _t->doBeep(); break;
        case 6: { bool _r = _t->isSensorPressedNow((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: _t->isWall(); break;
        case 8: { QString _r = _t->reset();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NXTDummyConnection::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NXTDummyConnection::staticMetaObject = {
    { &NXTConnection::staticMetaObject, qt_meta_stringdata_NXTDummyConnection,
      qt_meta_data_NXTDummyConnection, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NXTDummyConnection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NXTDummyConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NXTDummyConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NXTDummyConnection))
        return static_cast<void*>(const_cast< NXTDummyConnection*>(this));
    return NXTConnection::qt_metacast(_clname);
}

int NXTDummyConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NXTConnection::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
