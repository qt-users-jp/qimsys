include(../../../qimsys.pri)
TARGET = $$QIMSYS_APP_TARGET
include(../../../qimsysapp.pri)
include(../../../share/share.pri)

contains(QIMSYS_CONFIG, no-gui) {
    QT -= gui
} else {
    QT += widgets
}

SOURCES += main.cpp \
    application.cpp
HEADERS += application.h
