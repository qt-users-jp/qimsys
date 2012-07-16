import Qt 4.7
import Qimsys.Core 1.0 as Qimsys

Rectangle {
    width: 200
    height: 200

    Qimsys.ApplicationManager {
        id: manager
    }

    Qimsys.Preedit {
        id: preedit
        onItemsChanged: {
            var str = "";
            console.debug(preedit.items.length)
            for (var i = 0; i < preedit.items.length; i++) {
                str += preedit.items[i].text
            }
            preeditArea.text = str
        }
    }

    Qimsys.KeyManager {
        id: keyManager
        onKeyPressed: {
            console.debug(text + "\t" + keycode + "\t" + modifiers + "\t" + autoRepeat)
        }
    }

    Text {
        id: preeditArea
        font: preedit.font
    }
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            keyManager.process("Q", 113, 0, true, false)
        }
        onExited: {
            keyManager.process("Q", 113, 0, false, false)
        }
    }
}
