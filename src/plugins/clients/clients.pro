include(../../../qimsysplatform.pri)
TEMPLATE = subdirs

qimsys_platform_linux {
    !contains(QIMSYS_CONFIG, no-dbus) {
        !contains(QIMSYS_CONFIG, no-qt4immodule) SUBDIRS += qt4immodule

        !contains(QIMSYS_CONFIG, no-gtk) {
            !contains(QIMSYS_CONFIG, no-gtk2immodule) SUBDIRS += gtk2immodule
        }
    }

    !contains(QIMSYS_CONFIG, no-xim) {
        CONFIG += ordered
        SUBDIRS += xim/IMdkit xim
    }
}
