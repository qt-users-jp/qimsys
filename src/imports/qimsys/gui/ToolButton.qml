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

Button {
    id: root
    className: 'ToolButton'

    width: screenInfo.width * 10
    height: landscape ? screenInfo.height : screenInfo.height * 2

    property bool landscape: false
    property bool portrait: !landscape

    action: Action { text: "Tool Button"; icon: Qt.resolvedUrl("qimsys.png") }

    Image {
        id: icon
        width: height
        source: action.icon
        visible: status != Image.Null
        fillMode: Image.PreserveAspectFit
        smooth: true
        anchors.bottomMargin: portrait ? text.height : 0
        anchors.margins: 2
    }

    Text {
        id: text
        color: screenInfo.foreground
        text: action.text
        font.pixelSize: screenInfo.fontSize
        anchors.margins: 2
    }

    states: [
        State {
            when: portrait
            AnchorChanges {
                target: icon
                anchors.top: root.top
                anchors.bottom: root.bottom
                anchors.horizontalCenter: root.horizontalCenter
            }

            AnchorChanges {
                target: text
                anchors.left: root.left
                anchors.right: root.right
                anchors.bottom: root.bottom
            }

            PropertyChanges {
                target: text
                horizontalAlignment: Text.AlignHCenter
            }

        },
        State {
            when: landscape
            AnchorChanges {
                target: icon
                anchors.top: root.top
                anchors.left: root.left
                anchors.bottom: root.bottom
            }

            AnchorChanges {
                target: text
                anchors.verticalCenter: icon.verticalCenter
                anchors.left: icon.right
                anchors.right: root.right
            }

            PropertyChanges {
                target: text
                horizontalAlignment: Text.AlignLeft
            }
        }
    ]

}
