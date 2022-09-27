TEMPLATE = subdirs

SUBDIRS = full half

contains(CUTEIME_CONFIG, tests) {
    SUBDIRS += full/test
}
