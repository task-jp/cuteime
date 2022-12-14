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

Button {
    id: root
    className: 'CheckBox'

    action: Action { text: "CheckBox"; checkable: true }

    width: screenInfo.width * 10
    height: screenInfo.height

    Rectangle {
        id: check
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: screenInfo.unit * 0.25
        width: height

        color: screenInfo.base
        border.color: screenInfo.background
        border.width: 1
        radius: screenInfo.unit * 0.15

        Text {
            anchors.centerIn: parent
            color: screenInfo.text
            text: '\u2713'
            font.pixelSize: screenInfo.fontSize
            visible: root.action.checked
        }
    }

    Label {
        id: label
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: check.right
        anchors.right: parent.right

        anchors.margins: screenInfo.unit * 0.1
        action: root.action
    }

    Connections {
        target: root
        onClicked: {
            action.toggle()
        }
    }
}
