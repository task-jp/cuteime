DEBUG_HEADERS = $$PWD/cuteimedebug.h
DEBUG_SOURCES = $$PWD/cuteimedebug.cpp
INCLUDEPATH += $$PWD

HEADERS *= $$DEBUG_HEADERS
SOURCES *= $$DEBUG_SOURCES

contains(CUTEIME_CONFIG, sdk) {
    debug_headers.files = $$DEBUG_HEADERS
    debug_headers.path = $$PREFIX/include/cuteime/qt
    INSTALLS += debug_headers
}

OTHER_FILES += \
    $$PWD/cuteimedebug.dox
