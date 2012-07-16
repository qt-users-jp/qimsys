include(../../../../qimsysapp.pri)

LIBS -= -L$$QIMSYS_BUILD_TREE/$$QIMSYS_TARGET_PATH/$$QIMSYS_LIBRARY_PATH
LIBS -= -l$$qtLibraryName(qimsys_qt)

SOURCES += main.cpp dialog.cpp
HEADERS += dialog.h
FORMS += dialog.ui
