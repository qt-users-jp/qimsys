include(../../qimsysplatform.pri)
TEMPLATE = subdirs
SUBDIRS = qimsys

qimsys_platform_maemo {
    SUBDIRS *= n900
}
