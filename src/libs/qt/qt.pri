INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
INCLUDEPATH += $$PWD/debug
DEPENDPATH += $$PWD/debug
INCLUDEPATH += $$PWD/ipc
DEPENDPATH += $$PWD/ipc
INCLUDEPATH += $$PWD/items
DEPENDPATH += $$PWD/items
INCLUDEPATH += $$PWD/plugins
DEPENDPATH += $$PWD/plugins
INCLUDEPATH += $$PWD/server
DEPENDPATH += $$PWD/server
INCLUDEPATH += $$PWD/utils
DEPENDPATH += $$PWD/utils
INCLUDEPATH += $$PWD/widgets
DEPENDPATH += $$PWD/widgets


include(../../../qimsys.pri)

LIBS *= -L$$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_LIBRARY_PATH

LIBS *= -l$$qtLibraryName(qimsys_qt$$QT_MAJOR_VERSION)

!contains(QIMSYS_CONFIG, no-dbus): QT += dbus
