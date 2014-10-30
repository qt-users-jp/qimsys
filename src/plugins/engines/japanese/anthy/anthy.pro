include(../../../../../qimsysplugin.pri)

HEADERS += plugin.h engine.h \
    libanthy.h \
    libanthydic.h \
    dictionarymodel.h \
    dictionary.h

SOURCES += plugin.cpp engine.cpp \
    libanthy.cpp \
    libanthydic.cpp \
    dictionarymodel.cpp \
    dictionary.cpp

!contains(QIMSYS_CONFIG, no-gui) {
    QT += widgets
    HEADERS += wordeditor.h settings.h
    SOURCES += wordeditor.cpp settings.cpp

    RESOURCES += anthy.qrc
    FORMS += wordeditor.ui settings.ui

    COPY_TARGET = qml/Japanese/Anthy
    COPY_FILES += Editor.qml
    include(../../../../../qimsysdata.pri)

    OTHER_FILES += $$COPY_FILES
}
