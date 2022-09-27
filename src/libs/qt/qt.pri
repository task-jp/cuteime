INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
INCLUDEPATH += $$PWD/debug
DEPENDPATH += $$PWD/debug
INCLUDEPATH += $$PWD/ipc
DEPENDPATH += $$PWD/ipc
INCLUDEPATH += $$PWD/items
DEPENDPATH += $$PWD/items
INCLUDEPATH += $$PWD/plugins
DEPENDPATH += $$PWD/plugins
INCLUDEPATH += $$PWD/server
DEPENDPATH += $$PWD/server
INCLUDEPATH += $$PWD/test
DEPENDPATH += $$PWD/test
INCLUDEPATH += $$PWD/utils
DEPENDPATH += $$PWD/utils
INCLUDEPATH += $$PWD/widgets
DEPENDPATH += $$PWD/widgets


include(../../../cuteime.pri)

LIBS *= -L$$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_LIBRARY_PATH

LIBS *= -l$$qtLibraryName(cuteime_qt$$QT_MAJOR_VERSION)

!contains(CUTEIME_CONFIG, no-dbus): QT += dbus
