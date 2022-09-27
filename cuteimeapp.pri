isEmpty(CUTEIMEAPP_PRI) {
CUTEIMEAPP_PRI = 1

include(./cuteime.pri)
TEMPLATE = app
macx {
    DESTDIR = $$CUTEIME_BUILD_TREE
} else {
    DESTDIR = $$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_APP_PATH
}

QMAKE_RPATHDIR += \$\$ORIGIN/../$$CUTEIME_LIBRARY_PATH
include(./cuteimerpath.pri)
include(./src/libs/libs.pri)

cuteime_platform_linux {
    target.path = $$PREFIX/$$CUTEIME_APP_PATH
    INSTALLS += target
}

} # CUTEIMEAPP_PRI
