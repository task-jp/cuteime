IPC_SOURCES = $$PWD/dbus.cpp \
    $$PWD/cuteimeabstractipcobject.cpp \
    $$PWD/cuteimeapplicationmanager.cpp \
    $$PWD/cuteimepreeditmanager.cpp \
    $$PWD/cuteimecandidatemanager.cpp \
    $$PWD/cuteimekeymanager.cpp \
    $$PWD/cuteimeinputmethodmanager.cpp \
    $$PWD/cuteimekeyboardmanager.cpp

IPC_HEADERS = $$PWD/dbus.h \
    $$PWD/cuteimeabstractipcobject.h \
    $$PWD/cuteimeapplicationmanager.h \
    $$PWD/cuteimepreeditmanager.h \
    $$PWD/cuteimecandidatemanager.h \
    $$PWD/cuteimekeymanager.h \
    $$PWD/cuteimeinputmethodmanager.h \
    $$PWD/cuteimekeyboardmanager.h

!contains(CUTEIME_CONFIG, no-gui) {
    cuteime_platform_x11 {
        IPC_SOURCES += $$PWD/cuteimekeymanager_x11.cpp
    }
    cuteime_platform_qws {
        IPC_SOURCES += $$PWD/cuteimekeymanager_qws.cpp
    }
    cuteime_platform_mac {
        IPC_SOURCES += $$PWD/cuteimekeymanager_mac.cpp
    }
    cuteime_platform_win32 {
        IPC_SOURCES += $$PWD/cuteimekeymanager_win.cpp
    }
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!contains(CUTEIME_CONFIG, no-dbus): QT += dbus

HEADERS *= $$IPC_HEADERS
SOURCES *= $$IPC_SOURCES

contains(CUTEIME_CONFIG, sdk) {
    ipc_headers.files = $$IPC_HEADERS
    ipc_headers.path = $$PREFIX/include/cuteime/qt
    INSTALLS += ipc_headers
}

OTHER_FILES += \
    $$PWD/cuteimeabstractipcobject.dox \
    $$PWD/cuteimeapplicationmanager.dox \
    $$PWD/cuteimepreeditmanager.dox \
    $$PWD/cuteimecandidatemanager.dox \
    $$PWD/cuteimekeymanager.dox
