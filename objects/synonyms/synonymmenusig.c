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
#include "synonymfuncs.h"
#include "include/objectman.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

void
on_syn_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    /* reselecting the row will cause a refresh of the 
       synonym info */
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}

void
on_syn_all_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    /* reselecting the row will cause a refresh of the 
       synonym info */
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}


void
on_syn_create_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show(obj_create_synwin());
}


void
on_syn_delete_activate(
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
    
    if(confirm("You are about to drop this synonym.  This is a destructive operation "
               "that cannot be undone.  Do you know what you're doing?")){
	if(strncmp(owner,"PUBLIC",6)==0)
            drop_object(dbid, owner, object, "PUBLIC SYNONYM",0);
	else
	    drop_object(dbid, owner, object, "SYNONYM",1);
	    
	gtk_ctree_remove_node (GTK_CTREE(obj_mainwin.tree1), 
	    gtk_ctree_node_nth(GTK_CTREE(obj_mainwin.tree1),dir->row));
    }

    free_path_elements(path_elements, num_elements);
}


