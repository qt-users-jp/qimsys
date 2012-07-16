TEMPLATE = subdirs

SUBDIRS = full half

contains(QIMSYS_CONFIG, tests) {
    SUBDIRS += full/test
}
