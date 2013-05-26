include(../../qimsysplatform.pri)
TEMPLATE = subdirs

isEqual(QT_MAJOR_VERSION, 4): SUBDIRS = qt4
isEqual(QT_MAJOR_VERSION, 5): SUBDIRS = qt5

isEqual(QT_MAJOR_VERSION, 4): qimsys_platform_linux {
    !contains(QIMSYS_CONFIG, no-dbus) {
        !contains(QIMSYS_CONFIG, no-gtk): SUBDIRS += gtk
    }
}
