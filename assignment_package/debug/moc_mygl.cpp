/****************************************************************************
** Meta object code from reading C++ file 'mygl.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/mygl.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mygl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMyGLENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMyGLENDCLASS = QtMocHelpers::stringData(
    "MyGL",
    "sig_sendPlayerPos",
    "",
    "sig_sendPlayerVel",
    "sig_sendPlayerAcc",
    "sig_sendPlayerLook",
    "sig_sendPlayerChunk",
    "sig_sendPlayerTerrainZone",
    "sig_sendFPS",
    "sig_sendAveFPS",
    "sig_enableLoadingBanner",
    "sig_sendLoadingStatus",
    "tick"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMyGLENDCLASS_t {
    uint offsetsAndSizes[26];
    char stringdata0[5];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[18];
    char stringdata5[19];
    char stringdata6[20];
    char stringdata7[26];
    char stringdata8[12];
    char stringdata9[15];
    char stringdata10[24];
    char stringdata11[22];
    char stringdata12[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMyGLENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMyGLENDCLASS_t qt_meta_stringdata_CLASSMyGLENDCLASS = {
    {
        QT_MOC_LITERAL(0, 4),  // "MyGL"
        QT_MOC_LITERAL(5, 17),  // "sig_sendPlayerPos"
        QT_MOC_LITERAL(23, 0),  // ""
        QT_MOC_LITERAL(24, 17),  // "sig_sendPlayerVel"
        QT_MOC_LITERAL(42, 17),  // "sig_sendPlayerAcc"
        QT_MOC_LITERAL(60, 18),  // "sig_sendPlayerLook"
        QT_MOC_LITERAL(79, 19),  // "sig_sendPlayerChunk"
        QT_MOC_LITERAL(99, 25),  // "sig_sendPlayerTerrainZone"
        QT_MOC_LITERAL(125, 11),  // "sig_sendFPS"
        QT_MOC_LITERAL(137, 14),  // "sig_sendAveFPS"
        QT_MOC_LITERAL(152, 23),  // "sig_enableLoadingBanner"
        QT_MOC_LITERAL(176, 21),  // "sig_sendLoadingStatus"
        QT_MOC_LITERAL(198, 4)   // "tick"
    },
    "MyGL",
    "sig_sendPlayerPos",
    "",
    "sig_sendPlayerVel",
    "sig_sendPlayerAcc",
    "sig_sendPlayerLook",
    "sig_sendPlayerChunk",
    "sig_sendPlayerTerrainZone",
    "sig_sendFPS",
    "sig_sendAveFPS",
    "sig_enableLoadingBanner",
    "sig_sendLoadingStatus",
    "tick"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMyGLENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   80,    2, 0x106,    1 /* Public | MethodIsConst  */,
       3,    1,   83,    2, 0x106,    3 /* Public | MethodIsConst  */,
       4,    1,   86,    2, 0x106,    5 /* Public | MethodIsConst  */,
       5,    1,   89,    2, 0x106,    7 /* Public | MethodIsConst  */,
       6,    1,   92,    2, 0x106,    9 /* Public | MethodIsConst  */,
       7,    1,   95,    2, 0x106,   11 /* Public | MethodIsConst  */,
       8,    1,   98,    2, 0x106,   13 /* Public | MethodIsConst  */,
       9,    1,  101,    2, 0x106,   15 /* Public | MethodIsConst  */,
      10,    1,  104,    2, 0x106,   17 /* Public | MethodIsConst  */,
      11,    1,  107,    2, 0x106,   19 /* Public | MethodIsConst  */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    0,  110,    2, 0x08,   21 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MyGL::staticMetaObject = { {
    QMetaObject::SuperData::link<OpenGLContext::staticMetaObject>(),
    qt_meta_stringdata_CLASSMyGLENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMyGLENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMyGLENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MyGL, std::true_type>,
        // method 'sig_sendPlayerPos'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sig_sendPlayerVel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sig_sendPlayerAcc'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sig_sendPlayerLook'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sig_sendPlayerChunk'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sig_sendPlayerTerrainZone'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sig_sendFPS'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sig_sendAveFPS'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sig_enableLoadingBanner'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'sig_sendLoadingStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'tick'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MyGL::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyGL *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sig_sendPlayerPos((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->sig_sendPlayerVel((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->sig_sendPlayerAcc((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->sig_sendPlayerLook((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->sig_sendPlayerChunk((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->sig_sendPlayerTerrainZone((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->sig_sendFPS((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->sig_sendAveFPS((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->sig_enableLoadingBanner((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->sig_sendLoadingStatus((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->tick(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendPlayerPos; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendPlayerVel; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendPlayerAcc; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendPlayerLook; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendPlayerChunk; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendPlayerTerrainZone; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendFPS; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendAveFPS; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(bool ) const;
            if (_t _q_method = &MyGL::sig_enableLoadingBanner; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (MyGL::*)(QString ) const;
            if (_t _q_method = &MyGL::sig_sendLoadingStatus; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject *MyGL::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyGL::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMyGLENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return OpenGLContext::qt_metacast(_clname);
}

int MyGL::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OpenGLContext::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void MyGL::sig_sendPlayerPos(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MyGL::sig_sendPlayerVel(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MyGL::sig_sendPlayerAcc(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MyGL::sig_sendPlayerLook(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MyGL::sig_sendPlayerChunk(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MyGL::sig_sendPlayerTerrainZone(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 5, _a);
}

// SIGNAL 6
void MyGL::sig_sendFPS(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 6, _a);
}

// SIGNAL 7
void MyGL::sig_sendAveFPS(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 7, _a);
}

// SIGNAL 8
void MyGL::sig_enableLoadingBanner(bool _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 8, _a);
}

// SIGNAL 9
void MyGL::sig_sendLoadingStatus(QString _t1)const
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(const_cast< MyGL *>(this), &staticMetaObject, 9, _a);
}
QT_WARNING_POP
