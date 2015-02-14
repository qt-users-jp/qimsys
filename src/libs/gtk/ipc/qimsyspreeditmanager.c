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

#include "qimsyspreeditmanager.h"
#include "qimsysdebug.h"
#include "dbus.h"
#include "qimsysmarshalers.h"

enum {
    QIMSYSPREEDITMANAGER_ITEM_CHANGED,
    QIMSYSPREEDITMANAGER_RECT_CHANGED,
#ifdef QIMSYSPREEDITMANAGER_FONT_SUPPORT
    QIMSYSPREEDITMANAGER_FONT_CHANGED,
#endif
    QIMSYSPREEDITMANAGER_CURRENT_POSIION_CHANGED,
    QIMSYSPREEDITMANAGER_SURROUNDING_TEXT_CHANGED,
    QIMSYSPREEDITMANAGER_CURRENT_SELECTION_CHANGED,
    QIMSYSPREEDITMANAGER_MAXIMUM_TEXT_LENGTH_CHANGED,
    QIMSYSPREEDITMANAGER_COMMITTED,
    QIMSYSPREEDITMANAGER_LAST_SIGNAL
};

static guint qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_LAST_SIGNAL] = { 0, 0, 0 };

G_DEFINE_TYPE(QimsysPreeditManager, qimsys_preedit_manager, QIMSYSABSTRACTIPCOBJECT_TYPE)

static void qimsys_preedit_manager_item_changed(DBusGProxy *proxy, GValueArray *value, gpointer user_data);
static void qimsys_preedit_manager_rect_changed(DBusGProxy *proxy, GValueArray *value, gpointer user_data);
#ifdef QIMSYSPREEDITMANAGER_FONT_SUPPORT
static void qimsys_preedit_manager_font_changed(DBusGProxy *proxy, char *value, gpointer user_data);
#endif
static void qimsys_preedit_manager_cursor_position_changed(DBusGProxy *proxy, int cursor_position, gpointer user_data);
static void qimsys_preedit_manager_surrounding_text_changed(DBusGProxy *proxy, char *surrounding_text, gpointer user_data);
static void qimsys_preedit_manager_current_selection_changed(DBusGProxy *proxy, char *current_selection, gpointer user_data);
static void qimsys_preedit_manager_maximum_text_length_changed(DBusGProxy *proxy, int maximum_text_length, gpointer user_data);
static void qimsys_preedit_manager_committed(DBusGProxy *proxy, char *value, gulonglong target, gpointer user_data);

#define QIMSYS_STRUCT_RECT (dbus_g_type_get_struct ("GValueArray", G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INVALID))
#ifdef QIMSYSPREEDITMANAGER_FONT_SUPPORT
#define G_STRUCT_STRING (dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_INVALID))
#endif

static void qimsys_preedit_manager_class_init(QimsysPreeditManagerClass *klass)
{
    qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_ITEM_CHANGED]
            = g_signal_new("item-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysPreeditManagerClass, qimsys_preedit_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__OBJECT,
                           G_TYPE_NONE, 1, G_TYPE_OBJECT);
    qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_RECT_CHANGED]
            = g_signal_new("rect-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysPreeditManagerClass, qimsys_preedit_manager),
                           NULL, NULL,
                           qimsys_marshal_VOID__INT_INT_INT_INT,
                           G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
#ifdef QIMSYSPREEDITMANAGER_FONT_SUPPORT
    qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_FONT_CHANGED]
            = g_signal_new("font_changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysPreeditManagerClass, qimsys_preedit_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__STRING,
                           G_TYPE_NONE, 1, G_TYPE_STRING);
#endif
    qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_CURRENT_POSIION_CHANGED]
            = g_signal_new("current-position-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysPreeditManagerClass, qimsys_preedit_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__INT,
                           G_TYPE_NONE, 1, G_TYPE_INT);
    qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_SURROUNDING_TEXT_CHANGED]
            = g_signal_new("surrounding-text-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysPreeditManagerClass, qimsys_preedit_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__CHAR,
                           G_TYPE_NONE, 1, G_TYPE_CHAR);
    qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_CURRENT_SELECTION_CHANGED]
            = g_signal_new("current-selection-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysPreeditManagerClass, qimsys_preedit_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__CHAR,
                           G_TYPE_NONE, 1, G_TYPE_CHAR);
    qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_MAXIMUM_TEXT_LENGTH_CHANGED]
            = g_signal_new("maximum-text-length-changed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysPreeditManagerClass, qimsys_preedit_manager),
                           NULL, NULL,
                           g_cclosure_marshal_VOID__INT,
                           G_TYPE_NONE, 1, G_TYPE_INT);
    qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_COMMITTED]
            = g_signal_new("committed",
                           G_TYPE_FROM_CLASS(klass),
                           G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                           G_STRUCT_OFFSET(QimsysPreeditManagerClass, qimsys_preedit_manager),
                           NULL, NULL,
                           qimsys_marshal_VOID__STRING_ULONGLONG,
                           G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_ULONGLONG);
}

static void qimsys_preedit_manager_init(QimsysPreeditManager *qpm)
{
    QimsysAbstractIpcObject *qaio;
    qimsys_debug_in();

    qaio = QIMSYSABSTRACTIPCOBJECT(qpm);

    /* Create a proxy object */
    qimsys_abstract_ipc_object_connect(qaio, QIMSYS_DBUS_SERVICE, "/QimsysPreeditManager", "local.PreeditManager");

    dbus_g_proxy_add_signal(qaio->proxy, "itemChanged", QIMSYS_STRUCT_PREEDITITEM, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "itemChanged", G_CALLBACK(qimsys_preedit_manager_item_changed), qpm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "rectChanged", QIMSYS_STRUCT_RECT, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "rectChanged", G_CALLBACK(qimsys_preedit_manager_rect_changed), qpm, NULL);

#ifdef QIMSYSPREEDITMANAGER_FONT_SUPPORT
    dbus_g_proxy_add_signal(qaio->proxy, "fontChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "fontChanged", G_CALLBACK(qimsys_preedit_manager_font_changed), qpm, NULL);
#endif

    dbus_g_proxy_add_signal(qaio->proxy, "currentPositionChanged", G_TYPE_INT, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "currentPositionChanged", G_CALLBACK(qimsys_preedit_manager_cursor_position_changed), qpm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "surroundingTextChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "surroundingTextChanged", G_CALLBACK(qimsys_preedit_manager_surrounding_text_changed), qpm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "currentSelectionChanged", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "currentSelectionChanged", G_CALLBACK(qimsys_preedit_manager_current_selection_changed), qpm, NULL);

    dbus_g_proxy_add_signal(qaio->proxy, "maximumTextLengthChanged", G_TYPE_INT, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "maximumTextLengthChanged", G_CALLBACK(qimsys_preedit_manager_maximum_text_length_changed), qpm, NULL);

    dbus_g_object_register_marshaller(qimsys_marshal_VOID__STRING_ULONGLONG, G_TYPE_NONE, G_TYPE_STRING, G_TYPE_ULONGLONG, G_TYPE_INVALID);
    dbus_g_proxy_add_signal(qaio->proxy, "committed", G_TYPE_STRING, G_TYPE_ULONGLONG, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(qaio->proxy, "committed", G_CALLBACK(qimsys_preedit_manager_committed), qpm, NULL);

    qimsys_debug_out();
}

QimsysPreeditManager *qimsys_preedit_manager_new()
{
    QimsysPreeditManager *ret = QIMSYSPREEDITMANAGER(g_object_new(QIMSYSPREEDITMANAGER_TYPE, NULL));
    return ret;
}

gboolean qimsys_preedit_manager_get_item(QimsysPreeditManager *qpm, QimsysPreeditItem **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray* item;

    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "item", &error, G_TYPE_INVALID, QIMSYS_STRUCT_PREEDITITEM, &item, G_TYPE_INVALID))
        *value = qimsys_preedit_item_new(item);
    }

    qimsys_debug_out();
    return ret;
}

gboolean qimsys_preedit_manager_set_item(QimsysPreeditManager *qpm, QimsysPreeditItem *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray *item;

    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        item = qimsys_preedit_item_get_value(value);
        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "setItem", &error, QIMSYS_STRUCT_PREEDITITEM, item, G_TYPE_INVALID, G_TYPE_INVALID))
        g_value_array_free(item);
    }

    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_manager_item_changed(DBusGProxy *proxy, GValueArray *value, gpointer user_data)
{
    QimsysPreeditItem *item;
    qimsys_debug_in();

    item = qimsys_preedit_item_new(value);
    g_signal_emit(QIMSYSPREEDITMANAGER(user_data), qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_ITEM_CHANGED], 0, item);
    g_object_unref(item);

    qimsys_debug_out();
}

gboolean qimsys_preedit_manager_get_rect(QimsysPreeditManager *qpm, int *x, int *y, int *w, int *h)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray *values;
    GValue *value;

    qimsys_debug_in();
    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(4);

        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "rect", &error, G_TYPE_INVALID, QIMSYS_STRUCT_RECT, &values, G_TYPE_INVALID))

        value = g_value_array_get_nth(values, 0);
        *x = g_value_get_int(value);
        g_value_unset(value);
        value = g_value_array_get_nth(values, 1);
        *y = g_value_get_int(value);
        g_value_unset(value);
        value = g_value_array_get_nth(values, 2);
        *w = g_value_get_int(value);
        g_value_unset(value);
        value = g_value_array_get_nth(values, 3);
        *h = g_value_get_int(value);
        g_value_unset(value);

        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

gboolean qimsys_preedit_manager_set_rect(QimsysPreeditManager *qpm, int x, int y, int w, int h)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValue value = G_VALUE_INIT;
    GValueArray *values;

    qimsys_debug_in();
    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        g_value_init(&value, G_TYPE_INT);
        g_value_set_int(&value, x);
        g_value_array_append(values, &value);
        g_value_unset(&value);

        g_value_init(&value, G_TYPE_INT);
        g_value_set_int(&value, y);
        g_value_array_append(values, &value);
        g_value_unset(&value);

        g_value_init(&value, G_TYPE_INT);
        g_value_set_int(&value, w);
        g_value_array_append(values, &value);
        g_value_unset(&value);

        g_value_init(&value, G_TYPE_INT);
        g_value_set_int(&value, h);
        g_value_array_append(values, &value);
        g_value_unset(&value);

        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "setRect", &error, QIMSYS_STRUCT_RECT, values, G_TYPE_INVALID, G_TYPE_INVALID))

        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_manager_rect_changed(DBusGProxy *proxy, GValueArray *values, gpointer user_data)
{
    gint x, y, w, h;
    GValue *value;
    qimsys_debug_in();
    value = g_value_array_get_nth(values, 0);
    x = g_value_get_int(value);
    g_value_unset(value);
    value = g_value_array_get_nth(values, 1);
    y = g_value_get_int(value);
    g_value_unset(value);
    value = g_value_array_get_nth(values, 2);
    w = g_value_get_int(value);
    g_value_unset(value);
    value = g_value_array_get_nth(values, 3);
    h = g_value_get_int(value);
    g_value_unset(value);
    g_signal_emit(QIMSYSPREEDITMANAGER(user_data), qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_RECT_CHANGED], 0, x, y, w, h);
    qimsys_debug_out();
}

#ifdef QIMSYSPREEDITMANAGER_FONT_SUPPORT
gboolean qimsys_preedit_manager_get_font(QimsysPreeditManager *qpm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray *values;
    GValue *v;

    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "font", &error, G_TYPE_INVALID, G_STRUCT_STRING, values, G_TYPE_INVALID))

        v = g_value_array_get_nth(values, 0);
        *value = g_value_get_string(v);
        g_value_unset(v);
        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

gboolean qimsys_preedit_manager_set_font(QimsysPreeditManager *qpm, char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValue v = G_VALUE_INIT;
    GValueArray *values;

    qimsys_debug_in();
    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        g_value_init(&v, G_TYPE_STRING);
        g_value_set_string(&v, value);
        g_value_array_append(values, &v);
        g_value_unset(&v);

        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "setFont", &error, G_STRUCT_STRING, values, G_TYPE_INVALID, G_TYPE_INVALID))

        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_manager_font_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
//    g_signal_emit(QIMSYSPREEDITMANAGER(user_data), qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_FONT_CHANGED], 0, value);
}
#endif

gboolean qimsys_preedit_manager_get_cursor_position(QimsysPreeditManager *qpm, int *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray *values;
    GValue *v;

    qimsys_debug_in();
    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "currentPosition", &error, G_TYPE_INVALID, G_TYPE_INT, values, G_TYPE_INVALID))

        v = g_value_array_get_nth(values, 0);
        *value = g_value_get_int(v);
        g_value_unset(v);
        g_value_array_free(values);
    }

    qimsys_debug_out();
    return ret;
}

gboolean qimsys_preedit_manager_set_cursor_position(QimsysPreeditManager *qpm, int value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValue v = G_VALUE_INIT;
    GValueArray *values;

    qimsys_debug_in();
    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        g_value_init(&v, G_TYPE_INT);
        g_value_set_int(&v, value);
        g_value_array_append(values, &v);
        g_value_unset(&v);

        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "setCurrentPosition", &error, G_TYPE_INT, values, G_TYPE_INVALID, G_TYPE_INVALID))

        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_manager_cursor_position_changed(DBusGProxy *proxy, int value, gpointer user_data)
{
    g_signal_emit(QIMSYSPREEDITMANAGER(user_data), qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_CURRENT_POSIION_CHANGED], 0, value);
}

gboolean qimsys_preedit_manager_get_surrounding_text(QimsysPreeditManager *qpm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray *values;
    GValue *v;

    qimsys_debug_in();
    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "surroundingText", &error, G_TYPE_INVALID, G_TYPE_STRING, values, G_TYPE_INVALID))

        v = g_value_array_get_nth(values, 0);
        *value = g_strdup(g_value_get_string(v));
        g_value_unset(v);
        g_value_array_free(values);
    }

    qimsys_debug_out();
    return ret;
}

gboolean qimsys_preedit_manager_set_surrounding_text(QimsysPreeditManager *qpm, const char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValue v = G_VALUE_INIT;
    GValueArray *values;
    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        g_value_init(&v, G_TYPE_STRING);
        g_value_set_string(&v, value);
        g_value_array_append(values, &v);
        g_value_unset(&v);

        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "setSurroundingText", &error, G_TYPE_STRING, values, G_TYPE_INVALID, G_TYPE_INVALID))
        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_manager_surrounding_text_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(QIMSYSPREEDITMANAGER(user_data), qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_SURROUNDING_TEXT_CHANGED], 0, value);
}

gboolean qimsys_preedit_manager_get_current_selection(QimsysPreeditManager *qpm, char **value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray *values;
    GValue *v;

    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "currentSelection", &error, G_TYPE_INVALID, G_TYPE_STRING, values, G_TYPE_INVALID))

        v = g_value_array_get_nth(values, 0);
        *value = g_strdup(g_value_get_string(v));
        g_value_unset(v);
        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

gboolean qimsys_preedit_manager_set_current_selection(QimsysPreeditManager *qpm, const char *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValue v = G_VALUE_INIT;
    GValueArray *values;
    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        g_value_init(&v, G_TYPE_STRING);
        g_value_set_string(&v, value);
        g_value_array_append(values, &v);
        g_value_unset(&v);

        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "setCurrentSelection", &error, G_TYPE_STRING, values, G_TYPE_INVALID, G_TYPE_INVALID))
        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_manager_current_selection_changed(DBusGProxy *proxy, char *value, gpointer user_data)
{
    g_signal_emit(QIMSYSPREEDITMANAGER(user_data), qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_CURRENT_SELECTION_CHANGED], 0, value);
}

gboolean qimsys_preedit_manager_get_maximum_text_length(QimsysPreeditManager *qpm, int *value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValueArray *values;
    GValue *v;

    qimsys_debug_in();
    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        CHECK_DBUS_ERROR(ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "maximumTextLength", &error, G_TYPE_INVALID, G_TYPE_INT, values, G_TYPE_INVALID))

        v = g_value_array_get_nth(values, 0);
        *value = g_value_get_int(v);
        g_value_unset(v);
        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

gboolean qimsys_preedit_manager_set_maximum_text_length(QimsysPreeditManager *qpm, int value)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    GValue v = G_VALUE_INIT;
    GValueArray *values;
    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm))) {
        values = g_value_array_new(0);

        g_value_init(&v, G_TYPE_INT);
        g_value_set_int(&v, value);
        g_value_array_append(values, &v);
        g_value_unset(&v);

        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "setMaximumTextLength", &error, G_TYPE_INT, values, G_TYPE_INVALID, G_TYPE_INVALID))
        g_value_array_free(values);
    }
    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_manager_maximum_text_length_changed(DBusGProxy *proxy, int value, gpointer user_data)
{
    g_signal_emit(QIMSYSPREEDITMANAGER(user_data), qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_MAXIMUM_TEXT_LENGTH_CHANGED], 0, value);
}

gboolean qimsys_preedit_manager_commit(QimsysPreeditManager *qpm, gulonglong target)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "commit", &error, G_TYPE_ULONGLONG, target, G_TYPE_INVALID, G_TYPE_INVALID))

    qimsys_debug_out();
    return ret;
}

gboolean qimsys_preedit_manager_commit_string(QimsysPreeditManager *qpm, char *string, gulonglong target)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    qimsys_debug_in();

    if (qimsys_abstract_ipc_object_is_connected(QIMSYSABSTRACTIPCOBJECT(qpm)))
        CHECK_DBUS_ERROR (ret, !dbus_g_proxy_call(QIMSYSABSTRACTIPCOBJECT(qpm)->proxy, "commit", &error, G_TYPE_STRING, string, G_TYPE_ULONGLONG, target, G_TYPE_INVALID, G_TYPE_INVALID))

    qimsys_debug_out();
    return ret;
}

static void qimsys_preedit_manager_committed(DBusGProxy *proxy, char *value, gulonglong target, gpointer user_data)
{
//    qimsys_debug_in();
    g_signal_emit(QIMSYSPREEDITMANAGER(user_data), qimsys_preedit_manager_signals[QIMSYSPREEDITMANAGER_COMMITTED], 0, value, target);
//    qimsys_debug_out();
}
