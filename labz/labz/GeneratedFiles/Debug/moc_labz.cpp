/****************************************************************************
** Meta object code from reading C++ file 'labz.h'
**
** Created: Mon 19. Mar 22:22:38 2018
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../labz.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'labz.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_labz[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x0a,
      11,    5,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_labz[] = {
    "labz\0\0go()\0attack()\0"
};

void labz::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        labz *_t = static_cast<labz *>(_o);
        switch (_id) {
        case 0: _t->go(); break;
        case 1: _t->attack(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData labz::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject labz::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_labz,
      qt_meta_data_labz, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &labz::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *labz::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *labz::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_labz))
        return static_cast<void*>(const_cast< labz*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int labz::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
