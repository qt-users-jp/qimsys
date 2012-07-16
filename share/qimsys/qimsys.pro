TEMPLATE = subdirs
include(../../qimsys.pri)
!contains(QIMSYS_CONFIG, no-translation) {
    SUBDIRS = translations
}
