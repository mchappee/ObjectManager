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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "include/objectman.h"
#include "rollbackfuncs.h"

static void
obj_on_rollback_onoff_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    int		 num_elements;
    const char	 *dbid;
    const char	 *owner,*object,*otype;
    char	 *path_elements[4];
    char         *query;
    char         toggle_template[]="alter rollback segment %s %s";
    char         *toggle_val;
    char         *msg;
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    otype = path_elements[DIRPATH_TYPE];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];

    if (GTK_TOGGLE_BUTTON (button)->active==0)
        toggle_val="OFFLINE";
    else
        toggle_val="ONLINE";
    
    query = g_malloc(strlen(toggle_template) + strlen(toggle_val) + strlen(object) + 5);
    sprintf(query,toggle_template,object,toggle_val);
    
    if(execute_immediate_sql(dbid, query)){
        msg = g_malloc(strlen(toggle_val) + strlen(object) + 50);
	sprintf(msg,"Rollback Segment %s Toggled %s",object,toggle_val);
	msgbox(msg);
    }
    
    free_path_elements(path_elements, num_elements);
    
}

static void
obj_on_rollback_shrink_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    int		 num_elements;
    const char	 *dbid;
    const char	 *owner,*object,*otype;
    char	 *path_elements[4];
    char         *query;
    char         toggle_template[]="alter rollback segment %s shrink";
    char         *msg;
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    otype = path_elements[DIRPATH_TYPE];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];
    
    query = g_malloc(strlen(toggle_template) + strlen(object) + 5);
    sprintf(query,toggle_template,object);
    
    if(execute_immediate_sql(dbid, query)){
        msg = g_malloc(strlen(object) + 50);
	sprintf(msg,"Rollback Segment %s Shrunk",object);
	msgbox(msg);
    }
    
    free_path_elements(path_elements, num_elements);
}

void 
obj_create_rollback_toolbar(
    gpointer    data,
    int         toggled)
{
    GtkWidget	 *button3;
    GtkWidget	 *button4;
    GtkWidget	 *tmp_toolbar_icon;
    int          tbstate;

    obj_mainwin.dynamictoolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_widget_ref (obj_mainwin.dynamictoolbar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.dynamictoolbar1", obj_mainwin.dynamictoolbar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_RELIEF_NONE);
    gtk_widget_show (obj_mainwin.dynamictoolbar1);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.toolbar1), obj_mainwin.dynamictoolbar1, NULL, NULL);

    tmp_toolbar_icon = create_rollback_pixmap (ROLLBACKTOOLBAR_TOGGLE);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.");
    button3 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					  NULL,
					  "Toggle\nOn/Off",
					  "Toggle Online/Offline", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button3", button3);
    gtk_widget_show (button3);
    
    if(toggled)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button3),TRUE);
    
    gtk_signal_connect (GTK_OBJECT (button3), "clicked",
			GTK_SIGNAL_FUNC (obj_on_rollback_onoff_clicked),
			data);

    tmp_toolbar_icon = create_rollback_pixmap (ROLLBACKTOOLBAR_SHRINK);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.");
    button4 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  "Shrink\nDatafile",
					  "Shrink the Rollback to Original Size", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button4", button4);
    gtk_widget_show (button4);
    gtk_signal_connect (GTK_OBJECT (button4), "clicked",
			GTK_SIGNAL_FUNC (obj_on_rollback_shrink_clicked),
			data);
  
    tbstate=prefs_get_int_var(PREF_TBSTATE);
    if(tbstate == 1)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_TEXT);

    if(tbstate == 2)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_ICONS);

    if(tbstate == 3)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_BOTH);
}

