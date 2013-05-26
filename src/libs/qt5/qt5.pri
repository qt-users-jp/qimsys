INCLUDEPATH += $$PWD/../qt4
DEPENDPATH += $$PWD/../qt4
INCLUDEPATH += $$PWD/../qt4/debug
DEPENDPATH += $$PWD/../qt4/debug
INCLUDEPATH += $$PWD/ipc
DEPENDPATH += $$PWD/ipc
INCLUDEPATH += $$PWD/items
DEPENDPATH += $$PWD/items

include(../../../qimsys.pri)

LIBS *= -L$$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_LIBRARY_PATH

LIBS *= -l$$qtLibraryName(qimsys_qt5)

!contains(QIMSYS_CONFIG, no-dbus): QT += dbus
