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
#include "objects/views/viewfuncs.h"
#include "objects/tables/tablefuncs.h"
#include "objects/tablespaces/tablespacefuncs.h"

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

/* Refresh the list of owners for a given object type */
void
on_objtype_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode	     *dir     = (DirNode *)user_data;
    char	     *dbid;
    char	     *otype;
    char	     *dbversion;
    char	     *path_elements[4];
    int		     num_elements;
    int              expanded = GTK_CTREE_ROW (dir->node)->expanded;

    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    otype = path_elements[DIRPATH_TYPE];

    dbversion = obj_cinfo.login->dbversion;

    gtk_clist_freeze(GTK_CLIST(obj_mainwin.tree1));
    /* Remove the old children */
    gtk_ctree_post_recursive_to_depth(dir->tree, dir->node, GTK_CTREE_ROW (dir->node)->level + 1, 
				     remove_tree_node, dir->node);
    /* re-populate tree under this node */
    if (dir->db_object->list_owners) {
	dir->db_object->list_owners(0,dir->node,dir->db_object->list_owners_type);
	/* clear out object/owners cache */
	obj_reset_owners();
    }

    if (expanded)
	gtk_ctree_expand(dir->tree,dir->node);
    gtk_clist_thaw(GTK_CLIST(obj_mainwin.tree1));
    free_path_elements(path_elements, num_elements);
}

/* refresh the list of objects of a given type for a particular owner */
void
on_gowner_refresh_activate(
    GtkMenuItem	     *menuitem,
    gpointer	     user_data)
{
    DirNode	     *dir     = (DirNode *)user_data;
    char	     *owner;
    char	     *dbid;
    char	     *dbversion;
    char	     *path_elements[4];
    int		     num_elements;
    int              expanded = GTK_CTREE_ROW (dir->node)->expanded;

    /* Nothing to do if the node has never been expanded */
    if (!dir->scanned)
	return;
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    
    dbversion = obj_cinfo.login->dbversion;

    gtk_clist_freeze(GTK_CLIST(obj_mainwin.tree1));
    /* Remove the old children children */
    gtk_ctree_post_recursive_to_depth(dir->tree, dir->node, GTK_CTREE_ROW (dir->node)->level + 1, 
				     remove_tree_node, dir->node);
    /* re-populate tree under this node */
    dir->db_object->load_user_objects(dir->node, owner, dbversion, dbid);
    if (expanded)
	gtk_ctree_expand(dir->tree,dir->node);

    gtk_clist_thaw(GTK_CLIST(obj_mainwin.tree1));
    free_path_elements(path_elements, num_elements);
}

void
on_root_refresh_activate(
    GtkMenuItem	     *menuitem,
    gpointer	     user_data)
{
    DirNode	     *dir     = (DirNode *)user_data;
    char	     *owner;
    char	     *dbid;
    char	     *dbversion;
    char	     *path_elements[4];
    int		     num_elements;
    GtkCTreeNode     *tnode;
    GList            *list_node;
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    
    dbversion = obj_cinfo.login->dbversion;
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

    gtk_clist_freeze(GTK_CLIST(obj_mainwin.tree1));

    list_node = GTK_CLIST(obj_mainwin.tree1)->selection;
    tnode = list_node->data;
    gtk_ctree_remove_node (GTK_CTREE(obj_mainwin.tree1),tnode);
    obj_do_post_login();

    gtk_clist_thaw(GTK_CLIST(obj_mainwin.tree1));
    free_path_elements(path_elements, num_elements);
}

void
on_root_disconnect_activate(
    GtkMenuItem	     *menuitem,
    gpointer	     user_data)
{
    DirNode	     *dir     = (DirNode *)user_data;
    char	     *owner;
    char	     *dbid;
    char	     *dbversion;
    char	     *path_elements[4];
    int		     num_elements;
    GtkCTreeNode     *tnode;
    GList            *list_node;
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    
    dbversion = obj_cinfo.login->dbversion;
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

    gtk_clist_freeze(GTK_CLIST(obj_mainwin.tree1));

    list_node = GTK_CLIST(obj_mainwin.tree1)->selection;
    tnode = list_node->data;
    gtk_ctree_remove_node (GTK_CTREE(obj_mainwin.tree1),tnode);
    refresh_dbid_combo();

    gtk_clist_thaw(GTK_CLIST(obj_mainwin.tree1));
    free_path_elements(path_elements, num_elements);
}


void
on_otab_create_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show (get_twwinmain_widget());
}

void
on_otablespace_create_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show (create_tablespace());
}


void
on_oindex_create_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show(obj_create_indexwin());
}


void
on_oview_create_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show(obj_create_viewwin());
}

void
on_oview_script_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{

}

static void
free_glist_string_item(
    gpointer list_data,
    gpointer user_data)
{
    g_free(list_data);
}


void
on_oproc_exportall_activate(
    GtkMenuItem	     *menuitem,
    gpointer	     user_data)
{
    DirNode	     *dir     = (DirNode *)user_data;
    char	     *owner;
    char	     *dbid;
    char	     *dirpath;
    char	     *path_elements[4];
    int		     num_elements;
    int              counter=0;
    GList            *owners;
    GList            *o;
    GList            *name;
    GList            *n;
    GList            *code;
    GList            *c;
    char             *query;
    char             *build_dir;

    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    
    dirpath=obj_show_dirbrowser();
    if(dirpath==NULL)
        return;
	
    build_dir = g_malloc(strlen(dirpath) + strlen(dbid) + 2);
    sprintf(build_dir,"%s/%s",dirpath,dbid);
    mkdir(build_dir,755);
    g_free(build_dir);
    
    owners = glist_from_query("select distinct owner from all_source",dbid, 0, TRUE, active_step);
    active_clear();

    for (o = owners; o; o = g_list_next(o)) {
        build_dir = g_malloc(strlen(dirpath) + strlen(dbid) + strlen(o->data) + 3);
	sprintf(build_dir,"%s/%s/%s",dirpath,dbid,(char *)o->data);
	mkdir(build_dir,755);
	g_free(build_dir);
	
	query = g_malloc(strlen(o->data) + 60);
	sprintf(query, "select distinct name from all_source where owner = '%s'",(char *)o->data);
	
	while (gtk_events_pending())
	    gtk_main_iteration();
	
	name = glist_from_query(query,dbid, 0, TRUE, active_step);
	active_clear();
	g_free(query);
	
	for (n = name; n; n = g_list_next(n)) {
	    FILE    *stream;
	    build_dir = g_malloc(strlen(dirpath) + strlen(dbid) + strlen(o->data) + strlen(n->data) + 4);
	    sprintf(build_dir,"%s/%s/%s/%s",dirpath,dbid,(char *)o->data,(char *)n->data);
	    stream = fopen(build_dir,"w");
	    if(!stream)
	        break;
		
	    query = g_malloc(strlen(o->data) + strlen(n->data) + 80);
	    sprintf(query, "select text from all_source where owner = '%s' and name = '%s' order by line",(char *)o->data,(char *)n->data);
	    
	    while (gtk_events_pending())
	        gtk_main_iteration();
	    
	    code = glist_from_query(query,dbid, 0, TRUE, active_step);
	    active_clear();
	    
	    g_free(query);
	    
	    for (c = code; c; c = g_list_next(c)){
	        counter++;
	        fprintf(stream,c->data);
		active_step(counter);
		while (gtk_events_pending())
	            gtk_main_iteration();
	    }
	    
	    active_clear();
	    counter=0;
	    
	    fclose(stream);
	    g_free(build_dir);
            g_list_foreach (code, free_glist_string_item, 0);
            g_list_free (code);	    
	}
	
	g_list_foreach (name, free_glist_string_item, 0);
        g_list_free (name);
        
    }
    
    g_list_foreach (owners, free_glist_string_item, 0);
    g_list_free (owners);
}


