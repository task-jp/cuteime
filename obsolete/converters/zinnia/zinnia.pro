include(../../../../cuteimeplugin.pri)

CONFIG += link_pkgconfig
PKGCONFIG += zinnia

HEADERS += zinniaplugin.h zinniaobject.h \
    zinniawidget.h \
    zinniaengine.h
SOURCES += zinniaplugin.cpp zinniaobject.cpp \
    zinniawidget.cpp \
    zinniaengine.cpp

RESOURCES += \
    zinnia.qrc

DEFINES += CUTEIME_ZINNIA_DEFAULT_MODEL_PATH=\\\"$$CUTEIME_ZINNIA_DEFAULT_MODEL_PATH\\\"
