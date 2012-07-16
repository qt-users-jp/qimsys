include(../../../qimsys.pri)

TEMPLATE = subdirs
versionCheck(4,7,0) {
    !contains(QIMSYS_CONFIG, no-gui): SUBDIRS = statusviewer
}
