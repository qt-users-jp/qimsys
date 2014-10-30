include(../qimsys.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = libs plugins
isEqual(QT_MAJOR_VERSION, 5) {
    SUBDIRS += apps
}
