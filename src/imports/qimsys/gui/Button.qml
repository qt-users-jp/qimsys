/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
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

Widget {
    id: root

    property variant action: Action { text: "Button" }

    width: screenInfo.width * 5
    height: screenInfo.height

    property alias isPressed: mouse.pressed
    signal clicked

    property color color: mouse.pressed ? screenInfo.highlight : screenInfo.button

    Rectangle {
        id: background
        anchors.fill: parent
        anchors.rightMargin: 1
        anchors.bottomMargin: 1
        color: root.color
        radius: screenInfo.unit * 0.20
    }

    Rectangle {
        anchors.fill: parent
        anchors.rightMargin: 1
        anchors.bottomMargin: 1
        radius: screenInfo.unit * 0.20
        gradient: Gradient {
            GradientStop { position: 0.0; color: screenInfo.foreground }
            GradientStop { position: 0.5; color: background.color }
            GradientStop { id: shadow; position: 1.0; color: background.color }
        }
        opacity: 0.25

        states: [
            State {
                when: mouse.pressed
                PropertyChanges {
                    target: shadow
                    color: screenInfo.foreground
                }
            }
        ]
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        onClicked: {
            action.trigger()
            root.clicked()
            focus = true
        }
    }
}
