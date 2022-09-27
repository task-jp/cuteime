include(../../../cuteime.pri)

TEMPLATE = subdirs
versionCheck(4,7,0) {
    !contains(CUTEIME_CONFIG, no-gui): SUBDIRS = statusviewer
}
