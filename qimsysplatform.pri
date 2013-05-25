unix {
    macx {
        CONFIG += qimsys_platform_mac qimsys_platform_desktop
        DEFINES += QIMSYS_PLATFORM_MAC QIMSYS_PLATFORM_DESKTOP
    } else {
        CONFIG += qimsys_platform_linux
        DEFINES += QIMSYS_PLATFORM_LINUX
        contains(QT_CONFIG, embedded) {
            CONFIG += qimsys_platform_qws qimsys_platform_embedded
            DEFINES += QIMSYS_PLATFORM_QWS QIMSYS_PLATFORM_EMBEDDED
        } else {
            CONFIG += qimsys_platform_x11 qimsys_platform_desktop
            DEFINES += QIMSYS_PLATFORM_X11 QIMSYS_PLATFORM_DESKTOP
        }
    }
} else {
    windows {
        CONFIG += qimsys_platform_windows
        DEFINES += QIMSYS_PLATFORM_WINDOWS
        contains(QT_CONFIG, embedded) {
            CONFIG += qimsys_platform_wince qimsys_platform_embedded
            DEFINES += QIMSYS_PLATFORM_WINCE QIMSYS_PLATFORM_EMBEDDED
        } else {
            CONFIG += qimsys_platform_win32 qimsys_platform_desktop
            DEFINES += QIMSYS_PLATFORM_WIN32 QIMSYS_PLATFORM_DESKTOP
        }
    }
}
