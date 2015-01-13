/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../QPLCsim/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[26];
    char stringdata[249];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 7),
QT_MOC_LITERAL(2, 19, 0),
QT_MOC_LITERAL(3, 20, 4),
QT_MOC_LITERAL(4, 25, 8),
QT_MOC_LITERAL(5, 34, 8),
QT_MOC_LITERAL(6, 43, 4),
QT_MOC_LITERAL(7, 48, 6),
QT_MOC_LITERAL(8, 55, 8),
QT_MOC_LITERAL(9, 64, 7),
QT_MOC_LITERAL(10, 72, 11),
QT_MOC_LITERAL(11, 84, 7),
QT_MOC_LITERAL(12, 92, 9),
QT_MOC_LITERAL(13, 102, 11),
QT_MOC_LITERAL(14, 114, 5),
QT_MOC_LITERAL(15, 120, 4),
QT_MOC_LITERAL(16, 125, 14),
QT_MOC_LITERAL(17, 140, 17),
QT_MOC_LITERAL(18, 158, 5),
QT_MOC_LITERAL(19, 164, 7),
QT_MOC_LITERAL(20, 172, 12),
QT_MOC_LITERAL(21, 185, 8),
QT_MOC_LITERAL(22, 194, 15),
QT_MOC_LITERAL(23, 210, 14),
QT_MOC_LITERAL(24, 225, 10),
QT_MOC_LITERAL(25, 236, 11)
    },
    "MainWindow\0newFile\0\0open\0loadFile\0"
    "fileName\0save\0saveAs\0saveFile\0openBin\0"
    "loadBinFile\0saveBin\0saveBinAs\0saveBinFile\0"
    "about\0info\0openRecentFile\0openRecentBinFile\0"
    "build\0plcLoad\0plcStartStop\0plcClear\0"
    "updateStatusBar\0scriptModified\0"
    "openToLoad\0buildToLoad\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  129,    2, 0x08,
       3,    0,  130,    2, 0x08,
       4,    1,  131,    2, 0x08,
       6,    0,  134,    2, 0x08,
       7,    0,  135,    2, 0x08,
       8,    1,  136,    2, 0x08,
       9,    0,  139,    2, 0x08,
      10,    1,  140,    2, 0x08,
      11,    0,  143,    2, 0x08,
      12,    0,  144,    2, 0x08,
      13,    1,  145,    2, 0x08,
      14,    0,  148,    2, 0x08,
      15,    0,  149,    2, 0x08,
      16,    0,  150,    2, 0x08,
      17,    0,  151,    2, 0x08,
      18,    0,  152,    2, 0x08,
      19,    0,  153,    2, 0x08,
      20,    0,  154,    2, 0x08,
      21,    0,  155,    2, 0x08,
      22,    0,  156,    2, 0x08,
      23,    0,  157,    2, 0x08,
      24,    0,  158,    2, 0x08,
      25,    0,  159,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::QString,    5,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::QString,    5,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::QString,    5,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->newFile(); break;
        case 1: { bool _r = _t->open();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = _t->loadFile((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = _t->save();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: { bool _r = _t->saveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: { bool _r = _t->saveFile((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { bool _r = _t->openBin();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: { bool _r = _t->loadBinFile((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: { bool _r = _t->saveBin();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 9: { bool _r = _t->saveBinAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: { bool _r = _t->saveBinFile((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: _t->about(); break;
        case 12: _t->info(); break;
        case 13: _t->openRecentFile(); break;
        case 14: _t->openRecentBinFile(); break;
        case 15: _t->build(); break;
        case 16: _t->plcLoad(); break;
        case 17: _t->plcStartStop(); break;
        case 18: _t->plcClear(); break;
        case 19: _t->updateStatusBar(); break;
        case 20: _t->scriptModified(); break;
        case 21: _t->openToLoad(); break;
        case 22: _t->buildToLoad(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
