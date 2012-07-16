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
    className: 'PushButton'

    action: Action { text: "Push Button" }

    width: screenInfo.width * 10
    height: screenInfo.height

    property alias horizontalAlignment: label.horizontalAlignment
    property alias verticalAlignment: label.verticalAlignment

    Label {
        id: label
        anchors.fill: parent
        anchors.leftMargin: screenInfo.unit * (isPressed ? 0.2 : 0.15)
        anchors.topMargin: screenInfo.unit * (isPressed ? 0.05 : 0.00)
        anchors.rightMargin: screenInfo.unit * 0.15
        anchors.bottomMargin: screenInfo.unit * 0.00
        action: root.action
    }
}
