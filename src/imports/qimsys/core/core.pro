TARGET  = qimsyscore
TARGETPATH = Qimsys/Core
include(../../../../qimsysimports.pri)

# Input
SOURCES += \
    plugin.cpp \
    applicationmanager.cpp \
    inputmethodmanager.cpp \
    preeditmanager.cpp \
    candidatemanager.cpp \
    preedititem.cpp \
    conversionitem.cpp \
    keymanager.cpp \
    keyboardmanager.cpp

HEADERS += \
    plugin.h \
    applicationmanager.h \
    inputmethodmanager.h \
    preeditmanager.h \
    candidatemanager.h \
    preedititem.h \
    conversionitem.h \
    keymanager.h \
    keyboardmanager.h

OTHER_FILES = qmldir

