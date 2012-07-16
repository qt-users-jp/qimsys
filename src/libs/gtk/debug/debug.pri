DEBUG_HEADERS = $$PWD/qimsysdebug.h
DEBUG_SOURCES = $$PWD/qimsysdebug.c
INCLUDEPATH += $$PWD

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

HEADERS *= $$DEBUG_HEADERS
SOURCES *= $$DEBUG_SOURCES

contains(QIMSYS_CONFIG, sdk) {
	debug_headers.files = $$DEBUG_HEADERS
    debug_headers.path = $$PREFIX/include/qimsys/gtk
	INSTALLS += debug_headers
}

