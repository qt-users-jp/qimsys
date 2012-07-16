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

qimsys_platform_symbian {
    TARGET.CAPABILITY = "LocalServices NetworkServices UserEnvironment ReadUserData WriteUserData"
}
