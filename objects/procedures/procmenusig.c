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
#include <gdk/gdkkeysyms.h>

#include "include/objectmansrc.h"
#include "include/miscfunc.h"
#include "procfuncs.h"
#include "common.h"
#include "include/objectman.h"
#include "objects/types/typefuncs.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>


void
on_proc_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}

void
on_proc_all_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}


void
on_proc_create_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
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
    
    if(strncmp(otype,"Procedure",9)==0)
        gtk_widget_show(create_procedure());
    
    if(strncmp(otype,"Package",7)==0)
        gtk_widget_show(create_package());
	
    if(strncmp(otype,"Function",8)==0)
        gtk_widget_show(create_function());

    if(strncmp(otype,"Type",4)==0)
        gtk_widget_show(obj_create_typewin());

    free_path_elements(path_elements, num_elements);
}


void
on_proc_delete_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
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
    
    if(confirm("You are about to drop this code object.  This is a destructive operation "
               "that cannot be undone.  Do you know what you're doing?")){
	       
        if(strncmp(otype,"Procedure",9)==0)
            drop_object(dbid, owner, object, "PROCEDURE", 1);
    
        if(strncmp(otype,"Package",7)==0)
            drop_object(dbid, owner, object, "PACKAGE", 1);
	
        if(strncmp(otype,"Function",8)==0)
            drop_object(dbid, owner, object, "FUNCTION", 1);
	    
	if(strncmp(otype,"Type",4)==0)
            drop_object(dbid, owner, object, "TYPE", 1);
	    
	gtk_ctree_remove_node (GTK_CTREE(obj_mainwin.tree1), 
	    gtk_ctree_node_nth(GTK_CTREE(obj_mainwin.tree1),dir->row));

    }
    
    free_path_elements(path_elements, num_elements);
}

void
on_proc_grant_privilege_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode	 *dir	       = (DirNode *)user_data;
    char	 *path_elements[4];
    int		 num_elements  = parse_path(dir->path, path_elements, 4);

    assert(num_elements >= 4);
    gtk_widget_show(grant_privilege_win(path_elements[DIRPATH_OWNER],
                                path_elements[DIRPATH_OBJECT],
				OMDB_Procedure, path_elements[DIRPATH_DBID]));
}

