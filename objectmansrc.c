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

GtkWidget *obj_confirm;

obj_progwin_struct obj_progwin;
obj_mainwin_struct obj_mainwin;

GtkWidget*
obj_create_window1 ()
{
    GtkWidget *vbox1;
    GtkWidget *menubar1;
    GtkWidget *combo_entry1;
    GtkWidget *hbox3;
    
    obj_mainwin.window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.window1", obj_mainwin.window1);
    gtk_window_set_title (GTK_WINDOW (obj_mainwin.window1), ("Oracle ObjectManager v2.0"));
    gtk_window_position (GTK_WINDOW (obj_mainwin.window1), GTK_WIN_POS_CENTER);
    gtk_signal_connect(GTK_OBJECT(obj_mainwin.window1), "destroy",
		       GTK_SIGNAL_FUNC(gtk_widget_destroyed), &obj_mainwin.window1);
    gtk_signal_connect (GTK_OBJECT (obj_mainwin.window1), "destroy",
			GTK_SIGNAL_FUNC (obj_destroy), NULL);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_widget_set_usize (vbox1, 800, 600);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.window1), vbox1);

    hbox3 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show (hbox3);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox3, FALSE, FALSE, 0);

    menubar1 = gtk_menu_bar_new ();
    gtk_widget_ref (menubar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "menubar1", menubar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (menubar1);
    gtk_box_pack_start (GTK_BOX (hbox3), menubar1, TRUE, TRUE, 0);

    obj_create_main_toolbar(vbox1);
    obj_create_menu(menubar1);

    obj_mainwin.hbox1 = gtk_hpaned_new ();
    gtk_widget_ref (obj_mainwin.hbox1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.hbox1", obj_mainwin.hbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.hbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), obj_mainwin.hbox1, TRUE, TRUE, 0);

    obj_mainwin.scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (obj_mainwin.scrolledwindow1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.scrolledwindow1", obj_mainwin.scrolledwindow1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.scrolledwindow1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.scrolledwindow1);

    obj_doorasoft();

    obj_mainwin.hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show (obj_mainwin.hbox2);
    gtk_box_pack_start (GTK_BOX (vbox1), obj_mainwin.hbox2, FALSE, FALSE, 0);

    obj_mainwin.toolbarcombo1 = gtk_combo_new ();
    gtk_widget_ref (obj_mainwin.toolbarcombo1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.toolbarcombo1", obj_mainwin.toolbarcombo1,
                            (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.toolbarcombo1);
    gtk_box_pack_start (GTK_BOX (obj_mainwin.hbox2), obj_mainwin.toolbarcombo1, FALSE, FALSE, 0);

    combo_entry1 = GTK_COMBO (obj_mainwin.toolbarcombo1)->entry;
    gtk_widget_ref (combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "combo_entry1", combo_entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry1);

    obj_mainwin.statusbar1 = gtk_statusbar_new ();
    gtk_widget_ref (obj_mainwin.statusbar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.statusbar1", obj_mainwin.statusbar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.statusbar1);
    gtk_box_pack_start (GTK_BOX (obj_mainwin.hbox2), obj_mainwin.statusbar1, TRUE, TRUE, 0);

    gtk_signal_connect (GTK_OBJECT (combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_dbid_combo_changed),
			NULL);

    return obj_mainwin.window1;
}

void
obj_do_generic_view(
    GtkCList *clist,
    const char *tab)
{
    GtkWidget *label3;
    GtkWidget *scrolledwindow1;

    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_ref (obj_mainwin.notebook1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), 
			      "obj_mainwin.notebook1", obj_mainwin.notebook1,
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

    if (clist) {
	gtk_widget_show (GTK_WIDGET(clist));
	gtk_container_add (GTK_CONTAINER (scrolledwindow1), GTK_WIDGET(clist));
    }

    label3 = gtk_label_new (tab);
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), label3);
}
