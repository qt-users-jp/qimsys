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
import Qimsys.Core 1.0 as QimsysCore
import Qimsys.N900 1.0 as N900

Rectangle {
    id: root
    width: 600
    height: 200
    color: "black"

    property real uWidth: root.width / 13.0
    property real uHeight: root.height * 2.0 / 7.0
    property int modifiers: 0

    QimsysCore.ApplicationManager {
        id: applicationManager
    }

    QimsysCore.KeyManager {
        id: keyManager
    }

    QimsysCore.PreeditManager {
        id: preeditManager
    }

    QimsysCore.KeyboardManager {
        id: keyboardManager
        onVisibleChanged: {}
    }

    N900.DeviceInfo {
        id: deviceInfo
        property bool landscape: deviceInfo.screenSize.width > deviceInfo.screenSize.height
        property bool portrait: !landscape
    }

    StateGroup {
        id: keyboardGeometry
        states: [
            State {
                name: "landscape"
                when: deviceInfo.landscape
                PropertyChanges {
                    target: self
                    geometry.x: 0
                    geometry.y: deviceInfo.desktopRect.y + deviceInfo.desktopRect.height / 2
                    geometry.width: deviceInfo.desktopRect.width + 1
                    geometry.height: deviceInfo.desktopRect.height / 2
                }
            },
            State {
                name: "portrait"
                when: deviceInfo.portrait
                PropertyChanges {
                    target: self
                    geometry.x: 0
                    geometry.y: deviceInfo.desktopRect.y + deviceInfo.desktopRect.height * 3 / 4
                    geometry.width: deviceInfo.desktopRect.width + 1
                    geometry.height: deviceInfo.desktopRect.height / 4
                }
            }
        ]
    }

    StateGroup {
        id: keyboardVisible
        states: [
            State {
                name: "visible"
                when: keyboardManager.visible && deviceInfo.slideClosed
                PropertyChanges {
                    target: self
                    visible: true
                }
            },
            State {
                name: "invisible"
                when: !keyboardManager.visible || !deviceInfo.slideClosed
                PropertyChanges {
                    target: self
                    visible: false
                }
            }
        ]
    }

    Flow {
        anchors.fill: parent
        Repeater {
            model: KeyboardLayout{}

            delegate: Button {
                id: button
                width: root.uWidth * (typeof hScale == 'undefined' ? 1.0 : hScale)
                height: root.uHeight * (typeof vScale == 'undefined' ? 1.0 : vScale)
                sizeHint: Math.min(uWidth, uHeight)

                property int modifier: 0

                Item {
                    id: container
                    anchors.fill: parent
                    anchors.margins: button.sizeHint * 0.15
                    Text { id: label
                        anchors.fill: parent
                        text: typeof caption == 'undefined' ? '' : caption
                        font.pixelSize: button.sizeHint * 0.40
                        color: "white"
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignBottom

                        StateGroup { // centering caption
                            states: [
                                State {
                                    when: (typeof caption == 'undefined' || typeof fn == 'undefined')
                                    PropertyChanges {
                                        target: label
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        font.pixelSize: 0.35
                                    }
                                }
                            ]
                        }
                    }
                }

                Text { id: fnLabel
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: button.sizeHint * 0.15
                    text: typeof fn == 'undefined' ? '' : fn
                    font.pixelSize: button.sizeHint * 0.35
                    color: "skyblue"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    visible: text != ''
                    smooth: true
                }

                Text { id: shiftLabel
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.margins: button.sizeHint * 0.15
                    text: typeof shift == 'undefined' ? '' : shift
                    font.pixelSize: button.sizeHint * 0.35
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    visible: text != ''
                }

                StateGroup { // set up special buttons
                    states: [
                        State { // control
                            when: typeof code != 'undefined' && code == '0x01000021'
                            PropertyChanges {
                                target: button
                                checkable: !(root.modifiers & 0x08000000)
                                modifier: 0x04000000
                            }
                            PropertyChanges {
                                target: label
                                font.pixelSize: button.sizeHint * 0.35
                            }
                            PropertyChanges {
                                target: fnLabel
                                font.pixelSize: button.sizeHint * 0.30
                            }
                        },
                        State { // fn
                            when: typeof caption != 'undefined' && caption == 'fn'
                            PropertyChanges {
                                target: button
                                checkable: true
                                modifier: 0x08000000
                            }
                            PropertyChanges {
                                target: label
                                color: 'skyblue'
                                text: '\u2190'
                                font.pixelSize: button.sizeHint * 0.75
                                smooth: true
                            }
                            PropertyChanges {
                                target: container
                                rotation: 135

                            }
                        },
                        State { // shift
                            when: typeof code != 'undefined' && code == 0x01000020
                            PropertyChanges {
                                target: button
                                checkable: true
                                modifier: 0x02000000
                            }
                            PropertyChanges {
                                target: label
                                font.pixelSize: button.sizeHint * 0.90
                            }
                        },
                        State { // backspace
                            when: typeof code != 'undefined' && code == 0xff08
                            PropertyChanges {
                                target: label
                                font.pixelSize: button.sizeHint * 0.50
                            }
                        },
                        State { // return
                            when: typeof code != 'undefined' && code == 0x01000004
                            PropertyChanges {
                                target: label
                                font.pixelSize: button.sizeHint * 0.75
                            }
                            PropertyChanges {
                                target: container
                                rotation: -45
                            }
                        },
                        State { // left
                            when: typeof code != 'undefined' && code == 0x01000012
                            PropertyChanges {
                                target: label
                                font.pixelSize: button.sizeHint * 0.75
                            }
                            PropertyChanges {
                                target: container
                                rotation: -90
                            }
                        },
                        State { // up
                            when: typeof code != 'undefined' && code == 0x01000013
                            PropertyChanges {
                                target: label
                                font.pixelSize: button.sizeHint * 0.75
                            }
                        },
                        State { // right
                            when: typeof code != 'undefined' && code == 0x01000014
                            PropertyChanges {
                                target: label
                                font.pixelSize: button.sizeHint * 0.75
                            }
                            PropertyChanges {
                                target: container
                                rotation: 90
                            }
                        },
                        State { // down
                            when: typeof code != 'undefined' && code == 0x01000015
                            PropertyChanges {
                                target: label
                                font.pixelSize: button.sizeHint * 0.75
                            }
                        }
                    ]
                }

                onCheckableChanged: {
                    // reset checked
                    if (!checkable) {
                        checked = false
                    }
                }

                onToggled: {
                    if (on) {
                        root.modifiers |= modifier
                    } else {
                        root.modifiers = root.modifiers & ~modifier
                    }
                }

                property string text: (typeof code == 'undefined' ? (typeof caption == 'undefined' ? '' : caption): String.fromCharCode(code))
                property string fnText: (typeof code == 'undefined' ? (typeof fn == 'undefined' ? '' : fn): String.fromCharCode(code))
                property int keycode: (typeof code == 'undefined' ? (typeof caption == 'undefined' ? 0 : caption.charCodeAt(0)) : code)

                onClicked: {
                    var c = (typeof code == 'undefined' ? 0 : code)
//                    console.debug(c, text, keycode, modifiers, autoRepeat)
                    switch (c) {
                    case 0: // input
                        if (applicationManager.composing && !(modifiers & 0x04000000)) {
                            if (modifiers & 0x08000000 && typeof fn != 'undefined') {
                                preeditManager.insert(fn)
                            } else if (modifiers & 0x02000000) {
                                if (typeof shift == 'undefined') {
                                    preeditManager.insert(text)
                                } else {
                                    preeditManager.insert(shift)
                                }
                            } else {
                                preeditManager.insert(text.toLowerCase())
                            }
                        } else {
                            if (typeof commit == 'undefined') {
                                if (modifiers & 0x08000000 && typeof fn != 'undefined') {
                                    preeditManager.commit(fn, applicationManager.widget);
                                } else {
                                    keyManager.process(text, keycode, modifiers, true, autoRepeat)
                                    keyManager.process(text, keycode, modifiers, false, autoRepeat)
                                }
                            } else {
                                preeditManager.commit(text, applicationManager.widget);
                            }
                        }
                        break;
                    case 0x010000ce: { // close
                        keyboardManager.visible = false
                        break }
                    default: {
                        keyManager.process(text, keycode, modifiers, true, autoRepeat)
                        keyManager.process(text, keycode, modifiers, false, autoRepeat)
                        break;
                    }
                    }

                    if (keycode < 0x01000012 || 0x01000015 < keycode ) {
                        root.modifiers = 0
                    }
                }

                Connections {
                    target: root
                    onModifiersChanged: {
                        if (button.modifier == 0) return
                        button.checked = (modifiers & button.modifier)
                    }
                }
            }
        }
    }
}
