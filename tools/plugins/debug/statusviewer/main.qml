/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   cuteime                                                                  *
 *   Copyright (C) 2010 by Tasuku Suzuki <stasuku@gmail.com>                 *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Lesser Public License as          *
 *   published by the Free Software Foundation; either version 2 of the      *
 *   License, or (at your option) any later version.                         *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU Lesser General Public License for more details.                     *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *
 *   License along with this program; if not, write to the                   *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
import Qt 4.7
import Cuteime.Core 1.0 as CuteimeCore
import "../../Cuteime/Gui" as CuteimeGui

Rectangle {
    id: root
    width: 500
    height: 600
    color: globalScreenInfo.background

    CuteimeGui.ScreenInfo {
        id: globalScreenInfo
        unit: 30
    }

    CuteimeCore.ApplicationManager {
        id: applicationManager
    }

    CuteimeCore.InputMethodManager {
        id: inputMethodManager
    }

    CuteimeCore.PreeditManager {
        id: preeditManager
    }

    CuteimeCore.CandidateManager {
        id: candidateManager
    }

    CuteimeCore.KeyboardManager {
        id: keyboardManager
    }

    function list2str(list) {
        var ret = '['
        for (var i = 0; i < list.length; i++) {
            if (i != 0) ret += ', '
            ret += list[i]
        }
        return ret + ']'
    }

    function rect2str(rect) {
        return rect.x + ', ' + rect.y + ' ' + rect.width + ' - ' + rect.height
    }

    Flickable {
        anchors.fill: parent
        anchors.margins: globalScreenInfo.unit * 0.1
        contentHeight: container.height
        contentWidth: container.width

        Grid {
            id: container
            columns: 2
            spacing: globalScreenInfo.unit * 0.1

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'ApplicationManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'displayLanguage' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: applicationManager.displayLanguage } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'focus' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: applicationManager.focus ? 'True' : 'False' } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'window' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: applicationManager.window } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'widget' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: applicationManager.widget } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'composing' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: applicationManager.composing ? 'True' : 'False' } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'exec' } }
            Column {
                CuteimeGui.PushButton { action: CuteimeGui.Action { text: "Reset"; onTriggered: applicationManager.exec(0) } }
                CuteimeGui.PushButton { action: CuteimeGui.Action { text: "Dict"; onTriggered: applicationManager.exec(1) } }
                CuteimeGui.PushButton { action: CuteimeGui.Action { text: "Settings"; onTriggered: applicationManager.exec(2) } }
                CuteimeGui.PushButton { action: CuteimeGui.Action { text: "About"; onTriggered: applicationManager.exec(3) } }
                CuteimeGui.PushButton { action: CuteimeGui.Action { text: "Controller"; onTriggered: applicationManager.exec(4) } }
            }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'InputMethodManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'locale' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: inputMethodManager.locale } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'identifier' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: inputMethodManager.identifier } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'converter' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: inputMethodManager.converter } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'interpreter' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: inputMethodManager.interpreter } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'engine' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: inputMethodManager.engine } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'state' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: inputMethodManager.state } }


            CuteimeGui.Label { action: CuteimeGui.Action { text: 'PreeditManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'item.to' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: list2str(preeditManager.item.to) } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'item.from' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: list2str(preeditManager.item.from) } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'item.rawString' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: list2str(preeditManager.item.rawString) } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'item.cursor' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: preeditManager.item.cursor } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'item.selection' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: preeditManager.item.selection } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'item.modified' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: preeditManager.item.modified } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'rect' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: rect2str(preeditManager.rect) } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'font' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: 'Font' } font: preeditManager.font }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'cursorPosition' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: preeditManager.cursorPosition } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'surroundingText' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: preeditManager.surroundingText } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'currentSelection' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: preeditManager.currentSelection } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'maximumTextLength' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: preeditManager.maximumTextLength } }


            CuteimeGui.Label { action: CuteimeGui.Action { text: 'CandidateManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'items' } }
            Column {
                Repeater {
                    model: candidateManager.items
                    delegate: Row {
                        spacing: globalScreenInfo.unit * 0.1
                        CuteimeGui.Label { action: CuteimeGui.Action { text: model.modelData.index } }
                        CuteimeGui.Label { action: CuteimeGui.Action { text: model.modelData.from } }
                        CuteimeGui.Label { action: CuteimeGui.Action { text: model.modelData.to } }
                        CuteimeGui.Label { action: CuteimeGui.Action { text: model.modelData.hint } }
                        CuteimeGui.Label { action: CuteimeGui.Action { text: model.modelData.source } }
                    }
                }
                Rectangle{ width: 1; height: 1; color: root.color }
            }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'currentIndex' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: candidateManager.currentIndex } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'KeyboardManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'visible' } }
            CuteimeGui.CheckBox { action: CuteimeGui.Action { text: keyboardManager.visible; checkable: true; checked: keyboardManager.visible; onCheckedChanged: keyboardManager.visible = checked } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'geometry' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: rect2str(keyboardManager.geometry) } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'focusArea' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: rect2str(keyboardManager.focusArea) } }

            CuteimeGui.Label { action: CuteimeGui.Action { text: 'sharedData' } }
            CuteimeGui.Label { action: CuteimeGui.Action { text: keyboardManager.sharedData } }
}
    }
}
