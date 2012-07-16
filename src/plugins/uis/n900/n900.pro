TEMPLATE = subdirs

SUBDIRS *= about imoff keyboard

!contains(QIMSYS_CONFIG, no-system-n900) {
    SUBDIRS *= dialog statusbar userdictionary
}
