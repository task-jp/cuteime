INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD/ipc
DEPENDPATH += $$PWD/ipc
INCLUDEPATH += $$PWD/debug
DEPENDPATH += $$PWD/debug
include(../../../cuteime.pri)

CONFIG += link_pkgconfig
PKGCONFIG += dbus-glib-1

LIBS += -L$$CUTEIME_BUILD_TREE/$$CUTEIME_LIBRARY_PATH

LIBS *= -l$$qtLibraryName(cuteime_gtk)
