include(../../../cuteimeplatform.pri)
TEMPLATE = subdirs

SUBDIRS *= candidatewindow
cuteime_platform_desktop {
    SUBDIRS *= aboutcuteime settings userdictionary
    !contains(CUTEIME_CONFIG, no-systemtray) SUBDIRS += systemtray
    !contains(CUTEIME_CONFIG, no-toolbar) SUBDIRS += toolbar
}
