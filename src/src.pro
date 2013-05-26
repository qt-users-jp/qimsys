include(../qimsys.pri)
TEMPLATE = subdirs
CONFIG += ordered
isEqual(QT_MAJOR_VERSION, 4) {
    SUBDIRS = libs apps plugins

    versionCheck(4,7,0) {
        SUBDIRS += imports
    }
}

isEqual(QT_MAJOR_VERSION, 5) {
    SUBDIRS = libs plugins
}
