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

    TabWidget {
        anchors.fill: parent

        Rectangle {
            id: aboutQimsys
            anchors.fill: parent
            color: "black"
            property variant action: Action { text: "About"; icon: Qt.resolvedUrl("../../Qimsys/Gui/qimsys.png") }

            Text {
                anchors.fill: parent
                anchors.margins: 10
                color: "white"
                property string versionString: (typeof version == 'undefined') ? "unknown" : version
                text: '<p><strong>qimsys version ' + versionString + '</strong></p><p>An input method for linux</p><p><a href="http://code.google.com/p/qimsys/" style="color: cyan">http://code.google.com/p/qimsys/</a></p><p>Copyright&copy;2009-2011 <a href="mailto:stasuku@gmail.com" style="color: cyan">Tasuku Suzuki</a></p>'
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }

        Rectangle {
            id: author
            anchors.fill: parent
            color: "black"
//            property url icon: Qt.resolvedUrl("../../Qimsys/Gui/qimsys.png")
            property variant action: Action { text: "Author" }

            ListView {
                id: authors

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: bugreport.top
                anchors.margins: 10
                clip: true

                model: ListModel {
                    ListElement {
                        author_image: "http://gravatar.com/avatar/bb2ff1535f4bcf9d8e6bd546f56cb1df"
                        author_name: "Tasuku Suzuki"
                        author_link: '<a href="http://twitter.com/#!/tasukusuzuki" style="color: cyan">@tasukusuzuki</a>'
                        author_role: "Author/Maintainer"
                    }
                }

                delegate: Item {
                    width: authors.width
                    height: 120
                    clip: true
                    Image {
                        id: icon
                        source: author_image
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 5
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: name
                        anchors.top: parent.top
                        anchors.left: icon.right
                        anchors.margins: 5

                        text: author_name
                        color: "white"
                    }

                    Text {
                        id: link
                        anchors.left: name.left
                        anchors.leftMargin: 20
                        anchors.top: name.bottom
                        anchors.topMargin: 5

                        text: author_link
                        color: "white"
                    }

                    Text {
                        id: role
                        anchors.left: link.left
                        anchors.top: link.bottom
                        anchors.topMargin: 5

                        text: author_role
                        color: "white"
                    }
                }
            }
            Text {
                id: bugreport
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
                anchors.left: parent.left
                anchors.right: parent.right
                color: "white"
                text: '<p>Please use <a href="http://bit.ly/fLeDng" style="color: cyan">http://bit.ly/fLeDng</a> to report bugs.</p>'
                onLinkActivated: Qt.openUrlExternally(link)
            }

        }

        Rectangle {
            id: credit
            anchors.fill: parent
            color: "black"
            property variant action: Action { text: "Credits" }

            ListView {
                id: credits
                anchors.fill: parent
                anchors.margins: 10
                clip: true

                model: ListModel {
                    /*
                    ListElement {
                        credit_image: "a"
                        credit_name: "a"
                        credit_link: "a"
                        credit_role: "a"
                    }
                    */
                }

                delegate: Item {
                    width: credit.width
                    height: 120
                    clip: true
                    Image {
                        id: icon
                        source: credit_image
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 5
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: name
                        anchors.top: parent.top
                        anchors.left: icon.right
                        anchors.margins: 5

                        text: credit_name
                        color: "white"
                    }

                    Text {
                        id: link
                        anchors.left: name.left
                        anchors.leftMargin: 20
                        anchors.top: name.bottom
                        anchors.topMargin: 5

                        text: credit_link
                        color: "white"
                    }

                    Text {
                        id: role
                        anchors.left: link.left
                        anchors.top: link.bottom
                        anchors.topMargin: 5

                        text: credit_role
                        color: "white"
                    }
                }
            }
        }

        Flickable {
            id: license
            anchors.fill: parent
            property variant action: Action { text: "License" }
            contentHeight: lgpl_text.height
            contentWidth: lgpl_text.width
            clip: true

            TextEdit {
                id: lgpl_text
                color: "white"
                readOnly: true
                text: (typeof lgpl == 'undefined') ? "LGPL 2.1" : lgpl
            }
        }
    }
}
