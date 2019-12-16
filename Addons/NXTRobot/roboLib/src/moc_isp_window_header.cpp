/****************************************************************************
** Meta object code from reading C++ file 'isp_window_header.h'
**
** Created
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../isp_window_header.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'isp_window_header.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WHeader[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      25,    9,    8,    8, 0x05,
      54,   48,    8,    8, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_WHeader[] = {
    "WHeader\0\0oldSize,newSize\0"
    "Resized(QSize*,QSize*)\0point\0"
    "mousePress(QPoint)\0"
};

const QMetaObject WHeader::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WHeader,
      qt_meta_data_WHeader, 0 }
};

const QMetaObject *WHeader::metaObject() const
{
    return &staticMetaObject;
}

void *WHeader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WHeader))
        return static_cast<void*>(const_cast< WHeader*>(this));
    return QWidget::qt_metacast(_clname);
}

int WHeader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: Resized((*reinterpret_cast< QSize*(*)>(_a[1])),(*reinterpret_cast< QSize*(*)>(_a[2]))); break;
        case 1: mousePress((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void WHeader::Resized(QSize * _t1, QSize * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WHeader::mousePress(QPoint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
