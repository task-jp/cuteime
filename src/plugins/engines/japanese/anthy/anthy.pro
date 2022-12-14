include(../../../../../cuteimeplugin.pri)

HEADERS += plugin.h engine.h \
    libanthy.h \
    libanthydic.h \
    dictionarymodel.h \
    dictionary.h

SOURCES += plugin.cpp engine.cpp \
    libanthy.cpp \
    libanthydic.cpp \
    dictionarymodel.cpp \
    dictionary.cpp

!contains(CUTEIME_CONFIG, no-gui) {
    QT += widgets
    HEADERS += wordeditor.h settings.h
    SOURCES += wordeditor.cpp settings.cpp

    RESOURCES += anthy.qrc
    FORMS += wordeditor.ui settings.ui
}
