include(../../cuteime.pri)

cuteime_platform_x11 {
    desktop.files = $$PWD/cuteime.desktop
	desktop.path = $$PREFIX/share/applications
    INSTALLS += desktop
}

