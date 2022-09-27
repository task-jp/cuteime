isEmpty(CUTEIMEIMPORTS_PRI) {
CUTEIMEIMPORTS_PRI = 1

include(./cuteime.pri)
include(./src/libs/libs.pri)

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

TEMPLATE = lib
CONFIG += plugin
QT += declarative

TARGET = $$qtLibraryTarget($$TARGET)

DESTDIR = $$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_IMPORTS_PATH/$$TARGETPATH

isEmpty(target.path) {
    target.path = $$PREFIX/$$CUTEIME_TARGET_PATH/$$CUTEIME_IMPORTS_PATH/$$TARGETPATH
}
INSTALLS += target

QMAKE_RPATHDIR += \$\$ORIGIN/../../../../
include(./cuteimerpath.pri)

qmldir.files += $$_PRO_FILE_PWD_/qmldir
qmldir.path = $$target.path
INSTALLS += qmldir

!equals(_PRO_FILE_PWD_, $$DESTDIR) {
    copy_qmldir.target = $${DESTDIR}/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

} # CUTEIMEIMPORTS_PRI
