TARGET=n900_about
include(../../../../../qimsysplugin.pri)

QT += declarative

HEADERS += widget.h \
    plugin.h \
    object.h
SOURCES += widget.cpp \
    plugin.cpp \
    object.cpp
FORMS +=

COPY_TARGET = qml/N900/About
COPY_FILES += \
    $$PWD/about.qml
include(../../../../../qimsysdata.pri)

OTHER_FILES += $$COPY_FILES
