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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "include/objectman.h"
#include "objects/dblinks/dblinkfuncs.h"

void
obj_dodblinkview(GtkWidget *clist1)
{
    GtkWidget *scrolledwindow1;
    GtkWidget *label15;
    GtkWidget *label16;
    GtkWidget *label3;
  
    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_ref (obj_mainwin.notebook1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.notebook1", obj_mainwin.notebook1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.notebook1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
    gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);
  
    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "scrolledwindow1", scrolledwindow1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow1);

    gtk_widget_ref (clist1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "clist1", clist1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), clist1);
    gtk_clist_set_column_width (GTK_CLIST (clist1), 0, 120);
    gtk_clist_set_column_width (GTK_CLIST (clist1), 1, 80);
    gtk_clist_column_titles_show (GTK_CLIST (clist1));

    label15 = gtk_label_new (("Tag"));
    gtk_widget_ref (label15);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label15", label15,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label15);
    gtk_clist_set_column_widget (GTK_CLIST (clist1), 0, label15);

    label16 = gtk_label_new (("Value"));
    gtk_widget_ref (label16);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label16", label16,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label16);
    gtk_clist_set_column_widget (GTK_CLIST (clist1), 1, label16);

    label3 = gtk_label_new (("DBLink Info"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), label3);

    return;
}

