TEMPLATE = lib
TARGET = qimsys_qt4

include(../../../qimsyslibrary.pri)

DEFINES += QIMSYS_LIBRARY

LIB_HEADERS = $$PWD/qimsysglobal.h
RESOURCES += qt4.qrc

INCLUDEPATH += $$PWD
HEADERS *= $$LIB_HEADERS

contains(QIMSYS_CONFIG, sdk) {
    lib_headers.files = $$LIB_HEADERS
    lib_headers.path = $$PREFIX/include/qimsys/qt4
    INSTALLS += lib_headers
}

include(debug/debug.pri)
include(ipc/ipc.pri)
include(plugins/plugins.pri)
include(items/items.pri)
include(utils/utils.pri)
include(server/server.pri)
!contains(QIMSYS_CONFIG, no-gui) {
    include(widgets/widgets.pri)
}
contains(QIMSYS_CONFIG, tests) {
    include(test/test.pri)
}
