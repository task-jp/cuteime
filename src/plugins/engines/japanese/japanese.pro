include(../../../../cuteimeplatform.pri)
TEMPLATE = subdirs

cuteime_platform_linux {
    !contains(CUTEIME_CONFIG, no-anthy) SUBDIRS += anthy
}

!contains(CUTEIME_CONFIG, no-socialime) SUBDIRS += socialime
!contains(CUTEIME_CONFIG, no-googleime) SUBDIRS += googleime
