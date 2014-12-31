/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
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

#ifndef NAMESPACE_H
#define NAMESPACE_H

namespace ja_JP
{
enum State {
    Direct = 0x01,
    EmptyString = 0x02,
    InputingString = 0x04,
    ConvertingString = 0x08,
    SelectingCandidate = 0x10,
    SelectingPrediction = 0x20,
};

enum {
    RawString = 0x01,
    Alphabet = 0x02,
    Hiragana = 0x04,
    Katakana = 0x08,
    CharacterMask = 0x0f,
    HalfWidth = 0x10,
    FullWidth = 0x20,
    WidthMask = 0xf0,
};

enum Typing {
    Roma = 0x100,
    Kana = 0x200,
    KyuriEx = 0x400,
    TypingMask = 0xff00,
};
}

#endif // NAMESPACE_H
