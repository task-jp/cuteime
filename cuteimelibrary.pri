isEmpty(CUTEIMELIBRARY_PRI) {
CUTEIMELIBRARY_PRI = 1

include(./cuteime.pri)

win32 {
    DLLDESTDIR = $$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_LIBRARY_PATH
}

TEMPLATE = lib

contains(QT_CONFIG, reduce_exports) {
    contains(CONFIG, qt): CONFIG += hide_symbols
}

TARGET = $$qtLibraryTarget($$TARGET)

DESTDIR = $$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_LIBRARY_PATH

QMAKE_RPATHDIR += \$\$ORIGIN/../$$CUTEIME_LIBRARY_PATH
include(./cuteimerpath.pri)

cuteime_platform_linux {
    target.path = $$PREFIX/$$CUTEIME_TARGET_PATH/$$CUTEIME_LIBRARY_PATH
    INSTALLS += target
}

} # CUTEIMELIBRARY_PRI
