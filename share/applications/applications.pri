include(../../qimsys.pri)

qimsys_platform_x11 | qimsys_platform_maemo {
    desktop.files = $$PWD/qimsys.desktop
	desktop.path = $$PREFIX/share/applications
    INSTALLS += desktop
}

