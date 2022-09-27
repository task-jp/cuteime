IPC_HEADERS +=  $$PWD/cuteimemarshalers.h $$PWD/cuteimeabstractipcobject.h $$PWD/cuteimeapplicationmanager.h $$PWD/cuteimeinputmethodmanager.h $$PWD/cuteimekeymanager.h $$PWD/cuteimepreeditmanager.h $$PWD/cuteimepreedititem.h $$PWD/cuteimekeyboardmanager.h
IPC_SOURCES += $$PWD/cuteimemarshalers.c $$PWD/cuteimeabstractipcobject.c $$PWD/cuteimeapplicationmanager.c $$PWD/cuteimeinputmethodmanager.c $$PWD/cuteimekeymanager.c $$PWD/cuteimepreeditmanager.c $$PWD/cuteimepreedititem.c $$PWD/cuteimekeyboardmanager.c

CONFIG += link_pkgconfig
PKGCONFIG += dbus-glib-1 gdk-pixbuf-2.0

INCLUDEPATH += $$PWD

HEADERS *= $$IPC_HEADERS $$PWD/dbus.h
SOURCES *= $$IPC_SOURCES

contains(CUTEIME_CONFIG, sdk) {
    ipc_headers.files = $$IPC_HEADERS
    ipc_headers.path = $$PREFIX/include/cuteime/gtk
    INSTALLS += ipc_headers
}

OTHER_FILES += \
    ipc/cuteimemarshaler.list

cuteimemarshalers_list.target = $$PWD/cuteimemarshalers.list
QMAKE_EXTRA_TARGETS += cuteimemarshalers_list

cuteimemarshalers_h.target = $$PWD/cuteimemarshalers.h
cuteimemarshalers_h.path = $$PWD/cuteimemarshalers.h
cuteimemarshalers_h.depends = $$PWD/cuteimemarshalers.list
cuteimemarshalers_h.commands = \
    glib-genmarshal --prefix=cuteime_marshal $$PWD/cuteimemarshaler.list --header | grep -v -e \"^/\\*.*\\*/\" > cuteimemarshalers.h.tmp \
    && (cmp -s $$PWD/cuteimemarshalers.h cuteimemarshalers.h.tmp || cp cuteimemarshalers.h.tmp $$PWD/cuteimemarshalers.h);\
    rm cuteimemarshalers.h.tmp

QMAKE_EXTRA_TARGETS += cuteimemarshalers_h

cuteimemarshalers_c.target = $$PWD/cuteimemarshalers.c
cuteimemarshalers_c.path = $$PWD/cuteimemarshalers.c
cuteimemarshalers_c.depends = $$PWD/cuteimemarshalers.list $$PWD/cuteimemarshalers.h
cuteimemarshalers_c.commands = \
    glib-genmarshal --prefix=cuteime_marshal $$PWD/cuteimemarshaler.list --body | grep -v -e \"^/\\*.*\\*/\" > cuteimemarshalers.c.tmp \
    && (cmp -s $$PWD/cuteimemarshalers.c cuteimemarshalers.c.tmp || cp cuteimemarshalers.c.tmp $$PWD/cuteimemarshalers.c);\
    rm cuteimemarshalers.c.tmp

QMAKE_EXTRA_TARGETS += cuteimemarshalers_c

