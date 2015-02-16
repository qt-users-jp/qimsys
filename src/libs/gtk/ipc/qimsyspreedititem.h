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

#ifndef QIMSYSPREEDITITEM_H
#define QIMSYSPREEDITITEM_H

#include <glib.h>
#include <glib-object.h>
#include "qimsysglobal.h"

#ifdef __cplusplus
extern "C" {
#endif

G_BEGIN_DECLS

#define QIMSYSPREEDITITEM_TYPE            (qimsys_preedit_item_get_type())
#define QIMSYSPREEDITITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), QIMSYSPREEDITITEM_TYPE, QimsysPreeditItem))
#define QIMSYSPREEDITITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), QIMSYSPREEDITITEM_TYPE, QimsysPreeditItemClass))
#define IS_QIMSYSPREEDITITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), QIMSYSPREEDITITEM_TYPE))
#define IS_QIMSYSPREEDITITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), QIMSYSPREEDITITEM_TYPE))
#define QIMSYSPREEDITITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), QIMSYSPREEDITITEM, QimsysPreeditItemClass))

#define QIMSYS_STRUCT_PREEDITITEM (dbus_g_type_get_struct ("GArray", G_TYPE_STRV, G_TYPE_STRV, G_TYPE_STRV, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID))

typedef struct _QimsysPreeditItem       QimsysPreeditItem;
typedef struct _QimsysPreeditItemClass  QimsysPreeditItemClass;

struct _QimsysPreeditItem
{
    GObject instance_parent;

    GStrv to;
    GStrv from;
    GStrv raw_string;
    int cursor;
    int selection;
    int modified;
};

struct _QimsysPreeditItemClass
{
    GObjectClass class_parent;

    void (* qimsys_preedit_item) (QimsysPreeditItem *item);
};

QIMSYS_EXPORT GType qimsys_preedit_item_get_type();
QIMSYS_EXPORT QimsysPreeditItem *qimsys_preedit_item_new(GArray *data);
QIMSYS_EXPORT GArray *qimsys_preedit_item_get_value(QimsysPreeditItem *item);

G_END_DECLS

#ifdef __cplusplus
}
#endif

#endif // QIMSYSPREEDITITEM_H
