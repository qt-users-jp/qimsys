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

Image {
    id: stick
    anchors.fill: parent
    source: "qimsys-stick.png"
    fillMode: Image.PreserveAspectFit
    smooth: true

    Image {
        id: wing
        anchors.fill: parent
        source: "qimsys-wing.png"
        fillMode: Image.PreserveAspectFit
        smooth: true

        RotationAnimation on rotation {
            from: 0
            to: 360
            duration: 5000
            loops: Animation.Infinite
        }
    }

    Image {
        id: pin
        anchors.fill: parent
        source: "qimsys-pin.png"
        fillMode: Image.PreserveAspectFit
        smooth: true
    }
}

