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

void clicked(gpointer data)
{
    QimsysApplicationManager *manager;
    qimsys_debug_in();
    manager = QIMSYSAPPLICATIONMANAGER(data);
    if (manager) {
        qimsys_application_manager_exec(manager, QimsysApplicationManager_ShowSettings);
    }
    qimsys_debug_out();
}

void destroy()
{
  gtk_main_quit ();
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    QimsysApplicationManager *manager;

    qimsys_debug_on();

    gtk_init (&argc, &argv);

    manager = qimsys_application_manager_new();

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_signal_connect (GTK_OBJECT (window), "destroy",
                        GTK_SIGNAL_FUNC (destroy), NULL);
    gtk_container_border_width (GTK_CONTAINER (window), 10);

    button = gtk_button_new_with_label ("Click");

    g_signal_connect_swapped (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (clicked), manager);

    gtk_container_add (GTK_CONTAINER (window), button);

    gtk_widget_show (button);

    gtk_widget_show (window);

    gtk_main ();

    return 0;
}
