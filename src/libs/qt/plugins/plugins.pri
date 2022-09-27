PLUGIN_SOURCES = $$PWD/cuteimepluginmanager.cpp $$PWD/cuteimeplugin.cpp $$PWD/cuteimeconverter.cpp $$PWD/cuteimeengine.cpp $$PWD/cuteimeenginedictionary.cpp $$PWD/cuteimeabstractpluginobject.cpp $$PWD/cuteimelanguage.cpp $$PWD/cuteimeinputmethod.cpp $$PWD/cuteimeinterpreter.cpp
PLUGIN_HEADERS = $$PWD/cuteimepluginmanager.h $$PWD/cuteimeplugin.h $$PWD/cuteimeconverter.h $$PWD/cuteimeengine.h $$PWD/cuteimeenginedictionary.h $$PWD/cuteimeabstractpluginobject.h $$PWD/cuteimelanguage.h $$PWD/cuteimeinputmethod.h $$PWD/cuteimeinterpreter.h

!contains(CUTEIME_CONFIG, no-gui) {
    QT += widgets
    PLUGIN_SOURCES += $$PWD/cuteimesettingswidget.cpp
    PLUGIN_HEADERS += $$PWD/cuteimesettingswidget.h
    FORMS += $$PWD/cuteimesettingswidget.ui
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS *= $$PLUGIN_HEADERS
SOURCES *= $$PLUGIN_SOURCES

contains(CUTEIME_CONFIG, sdk) {
    plugin_headers.files = $$PLUGIN_HEADERS
    plugin_headers.path = $$PREFIX/include/cuteime/qt
    INSTALLS += plugin_headers
}

OTHER_FILES += \
    $$PWD/cuteimeabstractpluginobject.dox \
    $$PWD/cuteimeconverter.dox \
    $$PWD/cuteimeengine.dox \
    $$PWD/cuteimeplugin.dox \
    $$PWD/cuteimepluginmanager.dox \
    $$PWD/cuteimesettingswidget.dox
