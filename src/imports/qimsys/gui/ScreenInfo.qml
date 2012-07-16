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
    width: unit
    height: unit

    property real unit: 50
    property real margins: unit * 0.1

    property real fontSize: unit * 0.50

    property color background: "black"
    property color foreground: "white"
    property color base: "white"
    property color text: "black"
    property color border: "#0066CC"
    property color button: "#606060"
    property color buttonText: "white"
    property color highlight: "#0066CC"
    property color highlightedText: "white"

    StateGroup {
        states: [
            State {
                name: "global"
                when: typeof globalScreenInfo != 'undefined'
                PropertyChanges {
                    target: root
                    unit: globalScreenInfo.unit
                    fontSize: globalScreenInfo.fontSize
                    background: globalScreenInfo.background
                    foreground: globalScreenInfo.foreground

                }
            }
        ]

    }
}
