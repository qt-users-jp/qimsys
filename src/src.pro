include(../qimsys.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = libs apps plugins

versionCheck(4,7,0) {
    SUBDIRS += imports
}
