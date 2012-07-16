isEmpty(QIMSYSAPP_PRI) {
QIMSYSAPP_PRI = 1

include(./qimsys.pri)
TEMPLATE = app
macx {
    DESTDIR = $$QIMSYS_BUILD_TREE
} else {
    DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_APP_PATH
}

QMAKE_RPATHDIR += \$\$ORIGIN/../$$QIMSYS_LIBRARY_PATH
include(./qimsysrpath.pri)
include(./src/libs/libs.pri)

qimsys_platform_linux {
    target.path = $$PREFIX/$$QIMSYS_APP_PATH
    INSTALLS += target
}

qimsys_platform_symbian {
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

} # QIMSYSAPP_PRI
