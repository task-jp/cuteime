TEMPLATE = lib
TARGET = cuteime_qt$$QT_MAJOR_VERSION

include(../../../cuteimelibrary.pri)

isEqual(QT_MAJOR_VERSION, 5): cuteime_platform_x11: QT += x11extras

DEFINES += CUTEIME_LIBRARY
RESOURCES += qt.qrc

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

LIB_HEADERS *= $$PWD/cuteimeglobal.h

include(ipc/ipc.pri)
include(items/items.pri)
include(debug/debug.pri)
isEqual(QT_MAJOR_VERSION, 5) {
    include(plugins/plugins.pri)
    include(utils/utils.pri)
    include(server/server.pri)
    include(widgets/widgets.pri)
}
contains(CUTEIME_CONFIG, tests) {
    include(test/test.pri)
}

HEADERS *= $$LIB_HEADERS

contains(CUTEIME_CONFIG, sdk) {
    lib_headers.files = $$LIB_HEADERS
    lib_headers.path = $$PREFIX/include/cuteime/qt$$QT_MAJOR_VERSION
    INSTALLS += lib_headers
}

