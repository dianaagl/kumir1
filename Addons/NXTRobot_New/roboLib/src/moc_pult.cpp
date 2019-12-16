/****************************************************************************
** Meta object code from reading C++ file 'pult.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "pult.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pult.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OvenTimer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,   11,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_OvenTimer[] = {
    "OvenTimer\0\0value\0angChange(int)\0"
    "setValue(int)\0"
};

void OvenTimer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        OvenTimer *_t = static_cast<OvenTimer *>(_o);
        switch (_id) {
        case 0: _t->angChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData OvenTimer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject OvenTimer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_OvenTimer,
      qt_meta_data_OvenTimer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OvenTimer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OvenTimer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OvenTimer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OvenTimer))
        return static_cast<void*>(const_cast< OvenTimer*>(this));
    return QWidget::qt_metacast(_clname);
}

int OvenTimer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void OvenTimer::angChange(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_linkLight[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_linkLight[] = {
    "linkLight\0"
};

void linkLight::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData linkLight::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject linkLight::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_linkLight,
      qt_meta_data_linkLight, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &linkLight::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *linkLight::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *linkLight::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_linkLight))
        return static_cast<void*>(const_cast< linkLight*>(this));
    return QWidget::qt_metacast(_clname);
}

int linkLight::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_MainButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      22,   11,   11,   11, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_MainButton[] = {
    "MainButton\0\0pressed()\0clicked()\0"
};

void MainButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainButton *_t = static_cast<MainButton *>(_o);
        switch (_id) {
        case 0: _t->pressed(); break;
        case 1: _t->clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MainButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainButton::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MainButton,
      qt_meta_data_MainButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainButton))
        return static_cast<void*>(const_cast< MainButton*>(this));
    return QWidget::qt_metacast(_clname);
}

int MainButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MainButton::pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void MainButton::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
static const uint qt_meta_data_loggerButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_loggerButton[] = {
    "loggerButton\0\0pressed()\0"
};

void loggerButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        loggerButton *_t = static_cast<loggerButton *>(_o);
        switch (_id) {
        case 0: _t->pressed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData loggerButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject loggerButton::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_loggerButton,
      qt_meta_data_loggerButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &loggerButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *loggerButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *loggerButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_loggerButton))
        return static_cast<void*>(const_cast< loggerButton*>(this));
    return QWidget::qt_metacast(_clname);
}

int loggerButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void loggerButton::pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_pultLogger[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      27,   11,   11,   11, 0x0a,
      44,   11,   11,   11, 0x0a,
      55,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_pultLogger[] = {
    "pultLogger\0\0upBtnPressed()\0downBtnPressed()\0"
    "ClearLog()\0CopyLog()\0"
};

void pultLogger::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        pultLogger *_t = static_cast<pultLogger *>(_o);
        switch (_id) {
        case 0: _t->upBtnPressed(); break;
        case 1: _t->downBtnPressed(); break;
        case 2: _t->ClearLog(); break;
        case 3: _t->CopyLog(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData pultLogger::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject pultLogger::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_pultLogger,
      qt_meta_data_pultLogger, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &pultLogger::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *pultLogger::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *pultLogger::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_pultLogger))
        return static_cast<void*>(const_cast< pultLogger*>(this));
    return QWidget::qt_metacast(_clname);
}

int pultLogger::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_NXTPult[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      17,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      16,    8,    8,    8, 0x05,
      25,    8,    8,    8, 0x05,
      34,    8,    8,    8, 0x05,
      44,    8,    8,    8, 0x05,
      58,    8,    8,    8, 0x05,
      73,    8,    8,    8, 0x05,
      84,    8,    8,    8, 0x05,
      97,    8,    8,    8, 0x05,
     110,    8,    8,    8, 0x05,
     124,    8,    8,    8, 0x05,
     132,    8,    8,    8, 0x05,
     140,    8,    8,    8, 0x05,
     146,    8,    8,    8, 0x05,
     153,    8,    8,    8, 0x05,
     167,  162,    8,    8, 0x05,
     184,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     202,    8,    8,    8, 0x0a,
     211,    8,    8,    8, 0x0a,
     220,    8,    8,    8, 0x0a,
     239,    8,    8,    8, 0x0a,
     258,    8,    8,    8, 0x0a,
     263,    8,    8,    8, 0x0a,
     270,    8,    8,    8, 0x0a,
     277,    8,    8,    8, 0x0a,
     285,    8,    8,    8, 0x0a,
     295,    8,    8,    8, 0x0a,
     308,    8,    8,    8, 0x0a,
     316,    8,    8,    8, 0x0a,
     323,    8,    8,    8, 0x0a,
     345,  337,    8,    8, 0x0a,
     366,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NXTPult[] = {
    "NXTPult\0\0goUp()\0goDown()\0goLeft()\0"
    "goRight()\0hasLeftWall()\0hasRightWall()\0"
    "noUpWall()\0noDownWall()\0noLeftWall()\0"
    "noRightWall()\0Color()\0Clean()\0Rad()\0"
    "Temp()\0logToK()\0text\0PultCmd(QString)\0"
    "sendText(QString)\0noLink()\0LinkOK()\0"
    "newClient(QString)\0clientDisconnect()\0"
    "Up()\0Down()\0Left()\0Right()\0SwStena()\0"
    "SwSvobodno()\0TempS()\0RadS()\0resetTurtle()\0"
    "message\0showMessage(QString)\0logToKumir()\0"
};

void NXTPult::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NXTPult *_t = static_cast<NXTPult *>(_o);
        switch (_id) {
        case 0: _t->goUp(); break;
        case 1: _t->goDown(); break;
        case 2: _t->goLeft(); break;
        case 3: _t->goRight(); break;
        case 4: _t->hasLeftWall(); break;
        case 5: _t->hasRightWall(); break;
        case 6: _t->noUpWall(); break;
        case 7: _t->noDownWall(); break;
        case 8: _t->noLeftWall(); break;
        case 9: _t->noRightWall(); break;
        case 10: _t->Color(); break;
        case 11: _t->Clean(); break;
        case 12: _t->Rad(); break;
        case 13: _t->Temp(); break;
        case 14: _t->logToK(); break;
        case 15: _t->PultCmd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: _t->sendText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 17: _t->noLink(); break;
        case 18: _t->LinkOK(); break;
        case 19: _t->newClient((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: _t->clientDisconnect(); break;
        case 21: _t->Up(); break;
        case 22: _t->Down(); break;
        case 23: _t->Left(); break;
        case 24: _t->Right(); break;
        case 25: _t->SwStena(); break;
        case 26: _t->SwSvobodno(); break;
        case 27: _t->TempS(); break;
        case 28: _t->RadS(); break;
        case 29: _t->resetTurtle(); break;
        case 30: _t->showMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 31: _t->logToKumir(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NXTPult::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NXTPult::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NXTPult,
      qt_meta_data_NXTPult, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NXTPult::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NXTPult::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NXTPult::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NXTPult))
        return static_cast<void*>(const_cast< NXTPult*>(this));
    if (!strcmp(_clname, "Ui::TurtlePult"))
        return static_cast< Ui::TurtlePult*>(const_cast< NXTPult*>(this));
    return QWidget::qt_metacast(_clname);
}

int NXTPult::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 32)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    }
    return _id;
}

// SIGNAL 0
void NXTPult::goUp()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void NXTPult::goDown()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void NXTPult::goLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void NXTPult::goRight()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void NXTPult::hasLeftWall()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void NXTPult::hasRightWall()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void NXTPult::noUpWall()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void NXTPult::noDownWall()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void NXTPult::noLeftWall()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void NXTPult::noRightWall()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void NXTPult::Color()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void NXTPult::Clean()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void NXTPult::Rad()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void NXTPult::Temp()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void NXTPult::logToK()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void NXTPult::PultCmd(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void NXTPult::sendText(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}
QT_END_MOC_NAMESPACE
