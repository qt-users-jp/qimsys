include(../../../qimsysplatform.pri)
TEMPLATE = subdirs

qimsys_platform_linux {
    isEqual(QT_MAJOR_VERSION, 5) {
        !contains(QIMSYS_CONFIG, no-dbus) {
            !contains(QIMSYS_CONFIG, no-qt5immodule) SUBDIRS += qt5immodule
            !contains(QIMSYS_CONFIG, no-gtk) SUBDIRS += gtkimmodule
        }

        !contains(QIMSYS_CONFIG, no-xim) {
            CONFIG += ordered
            SUBDIRS += xim/IMdkit xim
        }
    }

    isEqual(QT_MAJOR_VERSION, 4) {
        !contains(QIMSYS_CONFIG, no-qt4immodule) SUBDIRS += qt4immodule
    }
}
