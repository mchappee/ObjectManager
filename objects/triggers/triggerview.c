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
#include "triggerfuncs.h"

void
obj_dotriggerview(
    GtkCList	 *info,
    const char	 *body)
{
    GtkWidget	 *scrolledwindow1;
    GtkWidget	 *label13;
    GtkWidget	 *hbox1;
    GtkWidget	 *vscrollbar1;

    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_show (obj_mainwin.notebook1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
    gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow1);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), 
				    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), scrolledwindow1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), GTK_WIDGET(info));

    label13 = gtk_label_new ("Trigger Info");
    gtk_widget_show (label13);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), 
				label13);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), hbox1);

    if(obj_ginfo.highlighting){
        obj_mainwin.proctext1 = gtk_editor_new(NULL, NULL);
	install_pat(GTK_EDITOR(obj_mainwin.proctext1),obj_mainwin.window1);
    }else
        obj_mainwin.proctext1 = gtk_text_new(NULL, NULL);

    gtk_widget_show (obj_mainwin.proctext1);
    gtk_box_pack_start (GTK_BOX (hbox1), obj_mainwin.proctext1, TRUE,TRUE, 0);

    vscrollbar1 = gtk_vscrollbar_new (GTK_TEXT (obj_mainwin.proctext1)->vadj);
    gtk_box_pack_start (GTK_BOX (hbox1), vscrollbar1, FALSE,FALSE, 0);
    gtk_widget_show (vscrollbar1);

    gtk_text_set_editable (GTK_TEXT(obj_mainwin.proctext1), TRUE);
    gtk_text_insert(GTK_TEXT(obj_mainwin.proctext1), NULL, NULL, NULL, body, -1);

    if(obj_ginfo.highlighting)
	    gtk_editor_hilite_buffer (GTK_EDITOR(obj_mainwin.proctext1));

    label13 = gtk_label_new ("Trigger body");
    gtk_widget_show (label13);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 1), 
				label13);

    return;
}
