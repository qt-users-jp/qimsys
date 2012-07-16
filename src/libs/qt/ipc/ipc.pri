IPC_SOURCES = $$PWD/dbus.cpp \
    $$PWD/qimsysabstractipcobject.cpp \
    $$PWD/qimsysapplicationmanager.cpp \
    $$PWD/qimsyspreeditmanager.cpp \
    $$PWD/qimsyscandidatemanager.cpp \
    $$PWD/qimsyskeymanager.cpp \
    $$PWD/qimsysinputmethodmanager.cpp \
    $$PWD/qimsyskeyboardmanager.cpp

IPC_HEADERS = $$PWD/dbus.h \
    $$PWD/qimsysabstractipcobject.h \
    $$PWD/qimsysapplicationmanager.h \
    $$PWD/qimsyspreeditmanager.h \
    $$PWD/qimsyscandidatemanager.h \
    $$PWD/qimsyskeymanager.h \
    $$PWD/qimsysinputmethodmanager.h \
    $$PWD/qimsyskeyboardmanager.h

!contains(QIMSYS_CONFIG, no-gui) {
    qimsys_platform_x11 | qimsys_platform_maemo {
        IPC_SOURCES += $$PWD/qimsyskeymanager_x11.cpp
    }
    qimsys_platform_qws {
        IPC_SOURCES += $$PWD/qimsyskeymanager_qws.cpp
    }
    qimsys_platform_mac {
        IPC_SOURCES += $$PWD/qimsyskeymanager_mac.cpp
    }
    qimsys_platform_win32 {
        IPC_SOURCES += $$PWD/qimsyskeymanager_win.cpp
    }
    qimsys_platform_symbian {
        IPC_SOURCES += $$PWD/qimsyskeymanager_symbian.cpp
    }
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!contains(QIMSYS_CONFIG, no-dbus): QT += dbus

HEADERS *= $$IPC_HEADERS
SOURCES *= $$IPC_SOURCES

contains(QIMSYS_CONFIG, sdk) {
    ipc_headers.files = $$IPC_HEADERS
    ipc_headers.path = $$PREFIX/include/qimsys/qt
    INSTALLS += ipc_headers
}

OTHER_FILES += \
    $$PWD/qimsysabstractipcobject.dox \
    $$PWD/qimsysapplicationmanager.dox \
    $$PWD/qimsyspreeditmanager.dox \
    $$PWD/qimsyscandidatemanager.dox \
    $$PWD/qimsyskeymanager.dox
