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

    property variant action: Action { text: "Line Edit" }

    ScreenInfo {
        id: screenInfo
    }

    width: screenInfo.width * 10
    height: screenInfo.height

    Rectangle {
        anchors.fill: parent
        anchors.rightMargin: 1
        anchors.bottomMargin: 1
        radius: screenInfo.unit * 0.2
        border.color: screenInfo.border
        border.width: 1
        color: screenInfo.base

        Item {
            anchors.fill: parent
            anchors.margins: screenInfo.unit * 0.15
            clip: true
            TextInput {
                anchors.fill: parent
                font.pixelSize: screenInfo.fontSize
                color: screenInfo.text

                text: action.text
                onTextChanged: action.text = text
            }
        }
    }
}
