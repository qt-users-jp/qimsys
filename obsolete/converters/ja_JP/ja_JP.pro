include(../../../../qimsysplugin.pri)

HEADERS += japanese.h \
    preedit.h \
	japanesesettings.h \
    keyactionmanager.h \
    keyaction.h \
    converter.h \
    keyactionedit.h \
    interpreter.h \
    inputmanager.h \
    namespace.h
SOURCES += japanese.cpp \
    preedit.cpp \
	japanesesettings.cpp \
    keyactionmanager.cpp \
    keyaction.cpp \
    converter.cpp \
    keyactionedit.cpp \
    interpreter.cpp \
    inputmanager.cpp
RESOURCES += ja_JP.qrc
FORMS += japanesesettings.ui \
    keyactionedit.ui

OTHER_FILES += \
    typing/kyuriex.txt
