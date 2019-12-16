/****************************************************************************
** Meta object code from reading C++ file 'nxtconnection.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "nxtconnection.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nxtconnection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NXTConnection[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      75,   61,   14,   14, 0x0a,
     126,   99,   14,   14, 0x0a,
     171,  165,   14,   14, 0x0a,
     206,  197,   14,   14, 0x0a,
     222,  197,   14,   14, 0x0a,
     247,  239,   14,   14, 0x0a,
     262,  239,   14,   14, 0x0a,
     289,  278,   14,   14, 0x0a,
     305,   14,   14,   14, 0x0a,
     314,   14,   14,   14, 0x0a,
     333,  327,  322,   14, 0x0a,
     362,   14,   14,   14, 0x0a,
     375,   14,  371,   14, 0x0a,
     383,   14,  371,   14, 0x0a,
     400,  391,   14,   14, 0x0a,
     414,  391,   14,   14, 0x0a,
     433,  428,   14,   14, 0x0a,
     458,   14,  450,   14, 0x0a,
     466,   15,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_NXTConnection[] = {
    "NXTConnection\0\0error,value\0"
    "commandFinished(QString,QVariant)\0"
    "speed,time_ms\0penMotorStart(int,uint)\0"
    "speeda,speedb,speedc,a,b,c\0"
    "MotorStart(int,int,int,bool,bool,bool)\0"
    "a,b,c\0MotorStop(bool,bool,bool)\0"
    "distance\0goForward(uint)\0goBackward(uint)\0"
    "degrees\0turnLeft(uint)\0turnRight(uint)\0"
    "time,speed\0motorC(int,int)\0doBeep()\0"
    "touch()\0bool\0error\0isSensorPressedNow(QString&)\0"
    "isWall()\0int\0range()\0light()\0motor_id\0"
    "rotation(int)\0resetRot(int)\0time\0"
    "sleep_m_sec(int)\0QString\0reset()\0"
    "handleReplyFromThread(QString,QVariant)\0"
};

void NXTConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NXTConnection *_t = static_cast<NXTConnection *>(_o);
        switch (_id) {
        case 0: _t->commandFinished((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: _t->penMotorStart((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 2: _t->MotorStart((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6]))); break;
        case 3: _t->MotorStop((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 4: _t->goForward((*reinterpret_cast< const uint(*)>(_a[1]))); break;
        case 5: _t->goBackward((*reinterpret_cast< const uint(*)>(_a[1]))); break;
        case 6: _t->turnLeft((*reinterpret_cast< const uint(*)>(_a[1]))); break;
        case 7: _t->turnRight((*reinterpret_cast< const uint(*)>(_a[1]))); break;
        case 8: _t->motorC((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->doBeep(); break;
        case 10: _t->touch(); break;
        case 11: { bool _r = _t->isSensorPressedNow((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: _t->isWall(); break;
        case 13: { int _r = _t->range();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 14: { int _r = _t->light();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 15: _t->rotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->resetRot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->sleep_m_sec((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: { QString _r = _t->reset();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 19: _t->handleReplyFromThread((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NXTConnection::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NXTConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NXTConnection,
      qt_meta_data_NXTConnection, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NXTConnection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NXTConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NXTConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NXTConnection))
        return static_cast<void*>(const_cast< NXTConnection*>(this));
    return QObject::qt_metacast(_clname);
}

int NXTConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void NXTConnection::commandFinished(const QString & _t1, QVariant _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
