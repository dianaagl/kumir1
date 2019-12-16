/****************************************************************************
** Meta object code from reading C++ file 'nxtcore.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "nxtcore.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nxtcore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NXTCore[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,    9,    8,    8, 0x05,
      44,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      88,   61,    8,    8, 0x08,
     134,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NXTCore[] = {
    "NXTCore\0\0adresses\0scanFinished(QStringList)\0"
    "connectionDone()\0processName,data,errorCode\0"
    "handleHciToolFinished(QString,QByteArray,int)\0"
    "handleConnectionRunnerFinished()\0"
};

void NXTCore::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NXTCore *_t = static_cast<NXTCore *>(_o);
        switch (_id) {
        case 0: _t->scanFinished((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 1: _t->connectionDone(); break;
        case 2: _t->handleHciToolFinished((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QByteArray(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->handleConnectionRunnerFinished(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NXTCore::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NXTCore::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NXTCore,
      qt_meta_data_NXTCore, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NXTCore::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NXTCore::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NXTCore::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NXTCore))
        return static_cast<void*>(const_cast< NXTCore*>(this));
    return QObject::qt_metacast(_clname);
}

int NXTCore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NXTCore::scanFinished(const QStringList & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NXTCore::connectionDone()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
