include(../../../../qimsysplugin.pri)

QT += widgets x11extras

DEPENDPATH += .
INCLUDEPATH += . \
    ./IMdkit
LIBS += -L./IMdkit/ \
    -lXimd

# Input
HEADERS += plugin.h object.h client.h \
    inputcontext.h \
    preeditwidget.h \
    qtx11.h \
    qximinputmethod.h \
    qabstractinputmethod.h
SOURCES += plugin.cpp object.cpp client.cpp \
    inputcontext.cpp \
    preeditwidget.cpp \
    qximinputmethod.cpp \
    qabstractinputmethod.cpp
RESOURCES += xim.qrc
OTHER_FILES += keysym2qtkey.tbl
