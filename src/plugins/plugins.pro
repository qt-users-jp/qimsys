TEMPLATE = subdirs

SUBDIRS = system languages inputmethods engines

!contains(QIMSYS_CONFIG, no-keyboard) {
    SUBDIRS += converters interpreters
}

!contains(QIMSYS_CONFIG, no-gui) {
    SUBDIRS += clients uis
}

