include(../../../../qimsysplugin.pri)

CONFIG += link_pkgconfig
PKGCONFIG += protobuf

MOZC_ROOT = /home/tasuku/tools/inputmethods/mozc/src

INCLUDEPATH += $$MOZC_ROOT $$MOZC_ROOT/out/Release/obj/gen/proto_out/
LIBS += -L$$MOZC_ROOT/out/Release/obj.target/session -lsession_protocol
LIBS += -L$$MOZC_ROOT/out/Release/obj.target/client -lclient
LIBS += -L$$MOZC_ROOT/out/Release/obj.target/ipc -lipc
LIBS += -L$$MOZC_ROOT/out/Release/obj.target/base -lbase

HEADERS += mozcplugin.h mozcobject.h
SOURCES += mozcplugin.cpp mozcobject.cpp

RESOURCES += \
    mozc.qrc
