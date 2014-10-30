PLUGIN_SOURCES = $$PWD/qimsyspluginmanager.cpp $$PWD/qimsysplugin.cpp $$PWD/qimsysconverter.cpp $$PWD/qimsysengine.cpp $$PWD/qimsysenginedictionary.cpp $$PWD/qimsysabstractpluginobject.cpp $$PWD/qimsyslanguage.cpp $$PWD/qimsysinputmethod.cpp $$PWD/qimsysinterpreter.cpp
PLUGIN_HEADERS = $$PWD/qimsyspluginmanager.h $$PWD/qimsysplugin.h $$PWD/qimsysconverter.h $$PWD/qimsysengine.h $$PWD/qimsysenginedictionary.h $$PWD/qimsysabstractpluginobject.h $$PWD/qimsyslanguage.h $$PWD/qimsysinputmethod.h $$PWD/qimsysinterpreter.h

!contains(QIMSYS_CONFIG, no-gui) {
    QT += widgets
    PLUGIN_SOURCES += $$PWD/qimsyssettingswidget.cpp
    PLUGIN_HEADERS += $$PWD/qimsyssettingswidget.h
    FORMS += $$PWD/qimsyssettingswidget.ui
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS *= $$PLUGIN_HEADERS
SOURCES *= $$PLUGIN_SOURCES

contains(QIMSYS_CONFIG, sdk) {
    plugin_headers.files = $$PLUGIN_HEADERS
    plugin_headers.path = $$PREFIX/include/qimsys/qt
    INSTALLS += plugin_headers
}

OTHER_FILES += \
    $$PWD/qimsysabstractpluginobject.dox \
    $$PWD/qimsysconverter.dox \
    $$PWD/qimsysengine.dox \
    $$PWD/qimsysplugin.dox \
    $$PWD/qimsyspluginmanager.dox \
    $$PWD/qimsyssettingswidget.dox
