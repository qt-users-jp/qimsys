DEBUG_HEADERS = $$PWD/qimsysdebug.h
DEBUG_SOURCES = $$PWD/qimsysdebug.cpp
INCLUDEPATH += $$PWD

HEADERS *= $$DEBUG_HEADERS
SOURCES *= $$DEBUG_SOURCES

contains(QIMSYS_CONFIG, sdk) {
    debug_headers.files = $$DEBUG_HEADERS
    debug_headers.path = $$PREFIX/include/qimsys/qt
    INSTALLS += debug_headers
}

OTHER_FILES += \
    $$PWD/qimsysdebug.dox
