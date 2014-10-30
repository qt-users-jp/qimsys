include(../../qimsysplatform.pri)
TEMPLATE = subdirs

SUBDIRS = qt

isEqual(QT_MAJOR_VERSION, 5): qimsys_platform_linux {
    !contains(QIMSYS_CONFIG, no-dbus) {
        !contains(QIMSYS_CONFIG, no-gtk): SUBDIRS += gtk
    }
}
