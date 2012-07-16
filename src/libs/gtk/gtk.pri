INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD/ipc
DEPENDPATH += $$PWD/ipc
INCLUDEPATH += $$PWD/debug
DEPENDPATH += $$PWD/debug
include(../../../qimsys.pri)

CONFIG += link_pkgconfig
PKGCONFIG += dbus-glib-1

LIBS += -L$$QIMSYS_BUILD_TREE/$$QIMSYS_LIBRARY_PATH

LIBS *= -l$$qtLibraryName(qimsys_gtk)
