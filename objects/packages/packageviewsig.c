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
#include "packagefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

/* Callback used to remove a node's children. The top level, which we
   wish to preserve is passed as user data so that we can identify it */
static void
remove_tree_node (
    GtkCTree     *ctree,
    GtkCTreeNode *node,
    gpointer      data)
{
    if ((GtkCTreeNode *)data != node)
	gtk_ctree_remove_node(ctree, node);
}

void
on_package_expand_cb(
    DirNode      *dir,
    GtkWidget    *widget,
    GtkCTreeNode *parent_node)
{
    if(dir->scanned==FALSE){
	char *path_elements[5];
	int num_elements = parse_path(dir->path, path_elements, 5);

	gtk_clist_freeze(GTK_CLIST(widget));
	/* Remove the dummy child which was inserted so that the
	   expand/collapse box is displayed */
	gtk_ctree_pre_recursive_to_depth(GTK_CTREE(widget), parent_node, 
					 GTK_CTREE_ROW (parent_node)->level + 1, 
					 remove_tree_node, parent_node);

	loadpackageprocs(parent_node, path_elements[DIRPATH_DBID], 
			 path_elements[DIRPATH_OWNER], 
			 path_elements[DIRPATH_OBJECT]);
	gtk_clist_thaw(GTK_CLIST(widget));

	free_path_elements(path_elements, num_elements);
	dir->scanned=TRUE;
    }
}
