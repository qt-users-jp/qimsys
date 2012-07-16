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
    width: 640
    height: 480

    default property alias widgets: stackedWidget.children

    property int currentIndex: -1

    Component.onCompleted: {
        resized()
        currentIndex = 0
    }

    function resized() {
        for (var i = 0; i < widgets.length; i++) {
            widgets[i].width = root.width
            widgets[i].height = stackedWidget.height
        }
    }

    Row {
        id: stackedWidget
        anchors.top: parent.top
        anchors.bottom: tabBar.top
        x: -root.width * Math.max(0, root.currentIndex)
        Behavior on x {
            NumberAnimation {
                duration: 500
                easing.type: Easing.OutExpo
            }
        }
    }

    property alias leftAction: leftButton.action
    property alias centerAction: centerButton.action
    property alias rightAction: rightButton.action

    Row {
        id: tabBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        ToolButton {
            id: leftButton
            width: parent.width / 3
            landscape: root.width > root.height
        }

        ToolButton {
            id: centerButton
            width: parent.width / 3
            landscape: root.width > root.height
        }

        ToolButton {
            id: rightButton
            width: parent.width / 3
            landscape: root.width > root.height
        }
    }
    onWidthChanged: resized()
    onHeightChanged: resized()
}
