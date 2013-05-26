TEMPLATE = lib
TARGET = qimsys_qt5

include(../../../qimsyslibrary.pri)
qimsys_platform_x11: QT += x11extras

DEFINES += QIMSYS_LIBRARY

LIB_HEADERS = ../qt4/qimsysglobal.h

INCLUDEPATH += ../qt4
HEADERS *= $$LIB_HEADERS

contains(QIMSYS_CONFIG, sdk) {
    lib_headers.files = $$LIB_HEADERS
    lib_headers.path = $$PREFIX/include/qimsys/qt5
    INSTALLS += lib_headers
}

include(../qt4/debug/debug.pri)
include(ipc/ipc.pri)
include(items/items.pri)
