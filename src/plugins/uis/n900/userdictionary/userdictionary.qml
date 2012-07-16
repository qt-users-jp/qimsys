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
import "../../Qimsys/Gui"

Rectangle {
    id: root
    width: 400
    height: 240
    color: "black"

    property variant editor

    ScreenInfo {
        id: globalScreenInfo
        unit: 50
        foreground: "white"
        background: "black"
    }

    Action {
        id: noAction
    }

    Action {
        id: closeAction
        text: 'Close'
        onTriggered: {
            Qt.quit()
        }
    }

    Action {
        id: updateAction
        text: 'Update'

        onTriggered: {
            switch (stackedWidget.currentIndex) {
            case 1: {
                    editor.save()
                    stackedWidget.currentIndex--
                    list.currentIndex = list.currentIndex
                    break
                }
            }
        }
    }

    Action {
        id: addAction
        text: 'Add'

        onTriggered: {
            switch (stackedWidget.currentIndex) {
            case 0: {
                    stackedWidget.currentIndex++
                    editor.load(-1)
                    stackedWidget.leftAction = addAction
                    stackedWidget.centerAction = noAction
                    stackedWidget.rightAction = cancelAction
                    break
                }
            case 1: {
                    editor.save()
                    stackedWidget.currentIndex--
                    list.currentIndex = 0
                    break
                }
            }
        }
    }

    Action {
        id: cancelAction
        text: 'Cancel'

        onTriggered: {
            switch (stackedWidget.currentIndex) {
            case 1: {
                    stackedWidget.currentIndex--
                    break
                }
            }
        }
    }

    Action {
        id: deleteAction
        text: 'Delete'

        onTriggered: {
            switch (stackedWidget.currentIndex) {
            case 1: {
                    editor.del()
                    stackedWidget.currentIndex--
                    break
                }
            }
        }
    }

    StackedWidget {
        id: stackedWidget
        anchors.fill: parent

        Item {
            Delegate {
                id: header
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: globalScreenInfo.unit
                color: "gray"
                first: "Sound"
                second: "Spelling"
            }

            ListView {
                id: list
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: header.bottom
                anchors.bottom: parent.bottom
                model: userDictionaryModel
                clip: true

                delegate: Delegate {
                    width: list.width
                    height: globalScreenInfo.unit

                    first: sound
                    second: spelling

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            editor.load(index)
                            stackedWidget.currentIndex++
                            stackedWidget.leftAction = updateAction
                            stackedWidget.centerAction = deleteAction
                            stackedWidget.rightAction = cancelAction
                        }
                    }
                }
            }
        }

        Item {
            id: container

            Component.onCompleted: {
                var component = Qt.createComponent(userDictionaryEditor)
                if (component.status == Component.Ready) {
                    editor = component.createObject(container)
                }
            }
        }

        Rectangle {
            color: "green"
        }

        states: [
            State {
                name: 'List'
                when: stackedWidget.currentIndex == 0
                PropertyChanges {
                    target: stackedWidget
                    leftAction: addAction
                    centerAction: noAction
                    rightAction: closeAction
                }
            },
            State {
                name: 'Edit'
                when: stackedWidget.currentIndex == 1
                PropertyChanges {
                    target: stackedWidget
                }
            }
        ]
    }
}
