import Qt 4.7
import "../../Qimsys/Gui"

Flickable {
    id: root
    width: 360
    height: 200
    anchors.fill: parent
    contentWidth: container.width
    contentHeight: container.height
    clip: true

    ScreenInfo {
        id: screenInfo
    }

    property int index: -1

    function save() {
        var rawData = new Array
        rawData.push(sound.text)
        rawData.push(spelling.text)

        // http://code.google.com/p/macuim/source/browse/trunk/Anthy/AnthyDict/dict-canna-cclass.c?r=127
        if (generalNown.checked) {
            rawData.push('#KK')
        } else if (nameOfPeople.checked) {
            rawData.push('#JNM')
        }

        rawData.push(500) // Frequency

        userDictionaryModel.setRawData(index, rawData)
    }

    function load(entry) {
        root.index = entry
        if (entry == -1) {
            sound.text = ''
            spelling.text = ''
            generalNown.checked = true
        } else {
            var rawData = userDictionaryModel.rawData(entry)
            sound.text = rawData[0]
            spelling.text = rawData[1]
            if (rawData[2] == '#JNM') {
                nameOfPeople.checked = true
            } else {
                generalNown.checked = true
            }
        }
    }

    function del() {
        userDictionaryModel.setRawData(index, undefined)
    }

    Rectangle {
        id: container
        width: root.width
        height: dataArea.height + 2 * dataArea.anchors.margins
        color: "black"
        Column {
            id: labels
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: screenInfo.unit * 0.1
            spacing: screenInfo.unit * 0.1

            Label {
                action: Action { text: "Sound" }
                height: screenInfo.height
            }

            Label {
                action: Action { text: "Spelling" }
                height: screenInfo.height
            }

            Label {
                action: Action { text: "Word type" }
                height: screenInfo.height
            }
        }

        Column {
            id: dataArea
            anchors.top: parent.top
            anchors.left: labels.right
            anchors.right: parent.right
            anchors.margins: screenInfo.unit * 0.1
            spacing: screenInfo.unit * 0.1

            LineEdit {
                width: parent.width
                action: Action { id: sound }
            }

            LineEdit {
                width: parent.width
                action: Action { id: spelling }
            }

            RadioButton {
                width: parent.width
                action: Action { id: generalNown; text: 'General Noun'; checkable: true }
            }
            RadioButton {
                width: parent.width
                action: Action { id: nameOfPeople; text: 'Name of People'; checkable: true }
            }
        }
    }
}
