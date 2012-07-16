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

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <errno.h>

#include <qimsysdebug.h>
#include <qimsysapplicationmanager.h>

static QimsysApplicationManager *application_manager;
static GtkWidget *window;
static GtkWidget *icon;
static GdkPixbuf *pixbuf = NULL;

void destroy()
{
  gtk_main_quit ();
}

static void focus_changed(gpointer data, gboolean value, QimsysApplicationManager *manager)
{
    GtkWidget *widget = GTK_WIDGET(data);

    g_print("focus_changed\n");
}

static void current_icon_changed(gpointer data, GdkPixbuf *value, QimsysApplicationManager *manager)
{
    qimsys_debug_in();
    g_print("current_icon_changed\n");
#if 0
    if (qimsys_application_manager_get_current_icon(manager, &pixbuf) && pixbuf) {
        gtk_image_set_from_pixbuf (GTK_IMAGE (icon), pixbuf);
    }
#else
    gtk_image_set_from_pixbuf (GTK_IMAGE (icon), value);
#endif
    qimsys_debug_out();
}

int main(int argc, char *argv[])
{
    GError *error = NULL;
    qimsys_debug_on();

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_signal_connect (GTK_OBJECT (window), "destroy",
                        GTK_SIGNAL_FUNC (destroy), NULL);
    gtk_container_border_width (GTK_CONTAINER (window), 10);

    application_manager = qimsys_application_manager_new();
    g_signal_connect_swapped(application_manager, "focus-changed", G_CALLBACK(focus_changed), window);
    g_signal_connect_swapped(application_manager, "current-icon-changed", G_CALLBACK(current_icon_changed), window);

    if (!qimsys_application_manager_get_current_icon(application_manager, &pixbuf)) {
        if (error) {
            GtkWidget *dialog;

            dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_CLOSE,
                                             "Unable to open image file 'gtk-logo-rgb.gif': %s",
                                             error->message);
            g_error_free (error);

            g_signal_connect (dialog, "response",
                              G_CALLBACK (gtk_widget_destroy), NULL);

            gtk_widget_show (dialog);
        }
    }
    icon = gtk_image_new_from_pixbuf(pixbuf);

    gtk_container_add (GTK_CONTAINER (window), icon);
    gtk_widget_show (icon);

    gtk_widget_show (window);

    gtk_main ();

    return 0;
}
