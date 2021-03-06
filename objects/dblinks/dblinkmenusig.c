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
#include <string.h>

#include "include/objectmansrc.h"
#include "include/objectman.h"
#include "dblinkfuncs.h"
#include "common.h"
#include <stdio.h>

#include <stdio.h>
#include <assert.h>


void
on_dbl_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    /* reselecting the row will cause a refresh of the 
       dblink info */
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}

void
on_dbl_all_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    /* reselecting the row will cause a refresh of the 
       dblink info */
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}


void
on_dbl_create_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show(obj_create_dblwin());
}


void
on_dbl_delete_activate(
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
    
    if(strcmp(owner,strupr(obj_cinfo.login->owner))==0 || strncmp(owner, "PUBLIC", 6)==0){
        if(confirm("You are about to drop this dblink.  This is a destructive operation "
                   "that cannot be undone.  Do you know what you're doing?")){
        if(strncmp(owner,"PUBLIC",6)==0)
                drop_object(dbid, owner, object, "PUBLIC DATABASE LINK",0);
	    else
	        drop_object(dbid, owner, object, "DATABASE LINK",0);
        gtk_ctree_remove_node (GTK_CTREE(obj_mainwin.tree1), 
	    gtk_ctree_node_nth(GTK_CTREE(obj_mainwin.tree1),dir->row));
        }
    }else{
        msgbox("Silly DBA, you can't drop another person's dblink.");
    }
    
    free_path_elements(path_elements, num_elements);

}


void
on_dbl_test_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);
    obj_on_dblink_clicked(NULL,user_data);
}

