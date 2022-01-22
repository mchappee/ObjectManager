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
#include <stdio.h>
#include <assert.h>

#include "include/objectmansrc.h"
#include "viewfuncs.h"
/* export functions are implemented in tables */
#include "objects/tables/tablefuncs.h"
#include "common-files/common.h"
#include "include/objectman.h"

void
on_view_refresh_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode	 *dir	  = (DirNode *)user_data;

    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);
    fprintf(stderr, "refresh view %s\n", dir->path);

    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}

void
on_view_all_refresh_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode	 *dir	  = (DirNode *)user_data;

    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);
    fprintf(stderr, "refresh view %s\n", dir->path);

    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}


void
on_view_create_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    gtk_widget_show(obj_create_viewwin());
}


void
on_view_delete_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode *dir = (DirNode *)user_data;
    int		 num_elements;
    const char	 *dbid;
    const char	 *owner,*object,*otype;
    char	 *path_elements[4];
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    otype = path_elements[DIRPATH_TYPE];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];
    
    if(confirm("You are about to drop this view.  This is a destructive operation "
               "that cannot be undone.  Do you know what you're doing?")){
        drop_object(dbid, owner, object, "VIEW", 1);
	gtk_ctree_remove_node (GTK_CTREE(obj_mainwin.tree1), 
	    gtk_ctree_node_nth(GTK_CTREE(obj_mainwin.tree1),dir->row));
    }

    free_path_elements(path_elements, num_elements);
}


void
on_view_rename_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode	 *dir	  = (DirNode *)user_data;

    fprintf(stderr, "rename view %s\n", dir->path);
    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}

void
on_view_export_csv_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode	 *dir	  = (DirNode *)user_data;
    GtkWidget	 *dialog  = create_export_fileselection (dir, export_as_csv, NULL);

    gtk_file_selection_complete (GTK_FILE_SELECTION(dialog), "*.csv");
    gtk_widget_show (dialog);
    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}


void
on_view_export_html_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode	 *dir	  = (DirNode *)user_data;
    GtkWidget	 *dialog  = create_export_fileselection (dir, export_as_html, NULL);

    gtk_file_selection_complete (GTK_FILE_SELECTION(dialog), "*.html");
    gtk_widget_show (dialog);
    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}

void
on_view_export_sql_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode	 *dir	  = (DirNode *)user_data;
    GtkWidget	 *dialog  = create_export_fileselection (dir, export_as_sql, NULL);

    gtk_file_selection_complete (GTK_FILE_SELECTION(dialog), "*.sql");
    gtk_widget_show (dialog);
    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}


void
on_view_script_activate(
    GtkMenuItem	 *menuitem,
    gpointer	 user_data)
{
    DirNode	 *dir	  = (DirNode *)user_data;

    fprintf(stderr, "Generate SQL script for view %s\n", dir->path);

    gtk_widget_destroy(GTK_WIDGET(dir->menu));
    dir->menu = 0;
}

void
on_view_grant_privilege_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode	 *dir	       = (DirNode *)user_data;
    char	 *path_elements[4];
    int		 num_elements  = parse_path(dir->path, path_elements, 4);

    assert(num_elements >= 4);
    
    gtk_widget_show(grant_privilege_win(path_elements[DIRPATH_OWNER],
                                path_elements[DIRPATH_OBJECT],
				OMDB_View, path_elements[DIRPATH_DBID]));
}

