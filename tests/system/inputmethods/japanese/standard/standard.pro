TARGET = tst_japanese_standard
include(../../../../../qimsystest.pri)

QT += gui-private widgets

SOURCES += main.cpp
RESOURCES += main.qrc

OTHER_FILES += \
    data/functionality.txt
