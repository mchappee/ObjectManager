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
#include "sessionfuncs.h"

void
obj_dosessionview(){
    GtkWidget *label3;
    GtkWidget *label4;
    GtkWidget *label5;
    GtkWidget *scrolledwindow2;
    GtkWidget *notelabel1;
    GtkWidget *vbox2;
    
    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_ref (obj_mainwin.notebook1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.notebook1", obj_mainwin.notebook1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.notebook1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
    gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);
  
    scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "scrolledwindow2", scrolledwindow2);
    gtk_widget_show (scrolledwindow2);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow2);

    obj_mainwin.sessionclist2 = gtk_clist_new (3);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.sessionclist2", obj_mainwin.sessionclist2);
    gtk_widget_set_usize (obj_mainwin.sessionclist2, 600, 200);
    gtk_widget_show (obj_mainwin.sessionclist2);
    gtk_container_add (GTK_CONTAINER (scrolledwindow2), obj_mainwin.sessionclist2);
    gtk_clist_set_column_width (GTK_CLIST (obj_mainwin.sessionclist2), 0, 130);
    gtk_clist_set_column_width (GTK_CLIST (obj_mainwin.sessionclist2), 1, 120);
    gtk_clist_set_column_width (GTK_CLIST (obj_mainwin.sessionclist2), 2, 80);
    gtk_clist_column_titles_hide (GTK_CLIST (obj_mainwin.sessionclist2));

    label3 = gtk_label_new ("label3");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Catagory", label3);
    gtk_widget_show (label3);
    gtk_clist_set_column_widget (GTK_CLIST (obj_mainwin.sessionclist2), 0, label3);

    label4 = gtk_label_new ("label4");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Tag", label4);
    gtk_widget_show (label4);
    gtk_clist_set_column_widget (GTK_CLIST (obj_mainwin.sessionclist2), 1, label4);

    label5 = gtk_label_new ("label5");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Value", label5);
    gtk_widget_show (label5);
    gtk_clist_set_column_widget (GTK_CLIST (obj_mainwin.sessionclist2), 2, label5);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox2);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), vbox2);

    obj_mainwin.sessiontext1 = add_sql_window(vbox2);
  
    notelabel1 = gtk_label_new (("Session Info"));
    gtk_widget_ref (notelabel1);
    gtk_widget_show (notelabel1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), notelabel1);
				
    notelabel1 = gtk_label_new (("SQL View"));
    gtk_widget_ref (notelabel1);
    gtk_widget_show (notelabel1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 1), notelabel1);
  
    return;
}

void
obj_session_create()
{
    
}

