isEmpty(QIMSYSPLUGIN_PRI) {
QIMSYSPLUGIN_PRI = 1

include(./qimsys.pri)
include(./src/libs/libs.pri)

contains(QT_CONFIG, reduce_exports) {
    contains(CONFIG, qt): CONFIG += hide_symbols
}

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget($$TARGET)

DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_PLUGIN_PATH/

isEmpty(target.path) {
    target.path = $$PREFIX/$$QIMSYS_TARGET_PATH/$$QIMSYS_PLUGIN_PATH/
}
INSTALLS += target

QMAKE_RPATHDIR += \$\$ORIGIN/../../../$$QIMSYS_LIBRARY_PATH
include(./qimsysrpath.pri)

} # QIMSYSPLUGIN_PRI
