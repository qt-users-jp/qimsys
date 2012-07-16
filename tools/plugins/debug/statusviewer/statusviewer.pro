include(../../../../qimsysplugin.pri)

QT += declarative

HEADERS += plugin.h \
    object.h
SOURCES += plugin.cpp \
    object.cpp

COPY_TARGET = qml/Debug/StatusViewer
COPY_FILES += \
    $$PWD/main.qml
include(../../../../qimsysdata.pri)

OTHER_FILES += $$COPY_FILES
