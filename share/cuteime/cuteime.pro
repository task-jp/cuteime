TEMPLATE = subdirs
include(../../cuteime.pri)
!contains(CUTEIME_CONFIG, no-translation) {
    SUBDIRS = translations
}
