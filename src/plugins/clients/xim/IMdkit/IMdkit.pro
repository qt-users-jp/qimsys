TEMPLATE = lib
QT -= core gui
TARGET = Ximd
CONFIG += static warn_off
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += FrameMgr.h IMdkit.h Xi18n.h Xi18nX.h XimFunc.h XimProto.h Xtrans.h
SOURCES += FrameMgr.c \
           i18nAttr.c \
           i18nClbk.c \
           i18nIc.c \
           i18nIMProto.c \
           i18nMethod.c \
           i18nPtHdr.c \
           i18nUtil.c \
           i18nX.c \
           IMConn.c \
           IMMethod.c \
           IMValues.c
