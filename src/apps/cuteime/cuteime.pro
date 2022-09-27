include(../../../cuteime.pri)
TARGET = $$CUTEIME_APP_TARGET
include(../../../cuteimeapp.pri)
include(../../../share/share.pri)

contains(CUTEIME_CONFIG, no-gui) {
    QT -= gui
} else {
    QT += widgets
}

SOURCES += main.cpp \
    application.cpp
HEADERS += application.h
