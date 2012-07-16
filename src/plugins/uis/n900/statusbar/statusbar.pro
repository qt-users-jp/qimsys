TARGET = -qimsys-n900-statusbar
CONFIG += gtk
CONFIG -= qt
QT -= core gui

include(../../../../../qimsysplugin.pri)
target.path = $$PREFIX/lib/hildon-desktop/
DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_LIBRARY_PATH/hildon-desktop

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0 gdk-pixbuf-2.0 hildon-1 libhildondesktop-1

HEADERS = statusbar.h
SOURCES = statusbar.c

qimsys_platform_maemo {
    desktop.files = $$PWD/qimsys-n900-statusbar.desktop
    desktop.path = $$PREFIX/share/applications/hildon-status-menu
    INSTALLS += desktop

    icons.files = $$PWD/qimsys_keyboard_on.png $$PWD/qimsys_keyboard_off.png
    icons.path = $$PREFIX/share/pixmaps/qimsys/
    INSTALLS += icons
}

