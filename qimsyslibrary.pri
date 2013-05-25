isEmpty(QIMSYSLIBRARY_PRI) {
QIMSYSLIBRARY_PRI = 1

include(./qimsys.pri)

win32 {
    DLLDESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_LIBRARY_PATH
}

TEMPLATE = lib

contains(QT_CONFIG, reduce_exports) {
    contains(CONFIG, qt): CONFIG += hide_symbols
}

TARGET = $$qtLibraryTarget($$TARGET)

DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_LIBRARY_PATH

QMAKE_RPATHDIR += \$\$ORIGIN/../$$QIMSYS_LIBRARY_PATH
include(./qimsysrpath.pri)

qimsys_platform_linux {
    target.path = $$PREFIX/$$QIMSYS_TARGET_PATH/$$QIMSYS_LIBRARY_PATH
    INSTALLS += target
}

} # QIMSYSLIBRARY_PRI
