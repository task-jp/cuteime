TARGET = im-cuteime-qt6

isEmpty(QT_IM_MODULE_DIR) {
    target.path = $$[QT_INSTALL_PLUGINS]/platforminputcontexts
} else {
    target.path = $$QT_IM_MODULE_DIR
}

include(../../../../cuteimeplugin.pri)

DESTDIR = $$CUTEIME_BUILD_TREE/$$CUTEIME_LIBRARY_PATH/plugins/platforminputcontexts

QT += gui-private widgets quick

SOURCES += inputcontext.cpp \
    main.cpp
HEADERS += inputcontext.h
