TARGET  = n900
TARGETPATH = Qimsys/N900
include(../../../qimsysimports.pri)

QT += dbus

SOURCES += \
    plugin.cpp \
    deviceinfo.cpp

HEADERS += \
    plugin.h \
    deviceinfo.h

OTHER_FILES = qmldir

