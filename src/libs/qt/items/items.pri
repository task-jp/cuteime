ITEMS_HEADERS = $$PWD/cuteimeconversionitem.h $$PWD/cuteimepreedititem.h
ITEMS_SOURCES = $$PWD/cuteimeconversionitem.cpp $$PWD/cuteimepreedititem.cpp
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS *= $$ITEMS_HEADERS
SOURCES *= $$ITEMS_SOURCES

contains(CUTEIME_CONFIG, sdk) {
    item_headers.files = $$ITEMS_HEADERS
    item_headers.path = $$PREFIX/include/cuteime/qt
	INSTALLS += item_headers
}

OTHER_FILES += \
    $$PWD/cuteimeconversionitem.dox \
    $$PWD/cuteimepreedititem.dox
