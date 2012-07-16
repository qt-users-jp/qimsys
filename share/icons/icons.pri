include(../../qimsys.pri)

qimsys_platform_x11 | qimsys_platform_maemo {
    icons.files = $$PWD/qimsys.png \
                  $$PWD/qimsys_blue.png \
                  $$PWD/qimsys_cyan.png \
                  $$PWD/qimsys_gray.png \
                  $$PWD/qimsys_green.png \
                  $$PWD/qimsys_magenta.png \
                  $$PWD/qimsys_red.png \
                  $$PWD/qimsys_yellow.png
    icons.path = $$PREFIX/share/pixmaps/qimsys
    INSTALLS += icons
}
