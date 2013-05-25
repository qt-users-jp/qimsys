include(../../../../../qimsysplugin.pri)

QT += network

HEADERS += engine.h \
    plugin.h

SOURCES += engine.cpp \
    plugin.cpp

!contains(QIMSYS_CONFIG, no-gui) {
    HEADERS += settings.h
    SOURCES += settings.cpp
    RESOURCES += socialime.qrc
    FORMS += settings.ui
}
