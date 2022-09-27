isEmpty(CUTEIMETEST_PRI) {
CUTEIMETEST_PRI = 1

include(./cuteime.pri)
contains(CUTEIME_CONFIG, no-gui): QT -= gui
TEMPLATE = app
QT += testlib concurrent

CONFIG += testcase
DESTDIR = $$CUTEIME_BUILD_TREE/$$CUTEIME_TARGET_PATH/$$CUTEIME_APP_PATH
QMAKE_RPATHDIR += \$\$ORIGIN/../$$CUTEIME_LIBRARY_PATH
include(./cuteimerpath.pri)
include(./src/libs/libs.pri)

} # CUTEIMETEST_PRI
