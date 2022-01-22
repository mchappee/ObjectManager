/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright ? Matthew Chappee <matthew@orasoft.org>
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
#include <gdk/gdkkeysyms.h>

#include "include/objectman.h"
#include "common-files/common.h"
#include "tablefuncs.h"

#include <stdio.h>
#include <assert.h>
#include <dlfcn.h>

/* create file selection dialog for data export options */
GtkWidget*
create_export_fileselection (
    DirNode	 *dir,
    void	 (*exporter)(const char *, GtkCList *),
    void         *handle)
{
    GtkWidget	 *fileselection1;
    GtkWidget	 *ok_button1;
    GtkWidget	 *cancel_button1;

    fileselection1 = gtk_file_selection_new (_("Load Call Answer Set-Up"));
    gtk_object_set_data (GTK_OBJECT (fileselection1), "fileselection1", fileselection1);
    gtk_object_set_data (GTK_OBJECT (fileselection1), "dir", dir);
    gtk_object_set_data (GTK_OBJECT (fileselection1), "exporter", exporter);
    gtk_object_set_data (GTK_OBJECT (fileselection1), "handle", handle);
    gtk_container_set_border_width (GTK_CONTAINER (fileselection1), 10);
    gtk_window_set_modal (GTK_WINDOW (fileselection1), TRUE);

    ok_button1 = GTK_FILE_SELECTION (fileselection1)->ok_button;
    gtk_widget_show (ok_button1);
    GTK_WIDGET_SET_FLAGS (ok_button1, GTK_CAN_DEFAULT);

    cancel_button1 = GTK_FILE_SELECTION (fileselection1)->cancel_button;
    gtk_widget_show (cancel_button1);
    GTK_WIDGET_SET_FLAGS (cancel_button1, GTK_CAN_DEFAULT);

    gtk_signal_connect (GTK_OBJECT (ok_button1), "clicked",
			GTK_SIGNAL_FUNC (on_fs_ok_button_clicked),
			fileselection1);
    gtk_signal_connect (GTK_OBJECT (cancel_button1), "clicked",
			GTK_SIGNAL_FUNC (on_fs_cancel_button_clicked),
			fileselection1);

    return fileselection1;
}

void
on_fs_ok_button_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char	      qtemplate[]  = "SELECT * from %s.%s";
    char	      *query;
    char	      *path_elements[4];
    int		      num_elements;
    GtkCList	      *l;
    DirNode	      *dir;
    void	      (*exporter)(const char *, GtkCList *);
    void              *handle;
    GtkFileSelection  *fs	   = GTK_FILE_SELECTION(user_data);
    
    dir = gtk_object_get_data (GTK_OBJECT (fs), "dir");
    exporter = gtk_object_get_data (GTK_OBJECT (fs), "exporter");
    handle = gtk_object_get_data (GTK_OBJECT (fs), "handle");
    
    assert(dir != 0);
    assert(exporter != 0);

    num_elements = parse_path(dir->path, path_elements, 4);
    assert(num_elements >= 4);

    query = g_malloc(sizeof(qtemplate) + strlen(path_elements[DIRPATH_OWNER]) +
		     strlen(path_elements[DIRPATH_OBJECT]) + 1);
    sprintf(query, qtemplate, path_elements[DIRPATH_OWNER], 
	    path_elements[DIRPATH_OBJECT]);

    l = clist_from_query(query, path_elements[DIRPATH_DBID], 0, "<NULL>", active_step);
    active_clear();
    if (l) {
	gtk_object_set_data_full (GTK_OBJECT (l), "owner", 
				  g_strdup(path_elements[DIRPATH_OWNER]), g_free);
	gtk_object_set_data_full (GTK_OBJECT (l), "tablename", 
				  g_strdup(path_elements[DIRPATH_OBJECT]), g_free);

	exporter(gtk_file_selection_get_filename(fs), l);
	
	if(handle)
	    dlclose(handle);
	gtk_widget_destroy(GTK_WIDGET(l));
    }
    free_path_elements(path_elements, num_elements);
    gtk_widget_destroy(GTK_WIDGET(fs));
}

void
on_fs_cancel_button_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(GTK_WIDGET(user_data));
}

void
on_tab_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    /* reselecting the row will cause a refresh of the 
       table info */
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);
}

void
on_tab_all_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    /* reselecting the row will cause a refresh of the 
       table info */
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);
}


void
on_tab_create_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show (get_twwinmain_widget());
}


void
on_tab_delete_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode	 *dir	       = (DirNode *)user_data;
    char	 *path_elements[4];
    int		 num_elements  = parse_path(dir->path, path_elements, 4);

    assert(num_elements >= 4);

    if (confirm("Really remove the table %s.%s from the database",
		path_elements[DIRPATH_OWNER], path_elements[DIRPATH_OBJECT])) {
	static char qdroptemplate[] = "DROP TABLE %s.%s";
	char *qdrop = g_malloc(sizeof(qdroptemplate) + 
			       strlen(path_elements[DIRPATH_OWNER]) +
			       strlen(path_elements[DIRPATH_OBJECT]));
	sprintf(qdrop, qdroptemplate, path_elements[DIRPATH_OWNER],
		path_elements[DIRPATH_OBJECT]);
	if (execute_immediate_sql(path_elements[DIRPATH_DBID], qdrop)) {
	    gtk_ctree_remove_node (GTK_CTREE(obj_mainwin.tree1), 
	        gtk_ctree_node_nth(GTK_CTREE(obj_mainwin.tree1),dir->row));
	    msgbox("Table dropped");
	}
    }
}


void
on_tab_rename_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show (obj_create_renamewin(user_data));
}


void
on_tab_clear_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode	 *dir	       = (DirNode *)user_data;
    char	 *path_elements[4];
    int		 num_elements  = parse_path(dir->path, path_elements, 4);

    assert(num_elements >= 4);

    if (confirm("Really remove all data from the table %s.%s",
		path_elements[DIRPATH_OWNER], path_elements[DIRPATH_OBJECT])) {
	static char qdeletetemplate[] = "TRUNCATE TABLE %s.%s";
	char *qdelete = g_malloc(sizeof(qdeletetemplate) + 
			       strlen(path_elements[DIRPATH_OWNER]) +
			       strlen(path_elements[DIRPATH_OBJECT]));
	sprintf(qdelete, qdeletetemplate, path_elements[DIRPATH_OWNER],
		path_elements[DIRPATH_OBJECT]);
	if (execute_immediate_sql(path_elements[DIRPATH_DBID], qdelete)) {
	    msgbox("Table cleared");
	}
    }
}


void
on_tab_grant_privilege_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode	 *dir	       = (DirNode *)user_data;
    char	 *path_elements[4];
    int		 num_elements  = parse_path(dir->path, path_elements, 4);

    assert(num_elements >= 4);

    gtk_widget_show(grant_privilege_win(path_elements[DIRPATH_OWNER],
                                path_elements[DIRPATH_OBJECT],
				OMDB_Table, path_elements[DIRPATH_DBID]));
}



void
on_tab_export_csv_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    GtkWidget *dialog = create_export_fileselection (dir, export_as_csv, NULL);

    gtk_file_selection_complete (GTK_FILE_SELECTION(dialog), "*.csv");
    gtk_widget_show (dialog);
    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}


void
on_tab_export_html_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    GtkWidget *dialog = create_export_fileselection (dir, export_as_html, NULL);

    gtk_file_selection_complete (GTK_FILE_SELECTION(dialog), "*.html");
    gtk_widget_show (dialog);
    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}

void
on_tab_export_sql_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    GtkWidget *dialog = create_export_fileselection (dir, export_as_sql, NULL);

    gtk_file_selection_complete (GTK_FILE_SELECTION(dialog), "*.sql");
    gtk_widget_show (dialog);
    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}

void
on_dl_export_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode    *dir   = (DirNode *)user_data;
    int        dl_id  = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(menuitem), "dl_id"));
    GtkWidget  *dialog;
    void       *handle;
    
    handle = dlopen (dl_info[dl_id].dl_filepath, RTLD_LAZY);
    if (!handle) {
        msgbox(dlerror());
        return;
    }
    
    dialog = create_export_fileselection (dir, dlsym(handle, "export_procedure"), handle);

    gtk_file_selection_complete (GTK_FILE_SELECTION(dialog), dl_info[dl_id].dl_extension);
    gtk_widget_show (dialog);
    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}


void
on_tab_script_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    fprintf(stderr, "Generate SQL script for table %s\n", dir->path);
}

