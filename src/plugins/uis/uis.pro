include(../../../qimsysplatform.pri)
TEMPLATE = subdirs

SUBDIRS *= candidatewindow
qimsys_platform_desktop {
    SUBDIRS *= aboutqimsys settings userdictionary
    !contains(QIMSYS_CONFIG, no-systemtray) SUBDIRS += systemtray
    !contains(QIMSYS_CONFIG, no-toolbar) SUBDIRS += toolbar
}
