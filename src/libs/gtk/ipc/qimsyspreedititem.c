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

#include "qimsyspreedititem.h"
#include "qimsysdebug.h"
#include "dbus.h"

static void qimsys_preedit_item_finalize(GObject *gobject);

G_DEFINE_TYPE(QimsysPreeditItem, qimsys_preedit_item, G_TYPE_OBJECT)

static void qimsys_preedit_item_class_init(QimsysPreeditItemClass *klass)
{
    G_OBJECT_CLASS(klass)->finalize = qimsys_preedit_item_finalize;
}

static void qimsys_preedit_item_init(QimsysPreeditItem *item)
{
    qimsys_debug_in();
    qimsys_debug_out();
}

QimsysPreeditItem *qimsys_preedit_item_new(GArray *data)
{
    char **strv_ptr;
    GValue *value;
    QimsysPreeditItem *ret;

    qimsys_debug_in();

    ret = QIMSYSPREEDITITEM(g_object_new(QIMSYSPREEDITITEM_TYPE, NULL));

    value = &g_array_index(data, GValue, 0);
    ret->to = g_value_dup_boxed(value);

    for (strv_ptr = ret->to; *strv_ptr; *strv_ptr++) {
        qimsys_debug("%s(%d) %s\n", __FUNCTION__, __LINE__, *strv_ptr);
    }

    value = &g_array_index(data, GValue, 1);
    ret->from = g_value_dup_boxed(value);

    value = &g_array_index(data, GValue, 2);
    ret->raw_string = g_value_dup_boxed(value);

    value = &g_array_index(data, GValue, 3);
    ret->cursor = g_value_get_int(value);

    value = &g_array_index(data, GValue, 4);
    ret->selection = g_value_get_int(value);

    value = &g_array_index(data, GValue, 5);
    ret->modified = g_value_get_int(value);

    qimsys_debug_out();
    return ret;
}

GArray *qimsys_preedit_item_get_value(QimsysPreeditItem *item)
{
    GValue value = G_VALUE_INIT;
    GArray *ret;

    qimsys_debug_in();

    ret = g_array_sized_new(FALSE, TRUE, sizeof(GValue), 0);
    g_array_set_clear_func(ret, (GDestroyNotify) g_value_unset);

    g_value_init(&value, G_TYPE_STRV);
    g_value_set_boxed(&value, item->to);
    g_array_append_val(ret, value);
    g_value_unset(&value);

    g_value_set_boxed(&value, item->from);
    g_array_append_val(ret, value);
    g_value_unset(&value);

    g_value_set_boxed(&value, item->raw_string);
    g_array_append_val(ret, value);
    g_value_unset(&value);

    g_value_init(&value, G_TYPE_INT);
    g_value_set_int(&value, item->cursor);
    g_array_append_val(ret, value);

    g_value_set_int(&value, item->selection);
    g_array_append_val(ret, value);

    g_value_set_int(&value, item->modified);
    g_array_append_val(ret, value);

    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_item_finalize(GObject *gobject)
{
    qimsys_debug_in();
    QimsysPreeditItem *item = QIMSYSPREEDITITEM(gobject);
    g_strfreev(item->to);
    g_strfreev(item->from);
    g_strfreev(item->raw_string);
    qimsys_debug_out();
}
