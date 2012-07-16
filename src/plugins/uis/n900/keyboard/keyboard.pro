TARGET=n900_keyboard
include(../../../../../qimsysplugin.pri)

QT += opengl declarative dbus

# Input
HEADERS += plugin.h object.h keyboard.h \
    keyboardcontrol.h
SOURCES += plugin.cpp object.cpp keyboard.cpp \
    keyboardcontrol.cpp

COPY_TARGET = qml/N900/Keyboard
COPY_FILES += \
    $$PWD/main.qml $$PWD/KeyboardLayout.qml $$PWD/Button.qml
include(../../../../../qimsysdata.pri)

OTHER_FILES += $$COPY_FILES
