TARGET = cuteimegui
TARGETPATH = Cuteime/Gui
include(../../../../cuteimeimports.pri)

SOURCES += \
    plugin.cpp

HEADERS += \
    plugin.h

COPY_TARGET = qml/Cuteime/Gui
COPY_FILES  = $$PWD/ScreenInfo.qml $$PWD/Action.qml $$PWD/Widget.qml $$PWD/LineEdit.qml
COPY_FILES += $$PWD/Label.qml $$PWD/Button.qml $$PWD/PushButton.qml $$PWD/CheckBox.qml $$PWD/RadioButton.qml
COPY_FILES += $$PWD/CuteimeIcon.qml $$PWD/TabWidget.qml $$PWD/ToolButton.qml $$PWD/StackedWidget.qml
COPY_FILES += $$PWD/cuteime-pin.png $$PWD/cuteime-stick.png $$PWD/cuteime-wing.png $$PWD/cuteime.png
include(../../../../cuteimedata.pri)

OTHER_FILES += $$COPY_FILES
