unix {
    macx {
        CONFIG += cuteime_platform_mac cuteime_platform_desktop
        DEFINES += CUTEIME_PLATFORM_MAC CUTEIME_PLATFORM_DESKTOP
    } else {
        CONFIG += cuteime_platform_linux
        DEFINES += CUTEIME_PLATFORM_LINUX
        contains(QT_CONFIG, embedded) {
            CONFIG += cuteime_platform_qws cuteime_platform_embedded
            DEFINES += CUTEIME_PLATFORM_QWS CUTEIME_PLATFORM_EMBEDDED
        } else {
            CONFIG += cuteime_platform_x11 cuteime_platform_desktop
            DEFINES += CUTEIME_PLATFORM_X11 CUTEIME_PLATFORM_DESKTOP
        }
    }
} else {
    windows {
        CONFIG += cuteime_platform_windows
        DEFINES += CUTEIME_PLATFORM_WINDOWS
        contains(QT_CONFIG, embedded) {
            CONFIG += cuteime_platform_wince cuteime_platform_embedded
            DEFINES += CUTEIME_PLATFORM_WINCE CUTEIME_PLATFORM_EMBEDDED
        } else {
            CONFIG += cuteime_platform_win32 cuteime_platform_desktop
            DEFINES += CUTEIME_PLATFORM_WIN32 CUTEIME_PLATFORM_DESKTOP
        }
    }
}
