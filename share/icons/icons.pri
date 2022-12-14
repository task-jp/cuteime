include(../../cuteime.pri)

cuteime_platform_x11 {
    icon16x16.files = $$PWD/16x16/cuteime.png
    icon16x16.path = $$PREFIX/share/icons/hicolor/16x16/apps
    INSTALLS += icon16x16

    icon22x22.files = $$PWD/22x22/cuteime.png
    icon22x22.path = $$PREFIX/share/icons/hicolor/22x22/apps
    INSTALLS += icon22x22

    icon32x32.files = $$PWD/32x32/cuteime.png
    icon32x32.path = $$PREFIX/share/icons/hicolor/32x32/apps
    INSTALLS += icon32x32

    icon64x64.files = $$PWD/64x64/cuteime.png
    icon64x64.path = $$PREFIX/share/icons/hicolor/64x64/apps
    INSTALLS += icon64x64

    icon128x128.files = $$PWD/128x128/cuteime.png
    icon128x128.path = $$PREFIX/share/icons/hicolor/128x128/apps
    INSTALLS += icon128x128
}
