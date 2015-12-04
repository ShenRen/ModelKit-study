/****************************************************************************
** Meta object code from reading C++ file 'draw.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../draw.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'draw.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_draw_t {
    QByteArrayData data[6];
    char stringdata0[32];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_draw_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_draw_t qt_meta_stringdata_draw = {
    {
QT_MOC_LITERAL(0, 0, 4), // "draw"
QT_MOC_LITERAL(1, 5, 6), // "setPen"
QT_MOC_LITERAL(2, 12, 0), // ""
QT_MOC_LITERAL(3, 13, 3), // "pen"
QT_MOC_LITERAL(4, 17, 8), // "setBrush"
QT_MOC_LITERAL(5, 26, 5) // "brush"

    },
    "draw\0setPen\0\0pen\0setBrush\0brush"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_draw[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       4,    1,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QPen,    3,
    QMetaType::Void, QMetaType::QBrush,    5,

       0        // eod
};

void draw::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        draw *_t = static_cast<draw *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setPen((*reinterpret_cast< const QPen(*)>(_a[1]))); break;
        case 1: _t->setBrush((*reinterpret_cast< const QBrush(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject draw::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_draw.data,
      qt_meta_data_draw,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *draw::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *draw::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_draw.stringdata0))
        return static_cast<void*>(const_cast< draw*>(this));
    return QWidget::qt_metacast(_clname);
}

int draw::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
