/****************************************************************************
** Meta object code from reading C++ file 'painter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "painter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'painter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Manta__Painter_t {
    QByteArrayData data[9];
    char stringdata0[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Manta__Painter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Manta__Painter_t qt_meta_stringdata_Manta__Painter = {
    {
QT_MOC_LITERAL(0, 0, 14), // "Manta::Painter"
QT_MOC_LITERAL(1, 15, 11), // "setViewport"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 5), // "Vec3i"
QT_MOC_LITERAL(4, 34, 8), // "gridsize"
QT_MOC_LITERAL(5, 43, 5), // "paint"
QT_MOC_LITERAL(6, 49, 7), // "doEvent"
QT_MOC_LITERAL(7, 57, 1), // "e"
QT_MOC_LITERAL(8, 59, 5) // "param"

    },
    "Manta::Painter\0setViewport\0\0Vec3i\0"
    "gridsize\0paint\0doEvent\0e\0param"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Manta__Painter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   37,    2, 0x0a /* Public */,
       6,    2,   38,    2, 0x0a /* Public */,
       6,    1,   43,    2, 0x2a /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    7,    8,
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void Manta::Painter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Painter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setViewport((*reinterpret_cast< const Vec3i(*)>(_a[1]))); break;
        case 1: _t->paint(); break;
        case 2: _t->doEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->doEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Painter::*)(const Vec3i & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Painter::setViewport)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Manta::Painter::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Manta__Painter.data,
    qt_meta_data_Manta__Painter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Manta::Painter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Manta::Painter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Manta__Painter.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Manta::Painter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Manta::Painter::setViewport(const Vec3i & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_Manta__LockedObjPainter_t {
    QByteArrayData data[1];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Manta__LockedObjPainter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Manta__LockedObjPainter_t qt_meta_stringdata_Manta__LockedObjPainter = {
    {
QT_MOC_LITERAL(0, 0, 23) // "Manta::LockedObjPainter"

    },
    "Manta::LockedObjPainter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Manta__LockedObjPainter[] = {

 // content:
       8,       // revision
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

void Manta::LockedObjPainter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Manta::LockedObjPainter::staticMetaObject = { {
    &Painter::staticMetaObject,
    qt_meta_stringdata_Manta__LockedObjPainter.data,
    qt_meta_data_Manta__LockedObjPainter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Manta::LockedObjPainter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Manta::LockedObjPainter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Manta__LockedObjPainter.stringdata0))
        return static_cast<void*>(this);
    return Painter::qt_metacast(_clname);
}

int Manta::LockedObjPainter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Painter::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
