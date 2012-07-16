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
import "../../Qimsys/Gui" as QimsysGui

Rectangle {
    id: root
    width: 500
    height: 600
    color: globalScreenInfo.background

    QimsysGui.ScreenInfo {
        id: globalScreenInfo
        unit: 30
    }

    QimsysCore.ApplicationManager {
        id: applicationManager
    }

    QimsysCore.InputMethodManager {
        id: inputMethodManager
    }

    QimsysCore.PreeditManager {
        id: preeditManager
    }

    QimsysCore.CandidateManager {
        id: candidateManager
    }

    QimsysCore.KeyboardManager {
        id: keyboardManager
    }

    function list2str(list) {
        var ret = '['
        for (var i = 0; i < list.length; i++) {
            if (i != 0) ret += ', '
            ret += list[i]
        }
        return ret + ']'
    }

    function rect2str(rect) {
        return rect.x + ', ' + rect.y + ' ' + rect.width + ' - ' + rect.height
    }

    Flickable {
        anchors.fill: parent
        anchors.margins: globalScreenInfo.unit * 0.1
        contentHeight: container.height
        contentWidth: container.width

        Grid {
            id: container
            columns: 2
            spacing: globalScreenInfo.unit * 0.1

            QimsysGui.Label { action: QimsysGui.Action { text: 'ApplicationManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            QimsysGui.Label { action: QimsysGui.Action { text: 'displayLanguage' } }
            QimsysGui.Label { action: QimsysGui.Action { text: applicationManager.displayLanguage } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'focus' } }
            QimsysGui.Label { action: QimsysGui.Action { text: applicationManager.focus ? 'True' : 'False' } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'window' } }
            QimsysGui.Label { action: QimsysGui.Action { text: applicationManager.window } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'widget' } }
            QimsysGui.Label { action: QimsysGui.Action { text: applicationManager.widget } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'composing' } }
            QimsysGui.Label { action: QimsysGui.Action { text: applicationManager.composing ? 'True' : 'False' } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'exec' } }
            Column {
                QimsysGui.PushButton { action: QimsysGui.Action { text: "Reset"; onTriggered: applicationManager.exec(0) } }
                QimsysGui.PushButton { action: QimsysGui.Action { text: "Dict"; onTriggered: applicationManager.exec(1) } }
                QimsysGui.PushButton { action: QimsysGui.Action { text: "Settings"; onTriggered: applicationManager.exec(2) } }
                QimsysGui.PushButton { action: QimsysGui.Action { text: "About"; onTriggered: applicationManager.exec(3) } }
                QimsysGui.PushButton { action: QimsysGui.Action { text: "Controller"; onTriggered: applicationManager.exec(4) } }
            }

            QimsysGui.Label { action: QimsysGui.Action { text: 'InputMethodManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            QimsysGui.Label { action: QimsysGui.Action { text: 'locale' } }
            QimsysGui.Label { action: QimsysGui.Action { text: inputMethodManager.locale } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'identifier' } }
            QimsysGui.Label { action: QimsysGui.Action { text: inputMethodManager.identifier } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'converter' } }
            QimsysGui.Label { action: QimsysGui.Action { text: inputMethodManager.converter } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'interpreter' } }
            QimsysGui.Label { action: QimsysGui.Action { text: inputMethodManager.interpreter } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'engine' } }
            QimsysGui.Label { action: QimsysGui.Action { text: inputMethodManager.engine } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'state' } }
            QimsysGui.Label { action: QimsysGui.Action { text: inputMethodManager.state } }


            QimsysGui.Label { action: QimsysGui.Action { text: 'PreeditManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            QimsysGui.Label { action: QimsysGui.Action { text: 'item.to' } }
            QimsysGui.Label { action: QimsysGui.Action { text: list2str(preeditManager.item.to) } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'item.from' } }
            QimsysGui.Label { action: QimsysGui.Action { text: list2str(preeditManager.item.from) } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'item.rawString' } }
            QimsysGui.Label { action: QimsysGui.Action { text: list2str(preeditManager.item.rawString) } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'item.cursor' } }
            QimsysGui.Label { action: QimsysGui.Action { text: preeditManager.item.cursor } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'item.selection' } }
            QimsysGui.Label { action: QimsysGui.Action { text: preeditManager.item.selection } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'item.modified' } }
            QimsysGui.Label { action: QimsysGui.Action { text: preeditManager.item.modified } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'rect' } }
            QimsysGui.Label { action: QimsysGui.Action { text: rect2str(preeditManager.rect) } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'font' } }
            QimsysGui.Label { action: QimsysGui.Action { text: 'Font' } font: preeditManager.font }

            QimsysGui.Label { action: QimsysGui.Action { text: 'cursorPosition' } }
            QimsysGui.Label { action: QimsysGui.Action { text: preeditManager.cursorPosition } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'surroundingText' } }
            QimsysGui.Label { action: QimsysGui.Action { text: preeditManager.surroundingText } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'currentSelection' } }
            QimsysGui.Label { action: QimsysGui.Action { text: preeditManager.currentSelection } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'maximumTextLength' } }
            QimsysGui.Label { action: QimsysGui.Action { text: preeditManager.maximumTextLength } }


            QimsysGui.Label { action: QimsysGui.Action { text: 'CandidateManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            QimsysGui.Label { action: QimsysGui.Action { text: 'items' } }
            Column {
                Repeater {
                    model: candidateManager.items
                    delegate: Row {
                        spacing: globalScreenInfo.unit * 0.1
                        QimsysGui.Label { action: QimsysGui.Action { text: model.modelData.index } }
                        QimsysGui.Label { action: QimsysGui.Action { text: model.modelData.from } }
                        QimsysGui.Label { action: QimsysGui.Action { text: model.modelData.to } }
                        QimsysGui.Label { action: QimsysGui.Action { text: model.modelData.hint } }
                        QimsysGui.Label { action: QimsysGui.Action { text: model.modelData.source } }
                    }
                }
                Rectangle{ width: 1; height: 1; color: root.color }
            }

            QimsysGui.Label { action: QimsysGui.Action { text: 'currentIndex' } }
            QimsysGui.Label { action: QimsysGui.Action { text: candidateManager.currentIndex } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'KeyboardManager' } }
            Rectangle{ width: 10; height: 10; color: root.color }

            QimsysGui.Label { action: QimsysGui.Action { text: 'visible' } }
            QimsysGui.CheckBox { action: QimsysGui.Action { text: keyboardManager.visible; checkable: true; checked: keyboardManager.visible; onCheckedChanged: keyboardManager.visible = checked } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'geometry' } }
            QimsysGui.Label { action: QimsysGui.Action { text: rect2str(keyboardManager.geometry) } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'focusArea' } }
            QimsysGui.Label { action: QimsysGui.Action { text: rect2str(keyboardManager.focusArea) } }

            QimsysGui.Label { action: QimsysGui.Action { text: 'sharedData' } }
            QimsysGui.Label { action: QimsysGui.Action { text: keyboardManager.sharedData } }
}
    }
}
