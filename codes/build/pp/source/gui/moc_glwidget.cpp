/****************************************************************************
** Meta object code from reading C++ file 'glwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "glwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Manta__GLWidget_t {
    QByteArrayData data[23];
    char stringdata0[162];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Manta__GLWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Manta__GLWidget_t qt_meta_stringdata_Manta__GLWidget = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Manta::GLWidget"
QT_MOC_LITERAL(1, 16, 8), // "paintSub"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 9), // "clickLine"
QT_MOC_LITERAL(4, 36, 3), // "pos"
QT_MOC_LITERAL(5, 40, 2), // "p0"
QT_MOC_LITERAL(6, 43, 2), // "p1"
QT_MOC_LITERAL(7, 46, 2), // "p2"
QT_MOC_LITERAL(8, 49, 2), // "q0"
QT_MOC_LITERAL(9, 52, 2), // "q1"
QT_MOC_LITERAL(10, 55, 2), // "q2"
QT_MOC_LITERAL(11, 58, 12), // "painterEvent"
QT_MOC_LITERAL(12, 71, 1), // "e"
QT_MOC_LITERAL(13, 73, 5), // "param"
QT_MOC_LITERAL(14, 79, 11), // "setViewport"
QT_MOC_LITERAL(15, 91, 5), // "Vec3i"
QT_MOC_LITERAL(16, 97, 8), // "gridsize"
QT_MOC_LITERAL(17, 106, 13), // "keyPressEvent"
QT_MOC_LITERAL(18, 120, 10), // "QKeyEvent*"
QT_MOC_LITERAL(19, 131, 15), // "keyReleaseEvent"
QT_MOC_LITERAL(20, 147, 10), // "windowSize"
QT_MOC_LITERAL(21, 158, 1), // "w"
QT_MOC_LITERAL(22, 160, 1) // "h"

    },
    "Manta::GLWidget\0paintSub\0\0clickLine\0"
    "pos\0p0\0p1\0p2\0q0\0q1\0q2\0painterEvent\0e\0"
    "param\0setViewport\0Vec3i\0gridsize\0"
    "keyPressEvent\0QKeyEvent*\0keyReleaseEvent\0"
    "windowSize\0w\0h"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Manta__GLWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    7,   55,    2, 0x06 /* Public */,
      11,    2,   70,    2, 0x06 /* Public */,
      11,    1,   75,    2, 0x26 /* Public | MethodCloned */,

 // slots: name, argc, parameters, tag, flags
      14,    1,   78,    2, 0x0a /* Public */,
      17,    1,   81,    2, 0x0a /* Public */,
      19,    1,   84,    2, 0x0a /* Public */,
      20,    2,   87,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,    4,    5,    6,    7,    8,    9,   10,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   12,   13,
    QMetaType::Void, QMetaType::Int,   12,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 18,   12,
    QMetaType::Void, 0x80000000 | 18,   12,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   21,   22,

       0        // eod
};

void Manta::GLWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GLWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->paintSub(); break;
        case 1: _t->clickLine((*reinterpret_cast< QPoint(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< float(*)>(_a[7]))); break;
        case 2: _t->painterEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->painterEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setViewport((*reinterpret_cast< const Vec3i(*)>(_a[1]))); break;
        case 5: _t->keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 6: _t->keyReleaseEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 7: _t->windowSize((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GLWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GLWidget::paintSub)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GLWidget::*)(QPoint , float , float , float , float , float , float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GLWidget::clickLine)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GLWidget::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GLWidget::painterEvent)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Manta::GLWidget::staticMetaObject = { {
    &QGLWidget::staticMetaObject,
    qt_meta_stringdata_Manta__GLWidget.data,
    qt_meta_data_Manta__GLWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Manta::GLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Manta::GLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Manta__GLWidget.stringdata0))
        return static_cast<void*>(this);
    return QGLWidget::qt_metacast(_clname);
}

int Manta::GLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Manta::GLWidget::paintSub()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Manta::GLWidget::clickLine(QPoint _t1, float _t2, float _t3, float _t4, float _t5, float _t6, float _t7)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Manta::GLWidget::painterEvent(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
