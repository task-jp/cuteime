include(../../../../cuteimeplugin.pri)

HEADERS += object.h \
    plugin.h

SOURCES += object.cpp \
    plugin.cpp

!contains(CUTEIME_CONFIG, no-gui) {
    QT += widgets
    HEADERS += settings.h
    SOURCES += settings.cpp
    RESOURCES += language.qrc
    FORMS += settings.ui
}
