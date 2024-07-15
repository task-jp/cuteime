TEMPLATE = subdirs

isEqual(QT_MAJOR_VERSION, 5) {
    SUBDIRS = system languages inputmethods engines

    !contains(CUTEIME_CONFIG, no-keyboard) {
        SUBDIRS += converters interpreters
    }

    !contains(CUTEIME_CONFIG, no-gui) {
        SUBDIRS += clients uis
    }
} else {
    SUBDIRS += clients
}
