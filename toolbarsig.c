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
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void
obj_on_button1_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_main_quit();
}


/*void
obj_on_button3_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    printf("\nDrop object %s",obj_cinfo.currentobject);
    fflush(stdout);
}
*/

void
obj_on_addconnect_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_show (obj_create_login());
}

void
on_dbid_combo_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    GtkCTreeNode  *tnode;
    int           i;
    DirNode       *dirnode;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	for(i=0; i < GTK_CTREE(obj_mainwin.tree1)->clist.rows; i++){
            tnode = gtk_ctree_node_nth(GTK_CTREE(obj_mainwin.tree1),i);
	    dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), tnode);
	    if(!strcmp(dirnode->path, entry))
                gtk_ctree_select(GTK_CTREE(obj_mainwin.tree1), tnode);
        }
    }
    
    
}

/* void
obj_on_dropnode_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    GtkCTreeNode *tnode;
    GList *list_node;
    
    list_node = GTK_CLIST(obj_mainwin.tree1)->selection;
    tnode = list_node->data;
    
    gtk_ctree_remove_node (GTK_CTREE(obj_mainwin.tree1),tnode);
    gtk_widget_set_sensitive(obj_toolbarbuttons.button5,0);
}
*/

