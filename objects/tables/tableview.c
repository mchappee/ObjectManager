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
#include "tablefuncs.h"

void
obj_dotableview(
    GtkCList *column_list,
    GtkWidget *clist2,
    GtkWidget *clist4,
    GtkWidget *clist5)
{
    GtkWidget *label10;
    GtkWidget *label11;
    GtkWidget *label13;
    GtkWidget *label14;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *label3;
    GtkWidget *label7;
    GtkWidget *label8;
    GtkWidget *scrolledwindow1;
    GtkWidget *scrolledwindow2;
    GtkWidget *scrolledwindow3;
    GtkWidget *scrolledwindow4;
    GtkWidget *scrolledwindow5;
    GtkWidget *clist1;

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

    clist1 = GTK_WIDGET(column_list);
    gtk_widget_ref (clist1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "clist1", clist1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), clist1);
    glist_set_opt_col_width(clist1, 0);
    glist_set_opt_col_width(clist1, 1);
    glist_set_opt_col_width(clist1, 2);
    glist_set_opt_col_width(clist1, 3);

    gtk_clist_column_titles_show (GTK_CLIST (clist1));

    label1 = gtk_label_new (("Column Data"));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), label1);

    scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "scrolledwindow2", scrolledwindow2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow2);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow2);

    gtk_widget_ref (clist2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "clist2", clist2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist2);
    gtk_container_add (GTK_CONTAINER (scrolledwindow2), clist2);
    gtk_clist_set_column_width (GTK_CLIST (clist2), 0, 160);
    gtk_clist_set_column_width (GTK_CLIST (clist2), 1, 80);
    gtk_clist_column_titles_show (GTK_CLIST (clist2));

    label7 = gtk_label_new (("Index Name"));
    gtk_widget_ref (label7);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label7", label7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label7);
    gtk_clist_set_column_widget (GTK_CLIST (clist2), 0, label7);

    label8 = gtk_label_new (("Uniqueness"));
    gtk_widget_ref (label8);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label8", label8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label8);
    gtk_clist_set_column_widget (GTK_CLIST (clist2), 1, label8);

    label2 = gtk_label_new (("Indexes"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 1), label2);

    scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "scrolledwindow3", scrolledwindow3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow3);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow3);

    gtk_widget_ref (clist4);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "clist4", clist4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist4);
    gtk_container_add (GTK_CONTAINER (scrolledwindow3), clist4);
    gtk_clist_set_column_width (GTK_CLIST (clist4), 0, 200);
    gtk_clist_set_column_width (GTK_CLIST (clist4), 1, 120);
    gtk_clist_column_titles_show (GTK_CLIST (clist4));

    label10 = gtk_label_new (("Tag"));
    gtk_widget_ref (label10);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label10", label10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label10);
    gtk_clist_set_column_widget (GTK_CLIST (clist4), 0, label10);

    label11 = gtk_label_new (("Value"));
    gtk_widget_ref (label11);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label11", label11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label11);
    gtk_clist_set_column_widget (GTK_CLIST (clist4), 1, label11);

    label3 = gtk_label_new (("Table Info"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 2), label3);

    scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow4);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "scrolledwindow4", scrolledwindow4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow4);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow4);

    label13 = gtk_label_new (("Table Contents"));
    gtk_widget_ref (label13);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label13", label13,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label13);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 3), 
				label13);
				
    scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow5);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "scrolledwindow5", scrolledwindow5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow5);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow5);
    
    gtk_widget_ref (clist5);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "clist5", clist5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist5);
    gtk_container_add (GTK_CONTAINER (scrolledwindow5), clist5);
    gtk_clist_set_column_width (GTK_CLIST (clist5), 0, 200);
    gtk_clist_set_column_width (GTK_CLIST (clist5), 1, 120);
    gtk_clist_column_titles_show (GTK_CLIST (clist5));
    
    label14 = gtk_label_new (("Constraints"));
    gtk_widget_ref (label14);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label14", label14,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label14);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 4), 
				label14);

    gtk_signal_connect (GTK_OBJECT (obj_mainwin.notebook1), "switch_page",
			GTK_SIGNAL_FUNC (on_table_notebook_switch_page),
			scrolledwindow4);

    return;
}
