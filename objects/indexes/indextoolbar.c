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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "include/objectman.h"
#include "indexfuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void 
obj_create_index_toolbar(gpointer data)
{
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *button6;
    GtkWidget *button7;
    GtkWidget *tmp_toolbar_icon;
    int       tbstate;

    obj_mainwin.dynamictoolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_widget_ref (obj_mainwin.dynamictoolbar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.dynamictoolbar1", obj_mainwin.dynamictoolbar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_RELIEF_NONE);
    gtk_widget_show (obj_mainwin.dynamictoolbar1);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.toolbar1), obj_mainwin.dynamictoolbar1, NULL, NULL);

    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_ANALYZE);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button1 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Analyze\nIndex"),
					  "analyze index ...", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
  
    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_RENAME);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button6 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Rename\nIndex"),
					  "Rename Index", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button6);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button6", button6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button6);
  
    tmp_toolbar_icon = create_table_pixmap (INDEXTOOLBAR_REBUILD);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button7 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Rebuild\nIndex"),
					  "alter index ... rebuild", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button7);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button7", button7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button7);

    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_DEALLOCATE);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button2 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  "Deallocate",
					  "Deallocate Unused Space", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button2", button2);
    gtk_widget_show (button2);
    
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_index_unallocate_clicked),
			data);

/* Toggle unusable is ready to go, but Oracle doesn't change
   the STATUS from 'VALID' when it's flagged UNUSABLE 

    button3 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					  NULL,
					  "Toggle\nUsable",
					  "Toggle Usable/Unusable", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button3", button3);
    gtk_widget_show (button3);
    
    gtk_signal_connect (GTK_OBJECT (button3), "clicked",
			GTK_SIGNAL_FUNC (obj_on_index_unusable_clicked),
			data);
*/
    tbstate=prefs_get_int_var(PREF_TBSTATE);
    if(tbstate == 1)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_TEXT);

    if(tbstate == 2)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_ICONS);

    if(tbstate == 3)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_BOTH);

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (obj_on_ndxanalyze_clicked),
			data);
    gtk_signal_connect (GTK_OBJECT (button6), "clicked",
			GTK_SIGNAL_FUNC (obj_on_ndxrename_clicked),
			data);
    gtk_signal_connect (GTK_OBJECT (button7), "clicked",
			GTK_SIGNAL_FUNC (obj_on_ndxrebuild_clicked),
			data);
}


