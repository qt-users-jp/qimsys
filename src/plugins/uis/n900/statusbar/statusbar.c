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

#include "statusbar.h"

#include <hildon/hildon.h>

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>
#include <qimsyskeyboardmanager.h>

#define N900STATUSBAR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, \
                            N900STATUSBAR_TYPE, N900StatusBarPrivate))

struct _N900StatusBarPrivate
{
    QimsysApplicationManager *qam;
    QimsysKeyboardManager *qkm;
    GtkWidget *composing;
    GtkWidget *keyboard;
};

HD_DEFINE_PLUGIN_MODULE (N900StatusBar, n900_status_bar, HD_TYPE_STATUS_MENU_ITEM);

static void n900_status_bar_class_finalize (N900StatusBarClass *klass);

static void n900_status_bar_current_icon_changed(gpointer button, GdkPixbuf *value, QimsysApplicationManager *qam);
static void n900_status_bar_composing_changed(gpointer button, gboolean value, QimsysApplicationManager *qam);
static void n900_status_bar_keyboard_changed(gpointer button, gboolean value, QimsysKeyboardManager *qkm);

static void n900_status_bar_open(gpointer instance, gpointer button);
static void n900_status_bar_toggle_composing(gpointer instance, gpointer button);
static void n900_status_bar_toggle_keyboard(gpointer instance, gpointer button);

static void n900_status_bar_class_init(N900StatusBarClass *klass)
{
//    qimsys_debug_on();
    qimsys_debug_in();

    g_type_class_add_private (klass, sizeof (N900StatusBarPrivate));

    qimsys_debug_out();
}


static void n900_status_bar_init(N900StatusBar *n900_status_bar)
{
    gboolean value;
    GtkWidget *button;
    GtkWidget * icon;
    GdkPixbuf *pixbuf = NULL;
    QimsysApplicationManager *qam;
    QimsysKeyboardManager *qkm;
    qimsys_debug_in();

    n900_status_bar->d = N900STATUSBAR_GET_PRIVATE (n900_status_bar);

    qam = qimsys_application_manager_new();
    n900_status_bar->d->qam = qam;

    qkm = qimsys_keyboard_manager_new();
    n900_status_bar->d->qkm = qkm;

    {
        GtkWidget *hbox = gtk_hbox_new(FALSE, 0);

        { // main button
            button = hildon_button_new (HILDON_SIZE_FINGER_HEIGHT, HILDON_BUTTON_ARRANGEMENT_VERTICAL);

            pixbuf = gdk_pixbuf_new_from_file_at_size ("/usr/share/pixmaps/qimsys/qimsys.png", 48, 48, NULL);
            icon = gtk_image_new_from_pixbuf(pixbuf);
            g_object_unref (pixbuf);

            hildon_button_set_style (HILDON_BUTTON(button), HILDON_BUTTON_STYLE_PICKER);
            hildon_button_set_image (HILDON_BUTTON(button), icon);
            hildon_button_set_title (HILDON_BUTTON(button), "qimsys");
//            hildon_button_set_value (HILDON_BUTTON(button), "qimsys");
            hildon_button_set_alignment (HILDON_BUTTON(button), 0.0, 0.0, 1.0, 1.0);
            g_signal_connect_swapped (G_OBJECT (button), "clicked", G_CALLBACK (n900_status_bar_open), qam);

            gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 1);
        }

        { // composing button
            button = gtk_toggle_button_new();
            if (qimsys_application_manager_get_current_icon(qam, &pixbuf)) {
                g_object_ref(pixbuf);
                n900_status_bar_current_icon_changed(n900_status_bar, pixbuf, qam);
            } else {
                pixbuf = gdk_pixbuf_new_from_file_at_size ("/usr/share/pixmaps/qimsys/qimsys.png", 48, 48, NULL);
            }
            icon = gtk_image_new_from_pixbuf(pixbuf);
            g_object_unref (pixbuf);
            gtk_button_set_image(GTK_BUTTON(button), GTK_WIDGET(icon));

            if (qimsys_application_manager_get_composing(qam, &value)) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), value);
            }
            g_signal_connect_swapped (G_OBJECT (button), "toggled", G_CALLBACK (n900_status_bar_toggle_composing), qam);

            gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 1);

            n900_status_bar->d->composing = button;

            g_signal_connect_swapped(qam, "current-icon-changed", G_CALLBACK(n900_status_bar_current_icon_changed), n900_status_bar);
            g_signal_connect_swapped(qam, "composing-changed", G_CALLBACK(n900_status_bar_composing_changed), button);
        }

        { // keyboard button
            button = gtk_toggle_button_new();
            pixbuf = gdk_pixbuf_new_from_file_at_size ("/usr/share/pixmaps/qimsys/qimsys_keyboard_off.png", 48, 48, NULL);
            icon = gtk_image_new_from_pixbuf(pixbuf);
            g_object_unref (pixbuf);
            gtk_button_set_image(GTK_BUTTON(button), GTK_WIDGET(icon));


            g_signal_connect_swapped (G_OBJECT (button), "toggled", G_CALLBACK (n900_status_bar_toggle_keyboard), qkm);

            gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 1);

            n900_status_bar->d->keyboard = button;

            g_signal_connect_swapped(qkm, "keyboard-changed", G_CALLBACK(n900_status_bar_keyboard_changed), button);
        }

        gtk_widget_show_all (hbox);

        gtk_container_add(GTK_CONTAINER(n900_status_bar), hbox);
    }
    gtk_widget_show (GTK_WIDGET (n900_status_bar));

    qimsys_debug_out();
}

static void n900_status_bar_class_finalize (N900StatusBarClass *klass)
{

}

static void n900_status_bar_current_icon_changed(gpointer n900_status_bar, GdkPixbuf *value, QimsysApplicationManager *qam)
{
    GtkWidget *image;
    GdkPixbuf *pixbuf;

    qimsys_debug_in();
    image = gtk_button_get_image(GTK_BUTTON(N900STATUSBAR(n900_status_bar)->d->composing));
    if (value) {
        pixbuf = gdk_pixbuf_scale_simple(value, 48, 48, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf (GTK_IMAGE(image), pixbuf);
        g_object_unref (pixbuf);

        pixbuf = gdk_pixbuf_scale_simple(value, 22, 22, GDK_INTERP_BILINEAR);
        hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (n900_status_bar), pixbuf);
        g_object_unref (pixbuf);

        g_object_unref (value);
    } else {
        pixbuf = gdk_pixbuf_new_from_file_at_size ("/usr/share/pixmaps/qimsys/qimsys.png", 48, 48, NULL);
        gtk_image_set_from_pixbuf (GTK_IMAGE(image), pixbuf);
        g_object_unref (pixbuf);

        hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (n900_status_bar), NULL);
    }
    qimsys_debug_out();
}

static void n900_status_bar_composing_changed(gpointer button, gboolean value, QimsysApplicationManager *qam)
{
    GtkToggleButton *toggle_button = NULL;

    qimsys_debug_in();
    toggle_button = GTK_TOGGLE_BUTTON(button);
    gtk_toggle_button_set_active(toggle_button, value);
    qimsys_debug_out();
}

static void n900_status_bar_keyboard_changed(gpointer button, gboolean value, QimsysKeyboardManager *qkm)
{
    GtkToggleButton *toggle_button = NULL;

    qimsys_debug_in();
    qimsys_debug("keyboard = %s", value ? "true" : "false");
    toggle_button = GTK_TOGGLE_BUTTON(button);
    gtk_toggle_button_set_active(toggle_button, value);
    qimsys_debug_out();
}

static void n900_status_bar_open (gpointer instance, gpointer button)
{
    QimsysApplicationManager *qam;

    qimsys_debug_in();
    qam = QIMSYSAPPLICATIONMANAGER(instance);
    if (qam) {
        qimsys_application_manager_exec(qam, QimsysApplicationManager_ShowController);
    }
    qimsys_debug_out();
}

static void n900_status_bar_toggle_composing(gpointer instance, gpointer button)
{
    gboolean value = FALSE;
    QimsysApplicationManager *qam;
    GtkToggleButton *toggle_button;

    qimsys_debug_in();
    toggle_button = GTK_TOGGLE_BUTTON(button);
    if (toggle_button) {
        value = gtk_toggle_button_get_active(toggle_button);
    }

    qam = QIMSYSAPPLICATIONMANAGER(instance);
    if (qam) {
        qimsys_application_manager_set_composing(qam, value);
    }
    qimsys_debug_out();
}

static void n900_status_bar_toggle_keyboard(gpointer instance, gpointer button)
{
    gboolean value = FALSE;
    QimsysKeyboardManager *qkm;
    GtkToggleButton *toggle_button;

    qimsys_debug_in();
    toggle_button = GTK_TOGGLE_BUTTON(button);
    if (toggle_button) {
        value = gtk_toggle_button_get_active(toggle_button);
    } else {
        qimsys_debug("%s\n", "toggle_button is not GtkToggleButton.");
    }

    qkm = QIMSYSKEYBOARDMANAGER(instance);
    if (qkm) {
        qimsys_keyboard_manager_set_visible(qkm, value);
    } else {
        qimsys_debug("%s\n", "instance is not QimsysKeyboardManager");
    }
    qimsys_debug_out();
}

