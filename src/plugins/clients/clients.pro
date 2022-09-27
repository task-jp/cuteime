include(../../../cuteimeplatform.pri)
TEMPLATE = subdirs

cuteime_platform_linux {
    isEqual(QT_MAJOR_VERSION, 5) {
        !contains(CUTEIME_CONFIG, no-dbus) {
            !contains(CUTEIME_CONFIG, no-qt5immodule) SUBDIRS += qt5immodule
            !contains(CUTEIME_CONFIG, no-gtk) SUBDIRS += gtkimmodule
        }

        !contains(CUTEIME_CONFIG, no-xim) {
            CONFIG += ordered
            SUBDIRS += xim/IMdkit xim
        }
    }

    isEqual(QT_MAJOR_VERSION, 4) {
        !contains(CUTEIME_CONFIG, no-qt4immodule) SUBDIRS += qt4immodule
    }
}
