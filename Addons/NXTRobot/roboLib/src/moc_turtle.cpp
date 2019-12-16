/****************************************************************************
** Meta object code from reading C++ file 'turtle.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "turtle.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'turtle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NXTSetup[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   10,    9,    9, 0x05,
      29,    9,    9,    9, 0x05,
      42,   34,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      69,    9,    9,    9, 0x0a,
      83,    9,   78,    9, 0x0a,
      97,   91,   78,    9, 0x0a,
     123,   91,   78,    9, 0x0a,
     150,   34,    9,    9, 0x0a,
     179,  175,    9,    9, 0x0a,
     198,  193,    9,    9, 0x0a,
     222,  211,    9,    9, 0x0a,
     246,  240,    9,    9, 0x0a,
     258,    9,    9,    9, 0x0a,
     273,    9,    9,    9, 0x0a,
     281,    9,    9,    9, 0x0a,
     298,  289,    9,    9, 0x0a,
     312,  289,    9,    9, 0x0a,
     330,  326,    9,    9, 0x0a,
     349,    9,    9,    9, 0x0a,
     362,    9,    9,    9, 0x0a,
     378,    9,    9,    9, 0x0a,
     393,  385,    9,    9, 0x0a,
     419,    9,    9,    9, 0x0a,
     426,    9,    9,    9, 0x0a,
     434,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NXTSetup[] = {
    "NXTSetup\0\0msg\0Otkaz(QString)\0Ok()\0"
    "err,val\0NXTReady(QString,QVariant)\0"
    "rotate()\0bool\0moveT()\0a,b,c\0"
    "runEngine(bool,bool,bool)\0"
    "stopEngine(bool,bool,bool)\0"
    "NXTFin(QString,QVariant)\0deg\0DoRotate(int)\0"
    "msec\0DoSleep(int)\0time,speed\0"
    "DoMotorC(int,int)\0range\0DoMove(int)\0"
    "CheckForWall()\0range()\0light()\0motor_id\0"
    "rotation(int)\0resetRot(int)\0pos\0"
    "hideTurtle(QPoint)\0connectNXT()\0"
    "disconnectNXT()\0scan()\0devices\0"
    "scanFinished(QStringList)\0beep()\0"
    "touch()\0storeValuesInSettings()\0"
};

void NXTSetup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NXTSetup *_t = static_cast<NXTSetup *>(_o);
        switch (_id) {
        case 0: _t->Otkaz((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->Ok(); break;
        case 2: _t->NXTReady((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 3: _t->rotate(); break;
        case 4: { bool _r = _t->moveT();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: { bool _r = _t->runEngine((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { bool _r = _t->stopEngine((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: _t->NXTFin((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 8: _t->DoRotate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->DoSleep((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->DoMotorC((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->DoMove((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->CheckForWall(); break;
        case 13: _t->range(); break;
        case 14: _t->light(); break;
        case 15: _t->rotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->resetRot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->hideTurtle((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 18: _t->connectNXT(); break;
        case 19: _t->disconnectNXT(); break;
        case 20: _t->scan(); break;
        case 21: _t->scanFinished((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 22: _t->beep(); break;
        case 23: _t->touch(); break;
        case 24: _t->storeValuesInSettings(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NXTSetup::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NXTSetup::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NXTSetup,
      qt_meta_data_NXTSetup, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NXTSetup::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NXTSetup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NXTSetup::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NXTSetup))
        return static_cast<void*>(const_cast< NXTSetup*>(this));
    return QWidget::qt_metacast(_clname);
}

int NXTSetup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void NXTSetup::Otkaz(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NXTSetup::Ok()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void NXTSetup::NXTReady(QString _t1, QVariant _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
