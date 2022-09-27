TEMPLATE = subdirs
!contains(CUTEIME_CONFIG, no-dbus) {
    SUBDIRS = cuteimeabstractipcobject cuteimeapplicationmanager cuteimeinputmethodmanager cuteimepreeditmanager cuteimecandidatemanager cuteimekeyboardmanager
}
