TEMPLATE = lib
TARGET = qimsys_gtk

CONFIG += gtk
CONFIG -= qt
QT -= core gui
# CONFIG += warn_off

include(../../../qimsyslibrary.pri)

DEFINES += QIMSYS_LIBRARY

LIB_HEADERS = $$PWD/qimsysglobal.h

HEADERS *= $$LIB_HEADERS

contains(QIMSYS_CONFIG, sdk) {
    lib_headers.files = $$LIB_HEADERS
    lib_headers.path = $$PREFIX/include/qimsys/gtk
    INSTALLS += lib_headers
}

include(debug/debug.pri)
include(ipc/ipc.pri)
