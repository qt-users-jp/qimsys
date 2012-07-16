include(../../../../qimsysplugin.pri)

HEADERS += plugin.h language.h
SOURCES += plugin.cpp language.cpp

qimsys_platform_symbian {
    TARGET.CAPABILITY = "ReadUserData WriteUserData"
}
