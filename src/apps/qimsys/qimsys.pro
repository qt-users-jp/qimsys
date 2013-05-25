TARGET = $$QIMSYS_APP_TARGET
include(../../../qimsysapp.pri)
include(../../../share/share.pri)

contains(QIMSYS_CONFIG, no-gui): QT -= gui

SOURCES += main.cpp \
    application.cpp
HEADERS += application.h
