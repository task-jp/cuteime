DEBUG_HEADERS = $$PWD/cuteimedebug.h
DEBUG_SOURCES = $$PWD/cuteimedebug.c
INCLUDEPATH += $$PWD

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

HEADERS *= $$DEBUG_HEADERS
SOURCES *= $$DEBUG_SOURCES

contains(CUTEIME_CONFIG, sdk) {
	debug_headers.files = $$DEBUG_HEADERS
    debug_headers.path = $$PREFIX/include/cuteime/gtk
	INSTALLS += debug_headers
}

