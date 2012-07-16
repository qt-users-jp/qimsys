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

ListModel {
    ListElement { caption: 'tab'; vScale: 0.5; code: 0x01000001 }    // tab
    ListElement { caption: '<'; vScale: 0.5; commit: true }
    ListElement { caption: '>'; vScale: 0.5; commit: true }
    ListElement { caption: '['; vScale: 0.5; commit: true }
    ListElement { caption: ']'; vScale: 0.5; commit: true }
    ListElement { caption: '{'; vScale: 0.5; commit: true }
    ListElement { caption: '}'; vScale: 0.5; commit: true }
    ListElement { caption: '|'; vScale: 0.5; commit: true }
    ListElement { caption: '¥'; vScale: 0.5; commit: true }
    ListElement { caption: '^'; vScale: 0.5; commit: true }
    ListElement { caption: '~'; vScale: 0.5; commit: true }
    ListElement { caption: '`'; vScale: 0.5; commit: true }
    ListElement { caption: '\u00d7'; vScale: 0.5; code: 0x010000ce }  // close

    ListElement { caption: 'Ctrl'; fn: 'Sym'; code: 0x01000021 }
    ListElement { caption: 'Q'; fn: '1' }
    ListElement { caption: 'W'; fn: '2' }
    ListElement { caption: 'E'; fn: '3' }
    ListElement { caption: 'R'; fn: '4' }
    ListElement { caption: 'T'; fn: '5' }
    ListElement { caption: 'Y'; fn: '6' }
    ListElement { caption: 'U'; fn: '7' }
    ListElement { caption: 'I'; fn: '8' }
    ListElement { caption: 'O'; fn: '9' }
    ListElement { caption: 'P'; fn: '0' }
    ListElement { caption: ','; fn: '='; shift: ';' }
    ListElement { caption: '\u2190'; code: 0xff08 } // backspace

    ListElement { caption: 'fn' }    // fn
    ListElement { caption: 'A'; fn: '*' }
    ListElement { caption: 'S'; fn: '+' }
    ListElement { caption: 'D'; fn: '#' }
    ListElement { caption: 'F'; fn: '-' }
    ListElement { caption: 'G'; fn: '_' }
    ListElement { caption: 'H'; fn: '(' }
    ListElement { caption: 'J'; fn: ')' }
    ListElement { caption: 'K'; fn: '&' }
    ListElement { caption: 'L'; fn: '!' }
    ListElement { caption: '.'; fn: '?'; shift: ':' }
    ListElement { caption: '\u25b2'; code: 0x01000013 } // up
    ListElement { caption: '\u2190'; code: 0x01000004 }

    ListElement { caption: '\u2302'; code: 0x01000020 } // shift
    ListElement { caption: 'Z'; fn: '£' }
    ListElement { caption: 'X'; fn: '$' }
    ListElement { caption: 'C'; fn: '\u20ac' }
    ListElement { caption: 'V'; fn: '/' }
    ListElement { caption: 'B'; fn: '\u005c' }
    ListElement { caption: 'N'; fn: '"' }
    ListElement { caption: 'M'; fn: "'" }
    ListElement { caption: 'space'; fn: '@'; code: 0x20; hScale: 2.0 } // space
    ListElement { caption: '\u25b2'; code: 0x01000012 } // left
    ListElement { caption: '\u25bc'; code: 0x01000015 } // down
    ListElement { caption: '\u25b2'; code: 0x01000014 } // right
}
