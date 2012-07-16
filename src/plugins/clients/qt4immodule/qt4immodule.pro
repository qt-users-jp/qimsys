TARGET = im-qimsys-qt

isEmpty(QT_IM_MODULE_DIR) {
    target.path = $$PREFIX/$$[QT_INSTALL_PLUGINS]/inputmethods
} else {
    target.path = $$QT_IM_MODULE_DIR
}

include(../../../../qimsysplugin.pri)

DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_LIBRARY_PATH/plugins/inputmethods
SOURCES += inputcontext.cpp \
    inputcontextplugin.cpp
HEADERS += inputcontext.h \
    inputcontextplugin.h
