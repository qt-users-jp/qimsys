INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD/debug
DEPENDPATH += $$PWD/debug
INCLUDEPATH += $$PWD/ipc
DEPENDPATH += $$PWD/ipc
INCLUDEPATH += $$PWD/plugins
DEPENDPATH += $$PWD/plugins
INCLUDEPATH += $$PWD/items
DEPENDPATH += $$PWD/items
INCLUDEPATH += $$PWD/utils
DEPENDPATH += $$PWD/utils
INCLUDEPATH += $$PWD/server
DEPENDPATH += $$PWD/server

!contains(QIMSYS_CONFIG, no-gui) {
    INCLUDEPATH += $$PWD/widgets
    DEPENDPATH += $$PWD/widgets
    versionCheck(4,7,0) {
        QT += declarative
    }
}

contains(QIMSYS_CONFIG, tests) {
    INCLUDEPATH += $$PWD/test
    DEPENDPATH += $$PWD/test
}

include(../../../qimsys.pri)

LIBS *= -L$$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_LIBRARY_PATH

LIBS *= -l$$qtLibraryName(qimsys_qt4)

!contains(QIMSYS_CONFIG, no-dbus): QT += dbus
