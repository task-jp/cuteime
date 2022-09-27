TARGET = im-cuteime-qt4

isEmpty(QT_IM_MODULE_DIR) {
    target.path = $$[QT_INSTALL_PLUGINS]/inputmethods
} else {
    target.path = $$QT_IM_MODULE_DIR
}

include(../../../../cuteimeplugin.pri)

DESTDIR = $$CUTEIME_BUILD_TREE/$$CUTEIME_LIBRARY_PATH/plugins/inputmethods
SOURCES += inputcontext.cpp \
    inputcontextplugin.cpp
HEADERS += inputcontext.h \
    inputcontextplugin.h
