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

Item {
    id: root
    width: 20
    height: 20

    property real sizeHint: root.width

    property bool checkable: false
    property bool checked: false

    signal toggled(bool on)
    signal pressed()
    signal released()
    signal clicked(bool autoRepeat)

    Rectangle {
        id: background
        anchors {
            fill: parent
            margins: sizeHint / 50.0
        }
        radius: sizeHint / 10.0
        color: "gray"
        border.width: radius / 2.0
        border.color: "black"
        smooth: true

        Rectangle {
            id: grad
            anchors.fill: parent
            radius: background.radius
            smooth: true
            gradient: Gradient {
                GradientStop { id: stop1; position: 0.0; color: background.color }
                GradientStop { id: stop2; position: 1.0; color: "black" }
            }
            opacity: 0.5
        }

        Timer {
            id: timer
            interval: 100
            repeat: true

            onTriggered: background.click(true)
        }

        function click(autoRepeat) {
            if (root.checkable) {
                root.checked = !root.checked
                root.toggled(root.checked)
            } else {
                root.clicked(autoRepeat)
            }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            onPressed: {
                root.pressed()
            }
            onReleased: {
                if (timer.running)
                    timer.stop()
                root.released()
            }
            onClicked: background.click(false)

            onPressAndHold: {
                if (!root.checkable)
                    timer.start()
            }
        }

        states: [
            State {
                when: (!root.checkable && mouse.pressed) || root.checked
                PropertyChanges {
                    target: stop1
                    color: "black"
                }
                PropertyChanges {
                    target: stop2
                    color: background.color
                }
            }
        ]
    }
}
