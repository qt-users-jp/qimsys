import Qt 4.7
import "../../Qimsys/Gui"

Item {
    id: root

    ScreenInfo {
        id: screenInfo
    }

    width: screenInfo * 10
    height: screenInfo * 2

    property alias first: firstText.text
    property alias second: secondText.text

    property color color: "black"

    Row {
        anchors.fill: parent
        Rectangle {
            width: parent.width / 2 - 1
            height: parent.height - 1
            color: root.color
            border.color: "white"
            border.width: 1
            radius: height / 5

            Label {
                action: Action {
                    id: firstText
                }
                anchors.centerIn: parent
            }
        }
        Rectangle {
            width: parent.width / 2 - 1
            height: parent.height - 1
            color: root.color
            border.color: "white"
            border.width: 1
            radius: height / 5

            Label {
                action: Action {
                    id: secondText
                }
                anchors.centerIn: parent
            }
        }
    }
}
