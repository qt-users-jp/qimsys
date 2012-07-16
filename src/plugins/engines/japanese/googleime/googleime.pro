include(../../../../../qimsysplugin.pri)

QT += network

HEADERS += engine.h \
    plugin.h
SOURCES += engine.cpp \
    plugin.cpp
RESOURCES += googleime.qrc

qimsys_platform_symbian {
    TARGET.CAPABILITY = "LocalServices NetworkServices UserEnvironment ReadUserData WriteUserData"
}
