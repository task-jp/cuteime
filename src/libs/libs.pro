include(../../cuteimeplatform.pri)
TEMPLATE = subdirs

SUBDIRS = qt

isEqual(QT_MAJOR_VERSION, 5): cuteime_platform_linux {
    !contains(CUTEIME_CONFIG, no-dbus) {
        !contains(CUTEIME_CONFIG, no-gtk): SUBDIRS += gtk
    }
}
