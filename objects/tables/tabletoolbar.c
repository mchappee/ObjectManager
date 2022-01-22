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
#include "tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void 
obj_create_table_toolbar(gpointer data, const char *dbversion)
{
    GtkWidget	 *button1;
    GtkWidget	 *button2;
    GtkWidget	 *button3;
    GtkWidget	 *button5;
    GtkWidget	 *button6;
    GtkWidget	 *button7;
    GtkWidget	 *button8;
    GtkWidget	 *tmp_toolbar_icon;
    int          tbstate;

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
					  ("Analyze\nTable"),
					  "analyze table ... compute statistics", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
  
    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_COUNT);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button3 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Row\nCount"),
					  "select count (*) from ...", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button3", button3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button3);

    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_CACHE);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    obj_mainwin.togglecache = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
							  GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
							  NULL,
							  ("Cache\nTable"),
							  "alter table ... cache", NULL,
							  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (obj_mainwin.togglecache);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.togglecache", obj_mainwin.togglecache,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.togglecache);
    
    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_ADD_COLUMN);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button5 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Add\nColumn"),
					  "Add a Column to the Table", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button5);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button5", button5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button5);

    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_RENAME);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button6 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Rename\nTable"),
					  "Rename Table", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button6);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button6", button6,(GtkDestroyNotify) gtk_widget_unref);

    if(dbversion)  /* Oracle 8 */
        gtk_widget_show (button6);

    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_TRUNCATE);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button7 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Truncate\nTable"),
					  "truncate table ...", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button7);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button7", button7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button7);
  
    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_INDEX);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button8 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Create\nIndex"),
					  "Create an index on the table", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button8);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button8", button8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button8);

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
			GTK_SIGNAL_FUNC (on_table_unallocate_clicked),
			data);
  
    tbstate=prefs_get_int_var(PREF_TBSTATE);
    if(tbstate == 1)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_TEXT);

    if(tbstate == 2)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_ICONS);

    if(tbstate == 3)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_BOTH);

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (obj_on_analyze_clicked),
			data);
    gtk_signal_connect (GTK_OBJECT (button3), "clicked",
			GTK_SIGNAL_FUNC (obj_on_rowcount_clicked),
			data);
    gtk_signal_connect (GTK_OBJECT (obj_mainwin.togglecache), "pressed",
			GTK_SIGNAL_FUNC (obj_on_cache_clicked),
			data);
    gtk_signal_connect (GTK_OBJECT (button5), "clicked",
			GTK_SIGNAL_FUNC (obj_on_addcolumn_clicked),
			data);
    if(dbversion){  /* Oracle 8 */
        gtk_signal_connect (GTK_OBJECT (button6), "clicked",
	  		    GTK_SIGNAL_FUNC (obj_on_rename_clicked),
			    data);
    }
    gtk_signal_connect (GTK_OBJECT (button7), "clicked",
			GTK_SIGNAL_FUNC (obj_on_truncate_clicked),
			data);
    gtk_signal_connect (GTK_OBJECT (button8), "clicked",
			GTK_SIGNAL_FUNC (obj_on_addindex_clicked),
			data);
}


