IPC_HEADERS +=  $$PWD/qimsysmarshalers.h $$PWD/qimsysabstractipcobject.h $$PWD/qimsysapplicationmanager.h $$PWD/qimsysinputmethodmanager.h $$PWD/qimsyskeymanager.h $$PWD/qimsyspreeditmanager.h $$PWD/qimsyspreedititem.h $$PWD/qimsyskeyboardmanager.h
IPC_SOURCES += $$PWD/qimsysmarshalers.c $$PWD/qimsysabstractipcobject.c $$PWD/qimsysapplicationmanager.c $$PWD/qimsysinputmethodmanager.c $$PWD/qimsyskeymanager.c $$PWD/qimsyspreeditmanager.c $$PWD/qimsyspreedititem.c $$PWD/qimsyskeyboardmanager.c

CONFIG += link_pkgconfig
PKGCONFIG += dbus-glib-1 gdk-pixbuf-2.0

INCLUDEPATH += $$PWD

HEADERS *= $$IPC_HEADERS $$PWD/dbus.h
SOURCES *= $$IPC_SOURCES

contains(QIMSYS_CONFIG, sdk) {
    ipc_headers.files = $$IPC_HEADERS
    ipc_headers.path = $$PREFIX/include/qimsys/gtk
    INSTALLS += ipc_headers
}

OTHER_FILES += \
    ipc/qimsysmarshaler.list

qimsysmarshalers_list.target = $$PWD/qimsysmarshalers.list
QMAKE_EXTRA_TARGETS += qimsysmarshalers_list

qimsysmarshalers_h.target = $$PWD/qimsysmarshalers.h
qimsysmarshalers_h.path = $$PWD/qimsysmarshalers.h
qimsysmarshalers_h.depends = $$PWD/qimsysmarshalers.list
qimsysmarshalers_h.commands = \
    glib-genmarshal --prefix=qimsys_marshal $$PWD/qimsysmarshaler.list --header | grep -v -e \"^/\\*.*\\*/\" > qimsysmarshalers.h.tmp \
    && (cmp -s $$PWD/qimsysmarshalers.h qimsysmarshalers.h.tmp || cp qimsysmarshalers.h.tmp $$PWD/qimsysmarshalers.h);\
    rm qimsysmarshalers.h.tmp

QMAKE_EXTRA_TARGETS += qimsysmarshalers_h

qimsysmarshalers_c.target = $$PWD/qimsysmarshalers.c
qimsysmarshalers_c.path = $$PWD/qimsysmarshalers.c
qimsysmarshalers_c.depends = $$PWD/qimsysmarshalers.list $$PWD/qimsysmarshalers.h
qimsysmarshalers_c.commands = \
    glib-genmarshal --prefix=qimsys_marshal $$PWD/qimsysmarshaler.list --body | grep -v -e \"^/\\*.*\\*/\" > qimsysmarshalers.c.tmp \
    && (cmp -s $$PWD/qimsysmarshalers.c qimsysmarshalers.c.tmp || cp qimsysmarshalers.c.tmp $$PWD/qimsysmarshalers.c);\
    rm qimsysmarshalers.c.tmp

QMAKE_EXTRA_TARGETS += qimsysmarshalers_c

