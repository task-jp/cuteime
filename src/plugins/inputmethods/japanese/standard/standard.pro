TARGET = japanese_standard
include(../../../../../cuteimeplugin.pri)

HEADERS += plugin.h inputmethod.h keyactions.h global.h
SOURCES += plugin.cpp inputmethod.cpp keyactions.cpp

RESOURCES += \
    standard.qrc
