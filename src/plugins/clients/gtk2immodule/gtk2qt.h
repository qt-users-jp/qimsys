/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2009-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
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

#ifndef GTK2QT_H
#define GTK2QT_H

#include <gdk/gdkkeysyms.h>

static const int qimsys_gtk2qt_key_tbl[] = {
#include "gtk2qtkey.tbl"
    0,                          0
};

inline int qimsys_gtk2qt_key_convert(int key)
{
    int i;
    int ret = key;

    for (i = 0; qimsys_gtk2qt_key_tbl[i]; i += 2) {
        if (qimsys_gtk2qt_key_tbl[i] == key) {
            ret = qimsys_gtk2qt_key_tbl[i+1];
            break;
        }
    }
    return ret;
}

#endif // GTK2QT_H
