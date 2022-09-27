TEMPLATE = lib
TARGET = cuteime_gtk

CONFIG += gtk
CONFIG -= qt

include(../../../cuteimelibrary.pri)

DEFINES += CUTEIME_LIBRARY

LIB_HEADERS = $$PWD/cuteimeglobal.h

HEADERS *= $$LIB_HEADERS

contains(CUTEIME_CONFIG, sdk) {
    lib_headers.files = $$LIB_HEADERS
    lib_headers.path = $$PREFIX/include/cuteime/gtk
    INSTALLS += lib_headers
}

include(debug/debug.pri)
include(ipc/ipc.pri)
