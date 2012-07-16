include(../../../qimsysplatform.pri)
TEMPLATE = subdirs

qimsys_platform_linux {
    SUBDIRS += autostart
}

SUBDIRS += inputmethodmanager language
