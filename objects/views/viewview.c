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
#include "viewfuncs.h"

void
obj_doviewview(
    GtkCList *column_list)
{
    GtkWidget *vbox1;
    GtkWidget *clist1;
    GtkWidget *hbox1;
    GtkWidget *scrolledwindow1;
    GtkWidget *scrolledwindow4;
    GtkWidget *label12;
    GtkWidget *label13;
    GtkWidget *vscrollbar1;

    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_show (obj_mainwin.notebook1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
    gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_usize (vbox1, 800, 600);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), vbox1);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

    if(obj_ginfo.highlighting){
        obj_mainwin.proctext1 = gtk_editor_new(NULL, NULL);
	install_pat(GTK_EDITOR(obj_mainwin.proctext1),obj_mainwin.window1);
    }else
        obj_mainwin.proctext1 = gtk_text_new(NULL, NULL);

    gtk_widget_show (obj_mainwin.proctext1);
    gtk_container_add (GTK_CONTAINER (hbox1), obj_mainwin.proctext1);
    gtk_text_set_editable (GTK_TEXT (obj_mainwin.proctext1), TRUE);

    vscrollbar1 = gtk_vscrollbar_new (GTK_TEXT (obj_mainwin.proctext1)->vadj);
    gtk_box_pack_start (GTK_BOX (hbox1), vscrollbar1, FALSE,FALSE, 0);
    gtk_widget_show (vscrollbar1);

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow1);
    gtk_container_add (GTK_CONTAINER (vbox1), scrolledwindow1);

    clist1 = GTK_WIDGET(column_list);
    gtk_widget_show (clist1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), clist1);
    gtk_clist_set_column_width (GTK_CLIST (clist1), 0, 120);
    gtk_clist_set_column_width (GTK_CLIST (clist1), 1, 120);
    gtk_clist_column_titles_show (GTK_CLIST (clist1));
    glist_set_opt_col_width(clist1, 0);
    glist_set_opt_col_width(clist1, 1);
    glist_set_opt_col_width(clist1, 2);
    glist_set_opt_col_width(clist1, 3);


    label12 = gtk_label_new (("View Info"));
    gtk_widget_show (label12);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), label12);
		      
    scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow4);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow4);

    label13 = gtk_label_new (("View Contents"));
    gtk_widget_show (label13);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 1), 
				label13);

    gtk_signal_connect (GTK_OBJECT (obj_mainwin.notebook1), "switch_page",
			GTK_SIGNAL_FUNC (on_view_notebook_switch_page),
			scrolledwindow4);

    return;
}

