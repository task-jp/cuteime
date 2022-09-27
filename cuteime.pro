TEMPLATE = subdirs
CONFIG += ordered

isEqual(QT_MAJOR_VERSION, 5) {
    SUBDIRS += src share
    contains(CUTEIME_CONFIG, sdk) : SUBDIRS += doc tools
    contains(CUTEIME_CONFIG, examples) : SUBDIRS += examples
    contains(CUTEIME_CONFIG, tests) {
        SUBDIRS += tests

        test.commands = "for test in `find bin -name \"tst_cuteime*\" -executable -type f`; do \$\$test; done"
        QMAKE_EXTRA_TARGETS += test
    }
}

isEqual(QT_MAJOR_VERSION, 4) {
    SUBDIRS += src
}

OTHER_FILES += configure.rb
