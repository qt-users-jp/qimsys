TEMPLATE = subdirs
CONFIG += ordered

isEqual(QT_MAJOR_VERSION, 4) {
    SUBDIRS += src share
    contains(QIMSYS_CONFIG, sdk) : SUBDIRS += doc tools
    contains(QIMSYS_CONFIG, examples) : SUBDIRS += examples
    contains(QIMSYS_CONFIG, tests) {
        SUBDIRS += tests

        test.commands = "for test in `find bin -name \"tst_qimsys*\" -executable -type f`; do \$\$test; done"
        QMAKE_EXTRA_TARGETS += test
    }
}

isEqual(QT_MAJOR_VERSION, 5) {
    SUBDIRS += src
}

OTHER_FILES += configure.rb
