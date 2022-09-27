include(../../../../cuteimeplatform.pri)
TEMPLATE = subdirs

!contains(CUTEIME_CONFIG, no-gtk2immodule) SUBDIRS += gtk2
!contains(CUTEIME_CONFIG, no-gtk3immodule) SUBDIRS += gtk3
