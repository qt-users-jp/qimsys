/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010-2016 by Tasuku Suzuki <stasuku@gmail.com>            *
 *   Copyright (C) 2016 by Takahiro Hashimoto <kenya888@gmail.com>           *
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

#include "../qimsysimcontext.h"
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsysinputmethodmanager.h>
#include <qimsyskeymanager.h>
#include <qimsyskeyboardmanager.h>
#include <qimsyspreeditmanager.h>
#include <string.h>
#include "../gtk2qt.h"

#define QIMSYS_IM_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), QIMSYS_IM_CONTEXT_TYPE, QimsysIMContextPrivate))

struct _QimsysIMContextPrivate
{
    GtkIMContext *slave;
    QimsysApplicationManager *application_manager;
    QimsysInputMethodManager *inputmethod_manager;
    QimsysKeyManager *key_manager;
    QimsysKeyboardManager *keyboard_manager;
    QimsysPreeditManager *preedit_manager;

    QimsysPreeditItem *item;
    GdkWindow *client_window;
};

static GType _qimsys_im_context_type = 0;
static GtkIMContextClass *_parent_im_context_class = NULL;
static GtkIMContext *_focus_im_context = NULL;

static void qimsys_im_context_class_init(QimsysIMContextClass *klass);
static void qimsys_im_context_init(QimsysIMContext *self);
static void qimsys_im_context_dispose(GObject *object);
static void qimsys_im_context_reset(GtkIMContext *context);
static gboolean qimsys_im_context_filter_keypress(GtkIMContext *context, GdkEventKey *key);
static void qimsys_im_context_focus_in(GtkIMContext *context);
static void qimsys_im_context_focus_out(GtkIMContext *context);
static void qimsys_im_context_get_preedit_string(GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos);
static void qimsys_im_context_set_client_window(GtkIMContext *context, GdkWindow *client_window);
static void qimsys_im_context_set_cursor_location(GtkIMContext *context, GdkRectangle *area);
static void qimsys_im_context_set_use_preedit(GtkIMContext *context, gboolean use_preedit);
static void qimsys_im_context_set_surrounding(GtkIMContext *context, const gchar *text, gint len, gint cursor_index);
static void qimsys_im_context_get_surrounding(GtkIMContext *context, gchar **text, gint *cursor_index);
static void qimsys_im_context_delete_surrounding(GtkIMContext *context, gint offset, gint n_chars);

static void qimsys_im_context_preedit_item_changed(gpointer data, QimsysPreeditItem *item, QimsysPreeditManager *preedit_manager);
static void qimsys_im_context_preedit_committed(gpointer data, char *text, gulong target, QimsysPreeditManager *preedit_manager);

static void qimsys_im_context_slave_commit(GtkIMContext *slave, char *text, QimsysIMContext *master);
static void qimsys_im_context_slave_preedit_start(GtkIMContext *slave, QimsysIMContext *master);
static void qimsys_im_context_slave_preedit_end(GtkIMContext *slave, QimsysIMContext *master);
static void qimsys_im_context_slave_preedit_changed(GtkIMContext *slave, QimsysIMContext *master);
static void qimsys_im_context_slave_retrieve_surrounding(GtkIMContext *slave, QimsysIMContext *master);
static void qimsys_im_context_slave_delete_surrounding(GtkIMContext *slave, gint offset, gint n_chars, QimsysIMContext *master);

void qimsys_im_context_register_type(GTypeModule *module)
{
    static const GTypeInfo qimsys_im_context_info = {
        sizeof(QimsysIMContextClass)
        , (GBaseInitFunc)NULL
        , (GBaseFinalizeFunc)NULL
        , (GClassInitFunc)qimsys_im_context_class_init
        , (GClassFinalizeFunc)NULL
        , (gconstpointer)NULL
        , sizeof(QimsysIMContext)
        , 0
        , (GInstanceInitFunc)qimsys_im_context_init
        , (GTypeValueTable *)NULL
    };
    qimsys_debug_in();

    if (!qimsys_im_context_get_type()) {
        GType type = g_type_module_register_type(module
                                                 , GTK_TYPE_IM_CONTEXT
                                                 , "QimsysIMContext"
                                                 , &qimsys_im_context_info
                                                 , (GTypeFlags)0
                                                 );
        _qimsys_im_context_type = type;
    }
    qimsys_debug_out();
}

GType qimsys_im_context_get_type()
{
    return _qimsys_im_context_type;
}

QimsysIMContext *qimsys_im_context_new()
{
    QimsysIMContext *ret;
    qimsys_debug_in();
    ret = QIMSYS_IM_CONTEXT(g_object_new(QIMSYS_IM_CONTEXT_TYPE, NULL));
    qimsys_debug_out();
    return ret;
}

static void qimsys_im_context_class_init(QimsysIMContextClass *klass)
{
    GtkIMContextClass *im_context_class;
    GObjectClass *gobject_class;
    qimsys_debug_in();

    _parent_im_context_class = (GtkIMContextClass *)g_type_class_peek_parent(klass);

    im_context_class = GTK_IM_CONTEXT_CLASS(klass);
    im_context_class->reset = qimsys_im_context_reset;
    im_context_class->focus_in = qimsys_im_context_focus_in;
    im_context_class->focus_out = qimsys_im_context_focus_out;
    im_context_class->filter_keypress = qimsys_im_context_filter_keypress;
    im_context_class->get_preedit_string = qimsys_im_context_get_preedit_string;
    im_context_class->set_client_window = qimsys_im_context_set_client_window;
    im_context_class->set_cursor_location = qimsys_im_context_set_cursor_location;
    im_context_class->set_use_preedit = qimsys_im_context_set_use_preedit;
    im_context_class->set_surrounding = qimsys_im_context_set_surrounding;
    im_context_class->get_surrounding = qimsys_im_context_get_surrounding;
    im_context_class->delete_surrounding = qimsys_im_context_delete_surrounding;

    gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = qimsys_im_context_dispose;

    g_type_class_add_private(klass, sizeof(QimsysIMContextPrivate));
    qimsys_debug_out();
}

static void qimsys_im_context_init(QimsysIMContext *self)
{
    qimsys_debug_in();
    self->d = QIMSYS_IM_CONTEXT_GET_PRIVATE(self);
    self->d->slave = gtk_im_context_simple_new();
    g_signal_connect(self->d->slave, "commit", G_CALLBACK(qimsys_im_context_slave_commit), self);
    g_signal_connect(self->d->slave, "preedit-start", G_CALLBACK(qimsys_im_context_slave_preedit_start), self);
    g_signal_connect(self->d->slave, "preedit-end", G_CALLBACK(qimsys_im_context_slave_preedit_end), self);
    g_signal_connect(self->d->slave, "preedit-changed", G_CALLBACK(qimsys_im_context_slave_preedit_changed), self);
    g_signal_connect(self->d->slave, "retrieve-surrounding", G_CALLBACK(qimsys_im_context_slave_retrieve_surrounding), self);
    g_signal_connect(self->d->slave, "delete-surrounding", G_CALLBACK(qimsys_im_context_slave_delete_surrounding), self);

    self->d->application_manager = NULL;
    self->d->inputmethod_manager = NULL;
    self->d->key_manager = NULL;
    self->d->keyboard_manager = NULL;
    self->d->preedit_manager = NULL;
    self->d->item = NULL;
    self->d->client_window = NULL;
    qimsys_debug_out();
}

static void qimsys_im_context_dispose(GObject *object)
{
    QimsysIMContext *self;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(object);

    qimsys_im_context_set_client_window(GTK_IM_CONTEXT(self), NULL);
    if (self->d->item) {
        g_object_unref(self->d->item);
    }

    G_OBJECT_CLASS(_parent_im_context_class)->dispose(object);
    qimsys_debug_out();
}

static void qimsys_im_context_reset(GtkIMContext *context)
{
    QimsysIMContext *self;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);
    if (self->d->application_manager)
        qimsys_application_manager_exec(self->d->application_manager, 0 /* Reset */);
    gtk_im_context_reset(self->d->slave);

    if (self->d->keyboard_manager) {
        qimsys_keyboard_manager_set_visible(self->d->keyboard_manager, TRUE);
    }
    qimsys_debug_out();
}

static gboolean qimsys_im_context_filter_keypress(GtkIMContext *context, GdkEventKey *event)
{
    gboolean ret = FALSE;
    QimsysIMContext *self;
    int modifiers = 0;
    qimsys_debug_in();
    qimsys_debug("\ttype = %d\n", event->type);
    qimsys_debug("\tsend_event = %d\n", event->send_event);
    qimsys_debug("\tstate = %X\n", event->state);
    qimsys_debug("\tkeyval = %X(%X)\n", event->keyval, qimsys_gtk2qt_key_convert(event->keyval));
    qimsys_debug("\tis_modifier = %d\n", event->is_modifier);

    if (event->state & 0x1) modifiers |= 0x02000000;
    if (event->state & 0x4) modifiers |= 0x04000000;
    if (event->state & 0x8) modifiers |= 0x08000000;
    if (event->state & 4000040) modifiers |= 0x10000000;

    self = QIMSYS_IM_CONTEXT(context);

    if (self->d->key_manager) {
        switch (event->type) {
        case GDK_KEY_PRESS:
            qimsys_key_manager_key_press(self->d->key_manager, event->string, qimsys_gtk2qt_key_convert(event->keyval), modifiers, FALSE, &ret);
            break;
        case GDK_KEY_RELEASE:
            qimsys_key_manager_key_release(self->d->key_manager, event->string, qimsys_gtk2qt_key_convert(event->keyval), modifiers, FALSE, &ret);
            break;
        default:
            break;
        }
    }

    if (!ret) {
        ret = gtk_im_context_filter_keypress(self->d->slave, event);
    }
    qimsys_debug_out();
    return ret;
}

static void qimsys_im_context_focus_in(GtkIMContext *context)
{
    QimsysIMContext *self;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);
    if (_focus_im_context != NULL && _focus_im_context != GTK_IM_CONTEXT(self)) {
        gtk_im_context_focus_out(_focus_im_context);
    }
    _focus_im_context = GTK_IM_CONTEXT(self);
    g_object_ref(_focus_im_context);

    if (!self->d->application_manager) {
        self->d->application_manager = qimsys_application_manager_new();
    }

    if (!self->d->inputmethod_manager) {
        self->d->inputmethod_manager = qimsys_inputmethod_manager_new();
    }

    if (!self->d->key_manager) {
        self->d->key_manager = qimsys_key_manager_new();
    }

    if (!self->d->keyboard_manager) {
        self->d->keyboard_manager = qimsys_keyboard_manager_new();
        if (self->d->keyboard_manager) {
            qimsys_keyboard_manager_set_visible(self->d->keyboard_manager, FALSE);
        }
    }

    if (!self->d->preedit_manager) {
        self->d->preedit_manager = qimsys_preedit_manager_new();
        if (self->d->preedit_manager) {
            g_signal_connect_swapped(self->d->preedit_manager, "item-changed", G_CALLBACK(qimsys_im_context_preedit_item_changed), self);
            g_signal_connect_swapped(self->d->preedit_manager, "committed", G_CALLBACK(qimsys_im_context_preedit_committed), self);
        }
    }

    if (self->d->application_manager) {
        if (self->d->client_window) {
            qimsys_application_manager_set_window(self->d->application_manager, gdk_x11_drawable_get_xid(self->d->client_window));
            qimsys_application_manager_set_widget(self->d->application_manager, gdk_x11_drawable_get_xid(self->d->client_window));
        }
        qimsys_application_manager_set_focus(self->d->application_manager, TRUE);
    }
    gtk_im_context_focus_in(self->d->slave);
    qimsys_debug_out();
}

static void qimsys_im_context_focus_out(GtkIMContext *context)
{
    QimsysIMContext *self;
    gulonglong client_window = 0;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);

    if (_focus_im_context == GTK_IM_CONTEXT(self)) {
        g_object_unref(_focus_im_context);
        _focus_im_context = NULL;
    }

    if (self->d->application_manager) {
        qimsys_application_manager_set_focus(self->d->application_manager, FALSE);
        qimsys_application_manager_get_widget(self->d->application_manager, &client_window);
        if (self->d->client_window && gdk_x11_drawable_get_xid(self->d->client_window) == client_window) {
            qimsys_application_manager_set_window(self->d->application_manager, 0);
            qimsys_application_manager_set_widget(self->d->application_manager, 0);
        }
        g_object_unref(self->d->application_manager);
        self->d->application_manager = NULL;
    }

    if (self->d->inputmethod_manager) {
        g_object_unref(self->d->inputmethod_manager);
        self->d->inputmethod_manager = NULL;
    }

    if (self->d->key_manager) {
        g_object_unref(self->d->key_manager);
        self->d->key_manager = NULL;
    }

    if (self->d->keyboard_manager) {
        qimsys_keyboard_manager_set_visible(self->d->keyboard_manager, FALSE);
        g_object_unref(self->d->keyboard_manager);
        self->d->keyboard_manager = NULL;
    }

    if (self->d->preedit_manager) {
        g_object_unref(self->d->preedit_manager);
        self->d->preedit_manager = 0;
    }

    gtk_im_context_focus_out(self->d->slave);
    qimsys_debug_out();
}

static void qimsys_im_context_get_preedit_string(GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos)
{
    gboolean ret;
    guint i = 0;
    QimsysIMContext *self;
    QimsysPreeditItem *item;
    char *preedit_string;
    char **strv_ptr;
    char *buf;
    int cursor;
    int selection = 0;
    int start_index = 0;
    int end_index = 0;
    PangoAttrList *attr_list;
    PangoAttribute *attr;

    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);

    preedit_string = g_strdup("");
    cursor = 0;
    attr_list = pango_attr_list_new();

    if (self->d->application_manager) {
        qimsys_application_manager_get_composing(self->d->application_manager, &ret);
        if (ret) {
            if (self->d->item) {
                item = self->d->item;
                if (item->cursor >= 0) {
                    cursor = g_utf8_strlen(preedit_string, -1) + item->cursor;
                    if (item->selection != 0) {
                        selection = item->selection;
                    }
                }

                qimsys_debug("%s(%d)\n", __FUNCTION__, __LINE__);
                attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
                qimsys_debug("%s(%d)\n", __FUNCTION__, __LINE__);
                attr->start_index = strlen(preedit_string);
                attr->end_index = attr->start_index;
                qimsys_debug("%s(%d)\n", __FUNCTION__, __LINE__);
                for (strv_ptr = item->to; *strv_ptr != NULL; *strv_ptr++) {
                    qimsys_debug("%s(%d) %x\n", __FUNCTION__, __LINE__, (void*)*strv_ptr);
                    qimsys_debug("%s(%d) %s\n", __FUNCTION__, __LINE__, *strv_ptr);
                    attr->end_index += strlen(*strv_ptr);
                }
                qimsys_debug("%s(%d)\n", __FUNCTION__, __LINE__);
                pango_attr_list_insert(attr_list, attr);

                qimsys_debug("%s(%d)\n", __FUNCTION__, __LINE__);
                for (strv_ptr = item->to; *strv_ptr != NULL; *strv_ptr++) {
                    buf = g_strconcat(preedit_string, *strv_ptr, NULL);
                    g_free(preedit_string);
                    preedit_string = buf;
                }
                qimsys_debug("\t%d: %s %d %d %d\n", i, preedit_string, item->cursor, item->selection, item->modified);

                if (selection != 0) {
                    buf = g_strdup(preedit_string);
                    g_utf8_strncpy(buf, preedit_string, cursor);
                    start_index = strlen(buf);
                    qimsys_debug("start_index: %d(%d) - %s\n", start_index, cursor, buf);

                    g_utf8_strncpy(buf, preedit_string, cursor + selection);
                    end_index = strlen(buf);
                    g_free(buf);
                    qimsys_debug("end_index: %d(%d + %d) - %s\n", end_index, cursor, selection, buf);

                    attr = pango_attr_foreground_new(0xffff, 0xffff, 0xffff);
                    attr->start_index = start_index;
                    attr->end_index = end_index;
                    pango_attr_list_insert(attr_list, attr);
                    attr = pango_attr_background_new(0, 0, 0x8fff);
                    attr->start_index = start_index;
                    attr->end_index = end_index;
                    pango_attr_list_insert(attr_list, attr);
                }
            } else {
    //            qimsys_preedit_manager_get_item(self->d->preedit_manager, &item);
            }
            if (str) {
                *str = preedit_string;
            } else {
                g_free(preedit_string);
            }
            if (cursor_pos) {
                *cursor_pos = cursor;
            }
            if (attrs) {
                *attrs = attr_list;
            } else {
                pango_attr_list_unref(attr_list);
            }
        } else {
            gtk_im_context_get_preedit_string(self->d->slave, str, attrs, cursor_pos);
        }
    } else {
        gtk_im_context_get_preedit_string(self->d->slave, str, attrs, cursor_pos);
    }
    qimsys_debug_out();
}

static void qimsys_im_context_set_client_window(GtkIMContext *context, GdkWindow *client_window)
{
    QimsysIMContext *self;
    self = QIMSYS_IM_CONTEXT(context);
    if (self->d->client_window == client_window) return;
    qimsys_debug_in();

    if (self->d->client_window != NULL)
        g_object_unref(self->d->client_window);

    self->d->client_window = client_window;
    if (client_window != NULL) {
        g_object_ref(client_window);
    }

    gtk_im_context_set_client_window(self->d->slave, client_window);

    if (self->d->application_manager) {
        if (client_window) {
            qimsys_application_manager_set_window(self->d->application_manager, gdk_x11_drawable_get_xid(client_window));
            qimsys_application_manager_set_widget(self->d->application_manager, gdk_x11_drawable_get_xid(client_window));
        } else {
            qimsys_application_manager_set_window(self->d->application_manager, 0);
            qimsys_application_manager_set_widget(self->d->application_manager, 0);
        }
    }
    qimsys_debug_out();
}

static void qimsys_im_context_set_cursor_location(GtkIMContext *context, GdkRectangle *area)
{
    QimsysIMContext *self;
    int x = 0;
    int y = 0;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);
    if (self->d->client_window) {
        gdk_window_get_origin (self->d->client_window, &x, &y);
    }
    if (self->d->preedit_manager) {
        qimsys_preedit_manager_set_rect(self->d->preedit_manager, x + area->x, y + area->y, area->width, area->height);
    }
    gtk_im_context_set_cursor_location(self->d->slave, area);
    qimsys_debug_out();
}

static void qimsys_im_context_set_use_preedit(GtkIMContext *context, gboolean use_preedit)
{
    QimsysIMContext *self;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);

    gtk_im_context_set_use_preedit(self->d->slave, use_preedit);
    qimsys_debug_out();
}

static void qimsys_im_context_set_surrounding(GtkIMContext *context, const gchar *text, gint len, gint cursor_index)
{
    QimsysIMContext *self;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);
    qimsys_debug("text: %s, len: %d, cursor_index, %d\n", text, len, cursor_index);

    qimsys_preedit_manager_set_surrounding_text(self->d->preedit_manager, text);
    qimsys_preedit_manager_set_cursor_position(self->d->preedit_manager, cursor_index);
//    gtk_im_context_set_surrounding(self->d->slave, text, len, cursor_index);
    qimsys_debug_out();
}

static void qimsys_im_context_get_surrounding(GtkIMContext *context, gchar **text, gint *cursor_index)
{
    QimsysIMContext *self;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);

    qimsys_preedit_manager_get_cursor_position(self->d->preedit_manager, cursor_index);
    qimsys_preedit_manager_get_surrounding_text(self->d->preedit_manager, text);
//    gtk_im_context_get_surrounding(self->d->slave, text, cursor_index);
    qimsys_debug_out();
}

static void qimsys_im_context_delete_surrounding(GtkIMContext *context, gint offset, gint n_chars)
{
    QimsysIMContext *self;
    qimsys_debug_in();
    self = QIMSYS_IM_CONTEXT(context);
    qimsys_debug("offset: %d, n_chars: %d\n", offset, n_chars);

//    qimsys_preedit_manager_set_surrounding_text(self->d->preedit, text);
    gtk_im_context_delete_surrounding(self->d->slave, offset, n_chars);
    qimsys_debug_out();
}

static void qimsys_im_context_preedit_item_changed(gpointer data, QimsysPreeditItem *item, QimsysPreeditManager *preedit_manager)
{
    QimsysIMContext *context = QIMSYS_IM_CONTEXT(data);
    qimsys_debug_in();
    if (_focus_im_context == GTK_IM_CONTEXT(context)) {
        if (context->d->item) {
            // has preedit
            g_object_unref(context->d->item);
            if (g_strv_length(item->to) > 0) {
                context->d->item = item;
                g_object_ref(context->d->item);
                g_signal_emit_by_name(context, "preedit-changed");
            } else {
                context->d->item = NULL;
                g_signal_emit_by_name(context, "preedit-changed");
                g_signal_emit_by_name(context, "preedit-end");
            }
        } else {
            if (g_strv_length(item->to) > 0) {
                context->d->item = item;
                g_object_ref(context->d->item);
                g_signal_emit_by_name(context, "preedit-start");
                g_signal_emit_by_name(context, "preedit-changed");
            } else {
                //
            }
        }
    }
    qimsys_debug_out();
}

static void qimsys_im_context_preedit_committed(gpointer data, char *text, gulong target, QimsysPreeditManager *preedit_manager)
{
    QimsysIMContext *context = QIMSYS_IM_CONTEXT(data);
    qimsys_debug_in();
    if (context->d->client_window && target == gdk_x11_drawable_get_xid(context->d->client_window)) {    // \todo
        g_signal_emit_by_name(context, "commit", text);
    }
    qimsys_debug_out();
}

static void qimsys_im_context_slave_commit(GtkIMContext *slave, char *text, QimsysIMContext *master)
{
    g_signal_emit_by_name(master, "commit", text);
}

static void qimsys_im_context_slave_preedit_start(GtkIMContext *slave, QimsysIMContext *master)
{
    g_signal_emit_by_name(master, "preedit-start");
}

static void qimsys_im_context_slave_preedit_end(GtkIMContext *slave, QimsysIMContext *master)
{
    g_signal_emit_by_name(master, "preedit-end");
}

static void qimsys_im_context_slave_preedit_changed(GtkIMContext *slave, QimsysIMContext *master)
{
    g_signal_emit_by_name(master, "preedit-changed");
}

static void qimsys_im_context_slave_retrieve_surrounding(GtkIMContext *slave, QimsysIMContext *master)
{
    g_signal_emit_by_name(master, "retrieve-surrounding");
}

static void qimsys_im_context_slave_delete_surrounding(GtkIMContext *slave, gint offset, gint n_chars, QimsysIMContext *master)
{
    g_signal_emit_by_name(master, "delete-surrounding", offset, n_chars);
}
