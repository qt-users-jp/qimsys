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
    default property alias tabs: stackedWidget.children

    property int currentIndex: -1

    function setCurrentIndex(index) {
        if (currentIndex != -1) {
            stackedWidget.children[currentIndex].visible = false
        }
        currentIndex = index
        if (currentIndex != -1) {
            stackedWidget.children[currentIndex].visible = true
        }
    }

    Component.onCompleted: {
        for (var i = 0; i < stackedWidget.children.length; i++) {
            stackedWidget.children[i].visible = false
        }

        setCurrentIndex(0)
    }

    Item {
        id: stackedWidget
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: tabBar.top
        clip: true
    }

    Row {
        id: tabBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            model: stackedWidget.children.length
            delegate: ToolButton {
                width: root.width / stackedWidget.children.length
                action: stackedWidget.children[index].action
                landscape: root.width > root.height

                onClicked: {
                    setCurrentIndex(index)
                }
            }
        }
    }
}
