TEMPLATE = lib
TARGET = qimsys_qt$$QT_MAJOR_VERSION

include(../../../qimsyslibrary.pri)

isEqual(QT_MAJOR_VERSION, 5): qimsys_platform_x11: QT += x11extras

DEFINES += QIMSYS_LIBRARY
RESOURCES += qt.qrc

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

LIB_HEADERS *= $$PWD/qimsysglobal.h

include(ipc/ipc.pri)
include(items/items.pri)
include(debug/debug.pri)
isEqual(QT_MAJOR_VERSION, 5) {
    include(plugins/plugins.pri)
    include(utils/utils.pri)
    include(server/server.pri)
    include(widgets/widgets.pri)
}
contains(QIMSYS_CONFIG, tests) {
    include(test/test.pri)
}

HEADERS *= $$LIB_HEADERS

contains(QIMSYS_CONFIG, sdk) {
    lib_headers.files = $$LIB_HEADERS
    lib_headers.path = $$PREFIX/include/qimsys/qt$$QT_MAJOR_VERSION
    INSTALLS += lib_headers
}

