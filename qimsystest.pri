isEmpty(QIMSYSTEST_PRI) {
QIMSYSTEST_PRI = 1

include(./qimsys.pri)
contains(QIMSYS_CONFIG, no-gui): QT -= gui
TEMPLATE = app
QT += testlib
CONFIG += testcase
DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_APP_PATH
QMAKE_RPATHDIR += \$\$ORIGIN/../$$QIMSYS_LIBRARY_PATH
include(./qimsysrpath.pri)
include(./src/libs/libs.pri)

} # QIMSYSTEST_PRI
