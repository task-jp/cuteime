isEmpty(CUTEIME_PRI) {
CUTEIME_PRI = 1

include(./cuteimeplatform.pri)

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

CUTEIME_VERSION = 0.2.0
#VERSION = $$CUTEIME_VERSION

cuteime_platform_mac: CUTEIME_CONFIG *= no-dbus
cuteime_platform_win32: CUTEIME_CONFIG *= no-dbus

contains(CUTEIME_CONFIG, no-gui): DEFINES += CUTEIME_NO_GUI
contains(CUTEIME_CONFIG, no-dbus): DEFINES += CUTEIME_NO_DBUS

CUTEIME_SOURCE_TREE = $$PWD
INCLUDEPATH += $$CUTEIME_SOURCE_TREE/src
isEmpty(CUTEIME_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    CUTEIME_BUILD_TREE = $$cleanPath($$OUT_PWD)
    CUTEIME_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}

cuteime_platform_mac {
    CUTEIME_APP_TARGET = Cuteime
    CUTEIME_TARGET_PATH = $${CUTEIME_APP_TARGET}.app/Contents
    CUTEIME_APP_PATH = MacOS
    CUTEIME_LIBRARY_PATH = PlugIns
    CUTEIME_PLUGIN_PATH = $$CUTEIME_LIBRARY_PATH/$$CUTEIME_APP_TARGET
    CUTEIME_IMPORTS_PATH = $$CUTEIME_LIBRARY_PATH/Imports
    CUTEIME_DATA_PATH = Resources
    CUTEIME_TRANSLATIONS_PATH = $$CUTEIME_DATA_PATH/translations
    QMAKE_CXXFLAGS *= -mmacosx-version-min=10.5
    QMAKE_LFLAGS *= -mmacosx-version-min=10.5
    copydata = 1
}

cuteime_platform_linux {
    CUTEIME_APP_TARGET = cuteime
    CUTEIME_TARGET_PATH =
    CUTEIME_APP_PATH = bin
    CUTEIME_LIBRARY_PATH = lib64
    CUTEIME_PLUGIN_PATH = $$CUTEIME_LIBRARY_PATH/$$CUTEIME_APP_TARGET/plugins
    CUTEIME_IMPORTS_PATH = $$CUTEIME_LIBRARY_PATH/$$CUTEIME_APP_TARGET/imports
    CUTEIME_DATA_PATH = share/$$CUTEIME_APP_TARGET
    CUTEIME_TRANSLATIONS_PATH = $$CUTEIME_DATA_PATH/translations
    !isEqual(CUTEIME_SOURCE_TREE, $$CUTEIME_BUILD_TREE):copydata = 1
    isEmpty(PREFIX): PREFIX = /usr/local
}

cuteime_platform_windows {
    CUTEIME_APP_TARGET = cuteime
    CUTEIME_TARGET_PATH =
    CUTEIME_APP_PATH = bin
    CUTEIME_LIBRARY_PATH = bin
    CUTEIME_PLUGIN_PATH = lib/$$CUTEIME_APP_TARGET/plugins
    CUTEIME_IMPORTS_PATH = lib/$$CUTEIME_APP_TARGET/imports
    CUTEIME_DATA_PATH = share/$$CUTEIME_APP_TARGET
    CUTEIME_TRANSLATIONS_PATH = $$CUTEIME_DATA_PATH/translations
    !isEqual(CUTEIME_SOURCE_TREE, $$CUTEIME_BUILD_TREE):copydata = 1
}

DEFINES += CUTEIME_VERSION=\\\"$$CUTEIME_VERSION\\\"
DEFINES += CUTEIME_APP_PATH=\\\"$$CUTEIME_APP_PATH\\\"
DEFINES += CUTEIME_LIBRARY_PATH=\\\"$$CUTEIME_LIBRARY_PATH\\\"
DEFINES += CUTEIME_PLUGIN_PATH=\\\"$$CUTEIME_PLUGIN_PATH\\\"
DEFINES += CUTEIME_IMPORTS_PATH=\\\"$$CUTEIME_IMPORTS_PATH\\\"
DEFINES += CUTEIME_DATA_PATH=\\\"$$CUTEIME_DATA_PATH\\\"
DEFINES += CUTEIME_TRANSLATIONS_PATH=\\\"$$CUTEIME_TRANSLATIONS_PATH\\\"

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

} # CUTEIME_PRI
