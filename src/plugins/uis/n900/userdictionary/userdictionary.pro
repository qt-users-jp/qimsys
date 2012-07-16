TARGET=n900_userdictionary
include(../../../../../qimsysplugin.pri)

QT += declarative

HEADERS += widget.h \
    plugin.h \
    object.h
SOURCES += widget.cpp \
    plugin.cpp \
    object.cpp
FORMS +=

COPY_TARGET = qml/N900/UserDictionary
COPY_FILES += \
    $$PWD/userdictionary.qml $$PWD/Delegate.qml
include(../../../../../qimsysdata.pri)

OTHER_FILES += $$COPY_FILES
