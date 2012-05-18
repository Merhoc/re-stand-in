/****************************************************************************
** Meta object code from reading C++ file 'rsi.h'
**
** Created: Tue 15. May 22:25:54 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../rsi.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rsi.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_rsi[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       5,    4,    4,    4, 0x08,
      17,    4,    4,    4, 0x08,
      30,    4,    4,    4, 0x08,
      46,    4,    4,    4, 0x08,
      55,    4,    4,    4, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_rsi[] = {
    "rsi\0\0choose_uw()\0choose_uw2()\0"
    "choose_muster()\0accept()\0quit()\0"
};

void rsi::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        rsi *_t = static_cast<rsi *>(_o);
        switch (_id) {
        case 0: _t->choose_uw(); break;
        case 1: _t->choose_uw2(); break;
        case 2: _t->choose_muster(); break;
        case 3: _t->accept(); break;
        case 4: _t->quit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData rsi::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject rsi::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_rsi,
      qt_meta_data_rsi, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &rsi::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *rsi::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *rsi::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_rsi))
        return static_cast<void*>(const_cast< rsi*>(this));
    if (!strcmp(_clname, "Ui::rsi"))
        return static_cast< Ui::rsi*>(const_cast< rsi*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int rsi::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
