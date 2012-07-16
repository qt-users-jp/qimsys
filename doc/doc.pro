TEMPLATE = app
OTHER_FILES += Doxyfile
TARGET = html/index.html
CONFIG -= qt
QT =
LIBS =
OBJECTS_DIR =
QMAKE_LINK = @: IGNORE THIS LINE

include(./src/src.pri)
include(../qimsys.pri)

doxygen.commands = (cat \
$$PWD/Doxyfile; \
echo "OUTPUT_DIRECTORY = $$QIMSYS_BUILD_TREE/doc"; \
echo "INPUT = $$QIMSYS_SOURCE_TREE/src $$QIMSYS_SOURCE_TREE/doc/src"; \
echo "EXAMPLE_PATH = $$QIMSYS_SOURCE_TREE/examples"; \
echo "IMAGE_PATH = $$QIMSYS_SOURCE_TREE/doc/images"; \
echo "STRIP_FROM_PATH = $$QIMSYS_BUILD_TREE"; \
echo "EXTRACT_ALL = NO"; \
) | doxygen -
QMAKE_EXTRA_TARGETS += doxygen
PRE_TARGETDEPS += doxygen
