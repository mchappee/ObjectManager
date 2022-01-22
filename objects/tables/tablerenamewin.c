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
#include <assert.h>

char      *object;
char      *owner;
char      *objecttype;
char      *dbid;
GtkWidget *entry3;

void
obj_on_cancelrename_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    g_free(owner);
    g_free(objecttype);
    g_free(object);
    g_free(dbid);
    gtk_widget_destroy(user_data);
}


void
obj_on_okrename_clicked(
    GtkButton	*button,
    gpointer	user_data)
{
    char	*newname = gtk_entry_get_text(GTK_ENTRY(entry3));
    char        *query;

    if(strlen(newname)){
	if (!strcmp(objecttype,"Tables")) {
	    query = g_malloc(strlen(owner) + strlen(object) + strlen(newname)+ 30);
	    sprintf(query,"ALTER TABLE %s.%s RENAME TO %s",owner,object,newname);
	    
	    if (execute_immediate_sql(dbid,query)) {
		msgbox("Table Renamed");
	    }
	    g_free(query);
	} else {
	    query = g_malloc(strlen(owner) + strlen(object) + strlen(newname)+ 30);
	    sprintf(query,"ALTER INDEX %s.%s RENAME TO %s",owner,object,newname);
	    
	    if (execute_immediate_sql(dbid,query)) {
		msgbox("Index Renamed");
	    }
	    g_free(query);
	}
    }
    g_free(owner);
    g_free(objecttype);
    g_free(object);
    g_free(dbid);
    gtk_widget_destroy(user_data);
}

GtkWidget*
obj_create_renamewin (gpointer data)
{
    GtkWidget *window1;
    GtkWidget *vbox1;
    GtkWidget *button2;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    int          num_elements=0;
    char         *path_elements[4];
    DirNode      *dir  = (DirNode *)data;

    num_elements=parse_path(dir->path, path_elements, 4);
    
    dbid = g_malloc(strlen(path_elements[DIRPATH_DBID])+1);
    strcpy(dbid,path_elements[DIRPATH_DBID]);
    objecttype = g_malloc(strlen(path_elements[DIRPATH_TYPE])+1);
    strcpy(objecttype,path_elements[DIRPATH_TYPE]);
    owner = g_malloc(strlen(path_elements[DIRPATH_OWNER])+1);
    strcpy(owner,path_elements[DIRPATH_OWNER]);
    object = g_malloc(strlen(path_elements[DIRPATH_OBJECT])+1);
    strcpy(object,path_elements[DIRPATH_OBJECT]);
    free_path_elements(path_elements, num_elements);

    window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
    gtk_window_set_title (GTK_WINDOW (window1), "New Name:");
    gtk_window_set_policy (GTK_WINDOW (window1), TRUE, TRUE, FALSE);
    gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_object_set_data (GTK_OBJECT (window1), "vbox1", vbox1);
    gtk_widget_set_usize (vbox1, 300, 100);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window1), vbox1);

    entry3 = gtk_entry_new ();
    gtk_widget_ref (entry3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry3", entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry3);
    gtk_container_add (GTK_CONTAINER (vbox1), entry3);
    
    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
				  (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, TRUE, TRUE, 0);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"OK");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
				  (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    
    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
				  (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			    GTK_SIGNAL_FUNC (obj_on_okrename_clicked),
			    window1);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			    GTK_SIGNAL_FUNC (obj_on_cancelrename_clicked),
			    window1);

    return window1;
}
