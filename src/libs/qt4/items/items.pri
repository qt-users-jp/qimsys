ITEMS_HEADERS = $$PWD/qimsysconversionitem.h $$PWD/qimsyspreedititem.h
ITEMS_SOURCES = $$PWD/qimsysconversionitem.cpp $$PWD/qimsyspreedititem.cpp
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS *= $$ITEMS_HEADERS
SOURCES *= $$ITEMS_SOURCES

contains(QIMSYS_CONFIG, sdk) {
    item_headers.files = $$ITEMS_HEADERS
    item_headers.path = $$PREFIX/include/qimsys/qt4
	INSTALLS += item_headers
}

OTHER_FILES += \
    $$PWD/qimsysconversionitem.dox \
    $$PWD/qimsyspreedititem.dox
