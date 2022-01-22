/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright ? Matthew Chappee <matthew@orasoft.org>
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "include/objectman.h"
#include "sequencefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void 
obj_create_sequence_toolbar(
    gpointer data)
{
    GtkWidget *button3;
    GtkWidget *tmp_toolbar_icon;

    obj_mainwin.dynamictoolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_widget_ref (obj_mainwin.dynamictoolbar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.dynamictoolbar1", obj_mainwin.dynamictoolbar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_RELIEF_NONE);
    gtk_widget_show (obj_mainwin.dynamictoolbar1);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.toolbar1), obj_mainwin.dynamictoolbar1, NULL, NULL);
    
    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_PRINT);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.");
    button3 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  "Reset",
					  "Reset the sequence", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button3", button3);
    gtk_widget_show (button3);
    gtk_signal_connect (GTK_OBJECT (button3), "clicked",
			GTK_SIGNAL_FUNC (on_seq_reset_clicked),
			data);
}
