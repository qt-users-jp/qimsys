TARGET = im-qimsys-qt4

isEmpty(QT_IM_MODULE_DIR) {
    target.path = $$[QT_INSTALL_PLUGINS]/inputmethods
} else {
    target.path = $$QT_IM_MODULE_DIR
}

include(../../../../qimsysplugin.pri)

DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_LIBRARY_PATH/plugins/inputmethods
SOURCES += inputcontext.cpp \
    inputcontextplugin.cpp
HEADERS += inputcontext.h \
    inputcontextplugin.h
