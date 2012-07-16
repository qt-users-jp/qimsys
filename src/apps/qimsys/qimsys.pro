TARGET = $$QIMSYS_APP_TARGET
include(../../../qimsysapp.pri)
include(../../../share/share.pri)

contains(QIMSYS_CONFIG, no-gui): QT -= gui

SOURCES += main.cpp \
    application.cpp
HEADERS += application.h

qimsys_platform_symbian {
    TARGET.UID3 = 0xE519FABC
    TARGET.CAPABILITY = "ReadUserData WriteUserData"
}
