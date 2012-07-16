include(../../../../qimsysplugin.pri)

HEADERS += object.h \
    plugin.h
SOURCES += object.cpp \
    plugin.cpp

OTHER_FILES += \
    inputmethodmanager.dox

qimsys_platform_symbian {
    TARGET.CAPABILITY = "ReadUserData WriteUserData"
}
