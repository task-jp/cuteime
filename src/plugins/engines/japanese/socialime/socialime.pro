include(../../../../../cuteimeplugin.pri)

QT += network

HEADERS += engine.h \
    plugin.h

SOURCES += engine.cpp \
    plugin.cpp

!contains(CUTEIME_CONFIG, no-gui) {
    QT += widgets
    HEADERS += settings.h
    SOURCES += settings.cpp
    RESOURCES += socialime.qrc
    FORMS += settings.ui
}
