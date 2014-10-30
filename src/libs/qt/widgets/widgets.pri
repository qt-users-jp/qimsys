include(../../../../qimsys.pri)

SOURCES += $$PWD/qimsyskeywidget.cpp $$PWD/qimsyswidget.cpp
HEADERS += $$PWD/qimsyskeywidget.h $$PWD/qimsyswidget.h

versionCheck(4,7,0) {
    QT += declarative
    HEADERS += $$PWD/qimsysdeclarativeview.h
    SOURCES += $$PWD/qimsysdeclarativeview.cpp
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

OTHER_FILES += \
    $$PWD/qimsyskeywidget.dox \
