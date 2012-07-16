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

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <libhildondesktop/libhildondesktop.h>

G_BEGIN_DECLS

#define N900STATUSBAR_TYPE            (n900_status_bar_get_type())
#define N900STATUSBAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), N900STATUSBAR_TYPE, N900StatusBar))
#define N900STATUSBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), N900STATUSBAR_TYPE, N900StatusBarClass))
#define IS_N900STATUSBAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), N900STATUSBAR_TYPE))
#define IS_N900STATUSBAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), N900STATUSBAR_TYPE))
#define N900STATUSBAR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), N900STATUSBAR, N900StatusBarClass))

typedef struct _N900StatusBar       N900StatusBar;
typedef struct _N900StatusBarClass  N900StatusBarClass;
typedef struct _N900StatusBarPrivate  N900StatusBarPrivate;

struct _N900StatusBar
{
    HDStatusMenuItem parent;

    N900StatusBarPrivate *d;
};

struct _N900StatusBarClass
{
    HDStatusMenuItemClass parent;
};

GType n900_status_bar_get_type();

G_END_DECLS

#endif//STATUSBAR_H
