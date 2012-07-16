include(../../../../qimsysplatform.pri)
TEMPLATE = subdirs

qimsys_platform_linux {
    !contains(QIMSYS_CONFIG, no-anthy) SUBDIRS += anthy
}

!contains(QIMSYS_CONFIG, no-socialime) SUBDIRS += socialime
!contains(QIMSYS_CONFIG, no-googleime) SUBDIRS += googleime
