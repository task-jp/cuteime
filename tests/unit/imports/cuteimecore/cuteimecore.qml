import Qt 4.7
import Cuteime.Core 1.0 as Cuteime

Rectangle {
    width: 200
    height: 200

    Cuteime.ApplicationManager {
        id: manager
    }

    Cuteime.Preedit {
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

    Cuteime.KeyManager {
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
