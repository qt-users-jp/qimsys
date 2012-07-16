TEMPLATE = lib
TARGET = im-qimsys-gtk

isEmpty(GTK_IM_MODULE_DIR) {
    target.path = $$system(pkg-config --variable=libdir gtk+-2.0)/gtk-2.0/$$system(pkg-config --variable=gtk_binary_version gtk+-2.0)/immodules/
} else {
    target.path = $$GTK_IM_MODULE_DIR
}

CONFIG += gtk
CONFIG -= qt
QT -= core gui

include(../../../../qimsysplugin.pri)

CONFIG += link_pkgconfig
PKGCONFIG += gtk+-2.0

HEADERS = \
    qimsysimcontext.h \
    gtk2qt.h
SOURCES = \
    im-qimsys.c \
    qimsysimcontext.c

DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_LIBRARY_PATH/plugins/inputmethods

update_local_gtk_immodules.path = $$DESTDIR
update_local_gtk_immodules.depends = all
update_local_gtk_immodules.files = gtk.immodules
update_local_gtk_immodules.commands = (cd $$DESTDIR && LD_LIBRARY_PATH=../../ gtk-query-immodules-2.0 libim-qimsys.so > gtk.immodules)

QMAKE_EXTRA_TARGETS += update_local_gtk_immodules

update_gtk_immodules.path = /etc/gtk-2.0
update_gtk_immodules.depends = install_target
update_gtk_immodules.files = gtk.immodules
update_gtk_immodules.commands = (test -w /etc/gtk-2.0/gtk.immodules && (gtk-query-immodules-2.0 > /etc/gtk-2.0/gtk.immodules)) || echo \"exec gtk-query-immodules-2.0 > /etc/gtk-2.0/gtk.immodules\"

INSTALLS += update_gtk_immodules

OTHER_FILES += \
    gtk2qtkey.tbl
