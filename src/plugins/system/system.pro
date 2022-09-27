include(../../../cuteimeplatform.pri)
TEMPLATE = subdirs

cuteime_platform_linux {
    SUBDIRS += autostart
}

SUBDIRS += inputmethodmanager language
