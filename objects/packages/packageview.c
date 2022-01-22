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
#include "packagefuncs.h"
#include "objects/procedures/procfuncs.h"

char *elements_b;

void
on_package_notebook_switch_page(
    GtkNotebook     *notebook,
    GtkNotebookPage *page,
    gint             page_num,
    gpointer         user_data)
{
    char	     *path_elements[3];
    int		     num_elements;
    
    num_elements = parse_path(elements_b, path_elements, 3);
    gtk_widget_set_sensitive(obj_mainwin.dynamictoolbar1,1);
    
    switch(page_num){
        case 0:{
	    obj_mainwin.proctext1=obj_mainwin.proctext3;
	    getprocstatus(path_elements[0],path_elements[2],path_elements[1],0);
	    break;
	}
	case 1:{
	    obj_mainwin.proctext1=obj_mainwin.proctext2;
	    getprocstatus(path_elements[0],path_elements[2],path_elements[1],1);
	    break;
	}
    }

}

static void
on_packtext1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    gtk_label_set_text(GTK_LABEL (obj_mainwin.proclabelchanged),"Not Saved");
}

void
obj_dopackageview(char *elements)
{
    GtkWidget *label13;
    GtkWidget *hbox1;
    GtkWidget *hbox2;
    GtkWidget *vscrollbar1;

    elements_b=g_malloc(strlen(elements)+1);
    strcpy(elements_b,elements);

    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_show (obj_mainwin.notebook1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
    gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), hbox1);

    obj_mainwin.proctext3 = gtk_editor_new (NULL, NULL);
    gtk_widget_show (obj_mainwin.proctext3);
    gtk_text_set_editable (GTK_TEXT (obj_mainwin.proctext3), TRUE);
    gtk_container_add (GTK_CONTAINER (hbox1), obj_mainwin.proctext3);
    obj_mainwin.proctext1=obj_mainwin.proctext3;

    install_pat(GTK_EDITOR(obj_mainwin.proctext3),obj_mainwin.window1);

    vscrollbar1 = gtk_vscrollbar_new (GTK_TEXT (obj_mainwin.proctext3)->vadj);
    gtk_box_pack_start (GTK_BOX (hbox1), vscrollbar1, FALSE,FALSE, 0);
    gtk_widget_show (vscrollbar1);

    label13 = gtk_label_new ("Package definition");
    gtk_widget_show (label13);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), 
				label13);
    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox2);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), hbox2);
  
    obj_mainwin.proctext2 = gtk_editor_new (NULL, NULL);
    gtk_widget_show (obj_mainwin.proctext2);
    gtk_text_set_editable (GTK_TEXT (obj_mainwin.proctext2), TRUE);
    gtk_container_add (GTK_CONTAINER (hbox2), obj_mainwin.proctext2);

    install_pat(GTK_EDITOR(obj_mainwin.proctext2),obj_mainwin.window1);

    vscrollbar1 = gtk_vscrollbar_new (GTK_TEXT (obj_mainwin.proctext2)->vadj);
    gtk_box_pack_start (GTK_BOX (hbox2), vscrollbar1, FALSE,FALSE, 0);
    gtk_widget_show (vscrollbar1);

    label13 = gtk_label_new ("Package body");
    gtk_widget_show (label13);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 1), 
				label13);

    gtk_signal_connect (GTK_OBJECT (obj_mainwin.proctext3), "changed",
			GTK_SIGNAL_FUNC (on_packtext1_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_mainwin.notebook1), "switch_page",
			GTK_SIGNAL_FUNC (on_package_notebook_switch_page),
			(gpointer)elements);

    return;
}

void
obj_dopackageprocview(
    GtkCList *args)
{
    GtkWidget	 *scrolledwindow1;
    GtkWidget	 *label13;

    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_show (obj_mainwin.notebook1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
    gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow1);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), 
				    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), GTK_WIDGET(args));

    label13 = gtk_label_new ("Procedure Arguments");
    gtk_widget_show (label13);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), 
				label13);
}
