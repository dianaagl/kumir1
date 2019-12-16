/****************************************************************************
** Meta object code from reading C++ file 'network.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "network.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'network.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SignalCrosser[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x05,
      30,   15,   14,   14, 0x05,
      45,   14,   14,   14, 0x05,
      57,   14,   14,   14, 0x05,
      71,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      78,   14,   14,   14, 0x0a,
      93,   83,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SignalCrosser[] = {
    "SignalCrosser\0\0d\0do_move(int)\0"
    "do_rotate(int)\0do_tailUp()\0do_tailDown()\0"
    "Sync()\0OK()\0module_id\0LostIsp(int)\0"
};

void SignalCrosser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SignalCrosser *_t = static_cast<SignalCrosser *>(_o);
        switch (_id) {
        case 0: _t->do_move((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->do_rotate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->do_tailUp(); break;
        case 3: _t->do_tailDown(); break;
        case 4: _t->Sync(); break;
        case 5: _t->OK(); break;
        case 6: _t->LostIsp((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SignalCrosser::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SignalCrosser::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SignalCrosser,
      qt_meta_data_SignalCrosser, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SignalCrosser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SignalCrosser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SignalCrosser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SignalCrosser))
        return static_cast<void*>(const_cast< SignalCrosser*>(this));
    return QObject::qt_metacast(_clname);
}

int SignalCrosser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void SignalCrosser::do_move(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SignalCrosser::do_rotate(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SignalCrosser::do_tailUp()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void SignalCrosser::do_tailDown()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void SignalCrosser::Sync()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
static const uint qt_meta_data_KNPConnection[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      28,   23,   14,   14, 0x05,
      43,   23,   14,   14, 0x05,
      67,   14,   14,   14, 0x05,
      81,   14,   14,   14, 0x05,
      95,   89,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     114,   23,   14,   14, 0x0a,
     131,   14,   14,   14, 0x08,
     154,  142,   14,   14, 0x08,
     196,   14,   14,   14, 0x08,
     208,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_KNPConnection[] = {
    "KNPConnection\0\0Ready()\0text\0Error(QString)\0"
    "GetModuleError(QString)\0getFuncList()\0"
    "getOK()\0value\0getReturn(QString)\0"
    "sendCmd(QString)\0readData()\0socketError\0"
    "socketError(QAbstractSocket::SocketError)\0"
    "Connected()\0Disconnected()\0"
};

void KNPConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KNPConnection *_t = static_cast<KNPConnection *>(_o);
        switch (_id) {
        case 0: _t->Ready(); break;
        case 1: _t->Error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->GetModuleError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->getFuncList(); break;
        case 4: _t->getOK(); break;
        case 5: _t->getReturn((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->sendCmd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->readData(); break;
        case 8: _t->socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 9: _t->Connected(); break;
        case 10: _t->Disconnected(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KNPConnection::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KNPConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KNPConnection,
      qt_meta_data_KNPConnection, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KNPConnection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KNPConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KNPConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KNPConnection))
        return static_cast<void*>(const_cast< KNPConnection*>(this));
    return QObject::qt_metacast(_clname);
}

int KNPConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void KNPConnection::Ready()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void KNPConnection::Error(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KNPConnection::GetModuleError(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KNPConnection::getFuncList()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void KNPConnection::getOK()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void KNPConnection::getReturn(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
static const uint qt_meta_data_KNPServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      24,   19,   10,   10, 0x05,
      39,   19,   10,   10, 0x05,
      58,   10,   10,   10, 0x05,
      68,   10,   10,   10, 0x05,
      80,   10,   10,   10, 0x05,
      99,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     107,   19,   10,   10, 0x0a,
     146,  134,   10,   10, 0x08,
     188,   10,   10,   10, 0x08,
     206,   10,   10,   10, 0x08,
     227,   10,   10,   10, 0x08,
     243,   10,   10,   10, 0x08,
     277,  262,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_KNPServer[] = {
    "KNPServer\0\0Ready()\0text\0Error(QString)\0"
    "newClient(QString)\0lockGui()\0unlockGui()\0"
    "clientDisconnect()\0reset()\0"
    "sendCmdAllClients(QString)\0socketError\0"
    "socketError(QAbstractSocket::SocketError)\0"
    "ClientConnected()\0ClientDisconnected()\0"
    "reciveMessage()\0deleteConnection()\0"
    "text,client_id\0servReplay(QString,int)\0"
};

void KNPServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KNPServer *_t = static_cast<KNPServer *>(_o);
        switch (_id) {
        case 0: _t->Ready(); break;
        case 1: _t->Error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->newClient((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->lockGui(); break;
        case 4: _t->unlockGui(); break;
        case 5: _t->clientDisconnect(); break;
        case 6: _t->reset(); break;
        case 7: _t->sendCmdAllClients((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 9: _t->ClientConnected(); break;
        case 10: _t->ClientDisconnected(); break;
        case 11: _t->reciveMessage(); break;
        case 12: _t->deleteConnection(); break;
        case 13: _t->servReplay((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KNPServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KNPServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KNPServer,
      qt_meta_data_KNPServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KNPServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KNPServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KNPServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KNPServer))
        return static_cast<void*>(const_cast< KNPServer*>(this));
    return QObject::qt_metacast(_clname);
}

int KNPServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void KNPServer::Ready()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void KNPServer::Error(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KNPServer::newClient(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KNPServer::lockGui()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void KNPServer::unlockGui()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void KNPServer::clientDisconnect()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void KNPServer::reset()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
