/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright © Matthew Chappee <matthew@orasoft.org>
 *             Paul Flinders <paul@orasoft.org>
 *             Brent Gilmore <brent@orasoft.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/objectman.h"

void
obj_progcancel_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    obj_ginfo.stopflag = 1;
}


GtkWidget*
obj_create_progressbar ()
{
    GtkWidget *fixed1;
  
    obj_progwin.window4 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (obj_progwin.window4), "obj_progwin.window4", obj_progwin.window4);
    gtk_window_set_title (GTK_WINDOW (obj_progwin.window4), "Gathering...Please Wait");
    gtk_window_position (GTK_WINDOW (obj_progwin.window4), GTK_WIN_POS_CENTER);
    gtk_window_set_policy (GTK_WINDOW (obj_progwin.window4), TRUE, TRUE, FALSE);
    gtk_window_set_position (GTK_WINDOW (obj_progwin.window4), GTK_WIN_POS_CENTER);
    gtk_signal_connect(GTK_OBJECT(obj_progwin.window4), "destroy",
		       GTK_SIGNAL_FUNC(gtk_widget_destroyed), &obj_progwin.window4);

    fixed1 = gtk_fixed_new ();
    gtk_object_set_data (GTK_OBJECT (obj_progwin.window4), "fixed1", fixed1);
    gtk_widget_set_usize (fixed1, 367, 121);
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (obj_progwin.window4), fixed1);

    obj_progwin.label1 = gtk_label_new ("Gathering User Info.  This could take a while...");
    gtk_object_set_data (GTK_OBJECT (obj_progwin.window4), "obj_progwin.label1", obj_progwin.label1);
    gtk_widget_show (obj_progwin.label1);
    gtk_fixed_put (GTK_FIXED (fixed1), obj_progwin.label1, 40, 16);
    gtk_widget_set_uposition (obj_progwin.label1, 40, 16);
    gtk_widget_set_usize (obj_progwin.label1, 288, 32);

    obj_progwin.progressbar1 = gtk_progress_bar_new ();
    gtk_object_set_data (GTK_OBJECT (obj_progwin.window4), "obj_progwin.progressbar1", obj_progwin.progressbar1);
    gtk_widget_show (obj_progwin.progressbar1);
    gtk_fixed_put (GTK_FIXED (fixed1), obj_progwin.progressbar1, 32, 56);
    gtk_widget_set_uposition (obj_progwin.progressbar1, 32, 56);
    gtk_widget_set_usize (obj_progwin.progressbar1, 304, 16);

/*  button15 = gtk_button_new_with_label ("Cancel");
    gtk_object_set_data (GTK_OBJECT (obj_progwin.window4), "button15", button15);
    gtk_widget_show (button15);
    gtk_fixed_put (GTK_FIXED (fixed1), button15, 160, 88);
    gtk_widget_set_uposition (button15, 160, 88);
    gtk_widget_set_usize (button15, 47, 22);
    gtk_signal_connect (GTK_OBJECT (button15), "clicked",
    GTK_SIGNAL_FUNC (obj_progcancel_clicked),
    NULL); */

    return obj_progwin.window4;
}

