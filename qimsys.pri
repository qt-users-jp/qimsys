isEmpty(QIMSYS_PRI) {
QIMSYS_PRI = 1

include(./qimsysplatform.pri)

defineReplace(qtLibraryName) {
    unset(LIBRARY_NAME)
    LIBRARY_NAME = $$1
    CONFIG(debug, debug|release) {
        !debug_and_release|build_pass {
            mac:RET = $$member(LIBRARY_NAME, 0)_debug
            else:win32:RET = $$member(LIBRARY_NAME, 0)d
        }
    }
    isEmpty(RET):RET = $$LIBRARY_NAME
    return($$RET)
}

defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

QIMSYS_VERSION = 0.2.0
#VERSION = $$QIMSYS_VERSION

qimsys_platform_mac: QIMSYS_CONFIG *= no-dbus
qimsys_platform_win32: QIMSYS_CONFIG *= no-dbus

contains(QIMSYS_CONFIG, no-gui): DEFINES += QIMSYS_NO_GUI
contains(QIMSYS_CONFIG, no-dbus): DEFINES += QIMSYS_NO_DBUS

QIMSYS_SOURCE_TREE = $$PWD
INCLUDEPATH += $$QIMSYS_SOURCE_TREE/src
isEmpty(QIMSYS_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    QIMSYS_BUILD_TREE = $$cleanPath($$OUT_PWD)
    QIMSYS_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}

qimsys_platform_mac {
    QIMSYS_APP_TARGET = Qimsys
    QIMSYS_TARGET_PATH = $${QIMSYS_APP_TARGET}.app/Contents
    QIMSYS_APP_PATH = MacOS
    QIMSYS_LIBRARY_PATH = PlugIns
    QIMSYS_PLUGIN_PATH = $$QIMSYS_LIBRARY_PATH/$$QIMSYS_APP_TARGET
    QIMSYS_IMPORTS_PATH = $$QIMSYS_LIBRARY_PATH/Imports
    QIMSYS_DATA_PATH = Resources
    QIMSYS_TRANSLATIONS_PATH = $$QIMSYS_DATA_PATH/translations
    QMAKE_CXXFLAGS *= -mmacosx-version-min=10.5
    QMAKE_LFLAGS *= -mmacosx-version-min=10.5
    copydata = 1
}

qimsys_platform_linux {
    QIMSYS_APP_TARGET = qimsys
    QIMSYS_TARGET_PATH =
    QIMSYS_APP_PATH = bin
    QIMSYS_LIBRARY_PATH = lib
    QIMSYS_PLUGIN_PATH = $$QIMSYS_LIBRARY_PATH/$$QIMSYS_APP_TARGET/plugins
    QIMSYS_IMPORTS_PATH = $$QIMSYS_LIBRARY_PATH/$$QIMSYS_APP_TARGET/imports
    QIMSYS_DATA_PATH = share/$$QIMSYS_APP_TARGET
    QIMSYS_TRANSLATIONS_PATH = $$QIMSYS_DATA_PATH/translations
    !isEqual(QIMSYS_SOURCE_TREE, $$QIMSYS_BUILD_TREE):copydata = 1
    isEmpty(PREFIX):PREFIX = /usr/local
}

qimsys_platform_windows {
    QIMSYS_APP_TARGET = qimsys
    QIMSYS_TARGET_PATH =
    QIMSYS_APP_PATH = bin
    QIMSYS_LIBRARY_PATH = bin
    QIMSYS_PLUGIN_PATH = lib/$$QIMSYS_APP_TARGET/plugins
    QIMSYS_IMPORTS_PATH = lib/$$QIMSYS_APP_TARGET/imports
    QIMSYS_DATA_PATH = share/$$QIMSYS_APP_TARGET
    QIMSYS_TRANSLATIONS_PATH = $$QIMSYS_DATA_PATH/translations
    !isEqual(QIMSYS_SOURCE_TREE, $$QIMSYS_BUILD_TREE):copydata = 1
}

DEFINES += QIMSYS_VERSION=\\\"$$QIMSYS_VERSION\\\"
DEFINES += QIMSYS_APP_PATH=\\\"$$QIMSYS_APP_PATH\\\"
DEFINES += QIMSYS_LIBRARY_PATH=\\\"$$QIMSYS_LIBRARY_PATH\\\"
DEFINES += QIMSYS_PLUGIN_PATH=\\\"$$QIMSYS_PLUGIN_PATH\\\"
DEFINES += QIMSYS_IMPORTS_PATH=\\\"$$QIMSYS_IMPORTS_PATH\\\"
DEFINES += QIMSYS_DATA_PATH=\\\"$$QIMSYS_DATA_PATH\\\"
DEFINES += QIMSYS_TRANSLATIONS_PATH=\\\"$$QIMSYS_TRANSLATIONS_PATH\\\"

CONFIG(debug, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/debug-shared
CONFIG(release, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/release-shared

CONFIG(debug, debug|release):MOC_DIR = $${OUT_PWD}/.moc/debug-shared
CONFIG(release, debug|release):MOC_DIR = $${OUT_PWD}/.moc/release-shared

RCC_DIR = $${OUT_PWD}/.rcc
UI_DIR = $${OUT_PWD}/.uic

defineTest(versionCheck) {
    major = $$1
    minor = $$2
    patch = $$3

    isEqual(QT_MAJOR_VERSION, $$major) {
        isEqual(QT_MINOR_VERSION, $$minor) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$minor) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$major) {
        return(true)
    }
    return(false)
}

} # QIMSYS_PRI
