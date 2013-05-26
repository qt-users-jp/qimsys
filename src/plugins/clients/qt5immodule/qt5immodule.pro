TARGET = im-qimsys-qt5

isEmpty(QT_IM_MODULE_DIR) {
    target.path = $$PREFIX/$$[QT_INSTALL_PLUGINS]/platforminputcontexts
} else {
    target.path = $$QT_IM_MODULE_DIR
}

include(../../../../qimsysplugin.pri)

DESTDIR = $$QIMSYS_BUILD_TREE/$$QIMSYS_LIBRARY_PATH/plugins/inputmethods

QT += gui-private widgets quick

SOURCES += inputcontext.cpp \
    main.cpp
HEADERS += inputcontext.h
