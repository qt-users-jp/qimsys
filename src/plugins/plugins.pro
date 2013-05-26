TEMPLATE = subdirs

isEqual(QT_MAJOR_VERSION, 4) {
    SUBDIRS = system languages inputmethods engines

    !contains(QIMSYS_CONFIG, no-keyboard) {
        SUBDIRS += converters interpreters
    }

    !contains(QIMSYS_CONFIG, no-gui) {
        SUBDIRS += clients uis
    }
}

isEqual(QT_MAJOR_VERSION, 5) {
    SUBDIRS += clients
}
