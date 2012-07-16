include(../../qimsysplatform.pri)
TEMPLATE = subdirs
SUBDIRS = qt

qimsys_platform_linux {
    !contains(QIMSYS_CONFIG, no-dbus) {
        !contains(QIMSYS_CONFIG, no-gtk): SUBDIRS += gtk
    }
}
