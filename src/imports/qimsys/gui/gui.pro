TARGET = qimsysgui
TARGETPATH = Qimsys/Gui
include(../../../../qimsysimports.pri)

SOURCES += \
    plugin.cpp

HEADERS += \
    plugin.h

COPY_TARGET = qml/Qimsys/Gui
COPY_FILES  = $$PWD/ScreenInfo.qml $$PWD/Action.qml $$PWD/Widget.qml $$PWD/LineEdit.qml
COPY_FILES += $$PWD/Label.qml $$PWD/Button.qml $$PWD/PushButton.qml $$PWD/CheckBox.qml $$PWD/RadioButton.qml
COPY_FILES += $$PWD/QimsysIcon.qml $$PWD/TabWidget.qml $$PWD/ToolButton.qml $$PWD/StackedWidget.qml
COPY_FILES += $$PWD/qimsys-pin.png $$PWD/qimsys-stick.png $$PWD/qimsys-wing.png $$PWD/qimsys.png
include(../../../../qimsysdata.pri)

OTHER_FILES += $$COPY_FILES
