isEmpty(CUTEIMEPLUGIN_PRI) {
CUTEIMEPLUGIN_PRI = 1

include(./cuteime.pri)
include(./src/libs/libs.pri)

contains(QT_CONFIG, reduce_exports) {
    contains(CONFIG, qt): CONFIG += hide_symbols
}

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget($$TARGET)

DESTDIR = $$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_PLUGIN_PATH/

isEmpty(target.path) {
    target.path = $$PREFIX/$$CUTEIME_TARGET_PATH/$$CUTEIME_PLUGIN_PATH/
}
INSTALLS += target

QMAKE_RPATHDIR += \$\$ORIGIN/../../../$$CUTEIME_LIBRARY_PATH
include(./cuteimerpath.pri)

} # CUTEIMEPLUGIN_PRI
