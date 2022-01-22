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
/* This file was borrowed from xmms (http://www.xmms.org),
   the best multimedia program in the world */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <oci.h>
#include "include/objectman.h"
#include "objects/tables/tablefuncs.h"
#include "objects/tablespaces/tablespacefuncs.h"

#include <sys/time.h>

GtkWidget *playlistwin_dir_browser,playlistwin;
void playlistwin_add_dir_handler(DirNode *);

static GdkPixmap *folder_pixmap=NULL, *ofolder_pixmap;
static GdkBitmap *folder_mask, *ofolder_mask;

#include <xpm/foldericon.xpm>
#include <xpm/ofoldericon.xpm>
#include <xpm/usericon.xpm>

void
destroy_cb(
    gpointer data)
{
    DirNode *node = data;

    if (node->menu) {
	gtk_widget_destroy(GTK_WIDGET(node->menu));
    }
    g_free(node->path);
    memset(node, '5', sizeof(*node));
    g_free(node);
}

void
set_dbid(
    const char *rnode)
{
    obj_logininfo    *login;

    for(login = obj_logins; login; login = login->next) {
	if(strcmp(rnode,login->dbid)==0){
	    obj_cinfo.login = login;
	    break;
	}
    }
    
    gtk_signal_handler_block_by_func (GTK_OBJECT (GTK_COMBO (obj_mainwin.toolbarcombo1)->entry),
        GTK_SIGNAL_FUNC (on_dbid_combo_changed),NULL);
    gtk_entry_set_text(GTK_ENTRY(GTK_COMBO (obj_mainwin.toolbarcombo1)->entry),
        obj_cinfo.login->dbid);
    gtk_signal_handler_unblock_by_func (GTK_OBJECT (GTK_COMBO (obj_mainwin.toolbarcombo1)->entry),
        GTK_SIGNAL_FUNC (on_dbid_combo_changed),NULL);
    /* We shouldn't fall out of the loop without finding the login */
    assert(login);
}

static void
collapse_cb(
    GtkWidget * widget, 
    GtkCTreeNode * parent_node)
{
}

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

static void
expand_cb(
    GtkWidget	  *widget,
    GtkCTreeNode  *parent_node)
{
    DirNode	  *dir;
    char	  *dbid;
    char	  *dbversion;

    dir = gtk_ctree_node_get_row_data(GTK_CTREE(widget), parent_node);
    obj_cinfo.current_node = parent_node;

    if(dir->expand_callback) {
	dir->expand_callback(dir, widget, parent_node);
    } else if(dir->scanned==FALSE){
	char *path_elements[4];
	int num_elements = parse_path(dir->path, path_elements, 4);
	
        if (num_elements >= 2)
	  obj_getctype(path_elements[DIRPATH_TYPE]);

	if (num_elements >= 3) {
	    set_dbid(path_elements[DIRPATH_DBID]);
	    assert(dir->db_object);
	    gtk_clist_freeze(GTK_CLIST(widget));
	    /* Remove the dummy child which was inserted so that the
	       expand/collapse box is displayed */
	    gtk_ctree_pre_recursive_to_depth(GTK_CTREE(widget), parent_node, 
					     GTK_CTREE_ROW (parent_node)->level + 1, 
					     remove_tree_node, parent_node);

	    dbid = path_elements[DIRPATH_DBID];
	    dbversion = obj_cinfo.login->dbversion;
	    dir->db_object->load_user_objects(parent_node, path_elements[DIRPATH_OWNER],
					      dbversion, dbid);
	    gtk_clist_thaw(GTK_CLIST(widget));

	}
	free_path_elements(path_elements, num_elements);
    }
}

void
on_ctree1_tree_select_row (
    GtkCTree	  *ctree,
    GList	  *node,
    gint	  column,
    gpointer	  user_data)
{
    GtkCTreeNode  *tnode;
    DirNode	  *dirnode=NULL;
    GList	  *list_node=NULL;
    void	  (*handler) (DirNode *);
    int           num_elements;
    char          *dbid;
    char	  *path_elements[4];

    gtk_signal_handler_block_by_func (GTK_OBJECT (obj_mainwin.tree1),
        GTK_SIGNAL_FUNC (on_ctree1_tree_select_row),NULL);    

    list_node = GTK_CLIST(ctree)->selection;
    if(list_node){
        while (list_node)
   	    {
	        tnode = list_node->data;
	        dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(ctree), tnode);
	        handler = (void (*)(DirNode *)) gtk_object_get_user_data(GTK_OBJECT(ctree));
	        if (handler)
		    handler(dirnode);
	        list_node = g_list_next(list_node);
	    }
	
        if(dirnode != NULL){
            num_elements = parse_path(dirnode->path, path_elements, 4);
            dbid = path_elements[DIRPATH_DBID];
            set_dbid(dbid);
            free_path_elements(path_elements, num_elements);
        }

    }
   
    gtk_signal_handler_unblock_by_func (GTK_OBJECT (obj_mainwin.tree1),
        GTK_SIGNAL_FUNC (on_ctree1_tree_select_row),NULL);

}

static void
unselect_row_cb(
    GtkWidget	    * widget,
    gint	    row, 
    gint	    column, 
    GdkEventButton  * bevent, 
    gpointer	    data)
{
    obj_cinfo.currentobject[0]=0;
    obj_cinfo.currenttype=OMDB_None;
    if (obj_ginfo.toolbar_owner != OMDB_None){
	gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
	obj_create_blank_toolbar();
	obj_ginfo.toolbar_owner=OMDB_None;
    }
}

static void
select_row_cb(
    GtkWidget	    * widget,
    gint	    row, 
    gint	    column, 
    GdkEventButton  * bevent, 
    gpointer	    data)
{
    DirNode	    *dirnode;
    GtkCTreeNode    *node;
    void	    (*handler) (DirNode *);
    
    
    if (bevent) {       
	if (bevent->type == GDK_2BUTTON_PRESS) {
	    node = gtk_ctree_node_nth(GTK_CTREE(widget), row);
	    dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(widget), node);
	    handler = (void (*)(DirNode *)) gtk_object_get_user_data(GTK_OBJECT(widget));
	    if (handler)
		handler(dirnode);
	}
    }
}

void 
playlistwin_add_dir_handler(
    DirNode	 *dir)
{
    char	 *owner;
    char	 *objectname;
    const char	 *dbid;
    const char	 *dbversion=NULL;
    char	 *path_elements[4];
    char	 *statusobject;

    if(dir){
        int          num_elements = parse_path(dir->path, path_elements, 4);

        dbid = path_elements[DIRPATH_DBID];
        owner = path_elements[DIRPATH_OWNER];
        objectname = path_elements[DIRPATH_OBJECT];

        if (num_elements >= 2){
	    obj_getctype(path_elements[DIRPATH_TYPE]);
	    set_dbid(dbid);
	    dbversion = obj_cinfo.login->dbversion;
        }
	
        if (num_elements == 3 && dir->db_object->general_info_func)
            dir->db_object->general_info_func(dbversion, dbid, owner);

        if (num_elements == 4) {
	    sprintf(obj_cinfo.currentobject,"%s.%s",owner,objectname);
	
	    statusobject=g_malloc(strlen(obj_cinfo.currentobject)+30);
	    sprintf(statusobject,"Current Object: %s",obj_cinfo.currentobject);
	
            gtk_statusbar_push( GTK_STATUSBAR(obj_mainwin.statusbar1),
	        gtk_statusbar_get_context_id(GTK_STATUSBAR(obj_mainwin.statusbar1),""), statusobject);

	    sprintf(obj_cinfo.currentobjectowner,"%s",owner);
	    sprintf(obj_cinfo.currentobject_noowner,"%s",objectname);
   
	    dir->db_object->object_info_func(dbversion, dbid, owner, objectname, dir);
        }
    
        free_path_elements(path_elements, num_elements);
    }
}

gboolean
on_dir_button_press(
    GtkWidget	    *widget,
    GdkEventButton  *event,
    gpointer	    user_data)
{
    gint	    row;
    gint	    column;
    GtkCTreeNode    *node;
    DirNode	    *dir=NULL;
    
    if (event->type == GDK_BUTTON_PRESS &&
	event->button == 3) {
	gtk_clist_get_selection_info (GTK_CLIST(widget),
				      event->x,
				      event->y,
				      &row,
				      &column);
	if (row >= 0) {
	    node = gtk_ctree_node_nth(GTK_CTREE(widget), row);
	    dir = gtk_ctree_node_get_row_data(GTK_CTREE(widget), node);

	    if (dir) {
	        if (dir->menu){
		    gtk_widget_destroy(GTK_WIDGET(dir->menu));
		    dir->menu = 0;
	        }
	        /* Make these available for the menu activation callback */
	        dir->tree = GTK_CTREE(widget);
	        dir->node = node;
	        dir->row = row;
	        if (dir->menu_creator) {
		    dir->menu = GTK_MENU(dir->menu_creator(dir));
	 	    gtk_menu_popup(dir->menu, NULL, NULL, NULL, NULL,
                               event->button, event->time);
	        }
	    }
	}
    }
    return FALSE;
}

GtkWidget *
obj_create_dir_browser(
    gchar * title,
    gchar * current_path,
    GtkSelectionMode mode, 
    void (*handler) (DirNode *))
{
    gchar *root_text;
    GtkCTreeNode *root_node;
    DirNode *dirnode;
 
    gtk_widget_realize(obj_mainwin.window1);
	
    root_text = obj_cinfo.login->dbid;
	
    if (!folder_pixmap)
	folder_pixmap =
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, folder);
    if (!ofolder_pixmap)
	ofolder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &ofolder_mask, NULL, ofolder);

    if(obj_mainwin.tree1 == NULL){
	obj_mainwin.tree1 = gtk_ctree_new(1, 0);
	gtk_clist_set_selection_mode (GTK_CLIST( obj_mainwin.tree1), GTK_SELECTION_SINGLE);
	gtk_signal_connect(GTK_OBJECT (obj_mainwin.tree1), "button-press-event",
			   GTK_SIGNAL_FUNC (on_dir_button_press),
			   NULL);

	gtk_clist_set_column_auto_resize(GTK_CLIST(obj_mainwin.tree1), 0, TRUE);
	gtk_clist_set_selection_mode(GTK_CLIST(obj_mainwin.tree1), mode);
	gtk_ctree_set_line_style(GTK_CTREE(obj_mainwin.tree1), GTK_CTREE_LINES_DOTTED);
	gtk_signal_connect(GTK_OBJECT(obj_mainwin.tree1), "tree_expand", GTK_SIGNAL_FUNC(expand_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(obj_mainwin.tree1), "tree_collapse", GTK_SIGNAL_FUNC(collapse_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(obj_mainwin.tree1), "select_row", GTK_SIGNAL_FUNC(select_row_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(obj_mainwin.tree1), "unselect_row", GTK_SIGNAL_FUNC(unselect_row_cb), NULL);
	gtk_signal_connect (GTK_OBJECT (obj_mainwin.tree1), "tree_select_row",GTK_SIGNAL_FUNC (on_ctree1_tree_select_row),NULL);
	gtk_container_add(GTK_CONTAINER(obj_mainwin.scrolledwindow1), obj_mainwin.tree1);
	gtk_object_set_user_data(GTK_OBJECT(obj_mainwin.tree1), (gpointer) handler);
	gtk_widget_set_usize (obj_mainwin.scrolledwindow1, 250, 600);
    }

    root_node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), NULL, NULL, &root_text, 4, 
				      folder_pixmap, folder_mask, ofolder_pixmap, ofolder_mask, FALSE, FALSE);
    dirnode = new_dirnode(NULL, NULL);
    dirnode->path = g_strdup(root_text);
    dirnode->scanned = FALSE;
    dirnode->menu_creator = create_root_menu;
    gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), root_node, dirnode, destroy_cb);

    gtk_ctree_expand(GTK_CTREE(obj_mainwin.tree1),root_node);
    gtk_widget_show(obj_mainwin.tree1);

    obj_create_nodes(root_node);
	
    return obj_mainwin.window1;
}

void playlistwin_show_dirbrowser()
{
    playlistwin_dir_browser = 
	obj_create_dir_browser(" ", user_home_dir(), 
			       GTK_SELECTION_SINGLE, 
			       playlistwin_add_dir_handler);

}

/* Where we store object/owner info */
/* obj_owners is indexed by object type, 
   each element is a list of owners */

static GHashTable *obj_owners = 0;

static char *oracleTypes[] = {
    /* 0 */ "NEXT OBJECT", 
    /* 1 */ "INDEX", 
    /* 2 */ "TABLE", 
    /* 3 */ "CLUSTER",
    /* 4 */ "VIEW", 
    /* 5 */ "SYNONYM", 
    /* 6 */ "SEQUENCE", 
    /* 7 */ "PROCEDURE", 
    /* 8 */ "FUNCTION", 
    /* 9 */ "PACKAGE",
    /* 10 */ 0,
    /* 11 */ "PACKAGE BODY", 
    /* 12 */ "TRIGGER",
    /* 13 */ "TYPE", 
    /* 14 */ "TYPE BODY", 
    /* 15-18 */ 0, 0, 0, 0,
    /* 19 */ "TABLE PARTITION", 
    /* 20 */ "INDEX PARTITION", 
    /* 21 */ 0,
    /* 22 */ "LIBRARY", 
    /* 23 */ "DIRECTORY", 
    /* 24 */ "QUEUE",
    /* 25-27 */ 0, 0, 0,
    /* 28 */ "JAVA SOURCE", 
    /* 29 */ "JAVA CLASS", 
    /* 30 */ "JAVA RESOURCE",
    /* 31 */ 0,
    /* 32 */ "INDEXTYPE", 
    /* 33 */ "OPERATOR",
    /* 34 */ "TABLE SUBPARTITION", 
    /* 35 */ "INDEX SUBPARTITION", 
    /* 36-38 */ 0, 0, 0,
    /* 39 */ "LOB PARTITION", 
    /* 40 */ "LOB SUBPARTITION",
    /* 41,42 */ 0, 0,
    /* 43 */ "DIMENSION",
    /* 44 */ "CONTEXT", 
    /* 45,46 */ 0, 0,
    /* 47 */ "RESOURCE PLAN", 
    /* 48 */ "CONSUMER GROUP"};


/* See query below, values[0] is object type, values[1] is owner and
   values[2] is the name of the object */
static int
obj_rowcallback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *indicators, 
    char	     **values, 
    ub2		     *values_l, 
    short	     *types, 
    int              (*rowcallback)(int),
    gpointer	     user_data)
{
    GList      *owners;
    int        i;
    int        typecode = -1;

    if( row >= 0 ){
	for (i = 0; i < sizeof(oracleTypes)/sizeof(oracleTypes[0]); i++)
	    if (oracleTypes[i] && strcmp(oracleTypes[i], values[0]) == 0) {
		typecode = i;
		break;
	    }
	assert(typecode > 0);

	owners = (GList *) g_hash_table_lookup (obj_owners, oracleTypes[typecode]);
	if (!owners) {
	    g_hash_table_insert (obj_owners, oracleTypes[typecode], g_list_append(0, g_strdup(values[1])));
	} else {
	    g_list_append(owners, g_strdup(values[1]));
	}

	if(obj_progwin.window4){
	    gtk_progress_bar_update( GTK_PROGRESS_BAR(obj_progwin.progressbar1),
				     (double)typecode/13.0);
	}
	while (gtk_events_pending())
	    gtk_main_iteration();

    }

    return TRUE;
}

void
obj_getowners(
    GtkCTreeNode  *rootnode,
    GtkCTreeNode  *topnode,
    char	  *objtype)
{
    GString	  *query;
    GList	  *owners = 0;
    GList	  *o;
    GtkCTreeNode  *node;
    GtkCTreeNode  *objectnode;
    char	  *dummy = "dummy";
    DirNode	  *dirnode, *parent_dirnode;
    gchar	  *text;


    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), topnode);
    if (!folder_pixmap)
	folder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, user);

    if (!obj_owners) {
	query = g_string_new("SELECT distinct object_type, owner "
			     " FROM all_objects"
			     " WHERE object_type in ('INDEX', 'TABLE', 'VIEW', "
			     "                       'SYNONYM', 'SEQUENCE', "
			     "                       'PROCEDURE', 'FUNCTION', 'PACKAGE',"
			     "                       'PACKAGE BODY', 'TRIGGER', 'TYPE',"
                             "                       'TABLE PARTITION', 'TABLE SUBPARTITION') ");

	if(!obj_cinfo.login->allobjects) {
	    g_string_sprintfa(query, " AND owner = '%s'", 
			      strupr(obj_cinfo.login->owner));
	}
	g_string_append(query, " ORDER BY object_type");

	obj_owners = g_hash_table_new(g_str_hash, g_str_equal);
	oracle_do_query(query->str, obj_cinfo.login->dbid, 0, (void *)obj_rowcallback, NULL, NULL);
	/*do_query(query->str, obj_cinfo.login->dbid, 0, obj_rowcallback, 0);*/
	g_string_free(query, 1);
    }

    objtype=g_strdup(objtype);
    strupr(objtype);
    owners = (GList *) g_hash_table_lookup (obj_owners, objtype);
    
    if (owners) {
	for (o = owners; o; o = g_list_next(o)) {
	    
	    dirnode = new_dirnode(parent_dirnode, o->data);
	    dirnode->menu_creator = create_generic_owner_menu;
	    text = o->data;
	    
	    node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), topnode, NULL, &text, 4, 
					 folder_pixmap, folder_mask, ofolder_pixmap, ofolder_mask, FALSE, FALSE);
	    
	    gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	    
	    objectnode = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), node, NULL, &dummy, 4, 
					       NULL, NULL, NULL, NULL, FALSE, FALSE);
	} 
    }
    g_free(objtype);
}

void
obj_gettablespaces(
    GtkCTreeNode  *rootnode,
    GtkCTreeNode  *topnode,
    char	  *objtype)
{
    GList	  *tablespaces = 0;
    GList	  *t;
    GtkCTreeNode  *node;
    DirNode	  *dirnode, *parent_dirnode;
    gchar	  *text;

    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), topnode);
    if (!folder_pixmap)
	folder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, user);

    /* possibly not the best way to check permissions but assume that
       if the dba tablespace view isn't visible to the current user via
       all_views we don't have permission to look at it */
    if (!glist_from_query("SELECT view_name from all_views "
			  "where view_name = 'DBA_TABLESPACES' "
			  "and owner = 'SYS'",
			  obj_cinfo.login->dbid, 0, TRUE, 0)) {
	gtk_ctree_node_set_selectable(GTK_CTREE(obj_mainwin.tree1),topnode, FALSE);
	return;
    }

    tablespaces = glist_from_query("SELECT tablespace_name from dba_tablespaces",
				   obj_cinfo.login->dbid, 0, TRUE, 0);
				   

    if (tablespaces) {
	for (t = tablespaces; t; t = g_list_next(t)) {
	    
	    dirnode = new_dirnode(parent_dirnode, t->data);
	    dirnode->menu_creator = create_otsmenu;
	    text = t->data;
	    
	    node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), topnode, NULL, &text, 4, 
					 folder_pixmap, folder_mask, ofolder_pixmap, ofolder_mask, 
					 TRUE, FALSE);
	    
	    gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	} 
    }
    g_free(objtype);
}

void
obj_reset_owners(void)
{
    /* FIXME FIXME - what about the lists and the strings they contain !!! */
    g_hash_table_destroy(obj_owners);
    obj_owners = 0;
}

DirNode *
new_dirnode(
    DirNode *parent,
    const char *name)
{
    DirNode *new_dirnode = g_malloc0(sizeof (DirNode));

    if (parent) {
	new_dirnode->path = g_malloc(strlen(parent->path) + strlen(name) + 10);
	sprintf(new_dirnode->path,"%s->%s",parent->path,name);
	new_dirnode->db_object = parent->db_object;
    }
    new_dirnode->scanned = FALSE;
    return new_dirnode;
}

/* These should probably be in a separate file */
/* Parse out an object path into components */
int
parse_path(
    const char *path,
    char **elements,
    int max_elements)
{
    int  elt_count = 0;

    if (path && *path && (max_elements > 0)) {
	char *p = g_strdup(path);
	char *s = p;

	elements[0] = p;
	++elt_count;

	while (*s && elt_count < max_elements) {
	    /* if object names are quoted they might contain "-" */
	    /* they might also contain "->" so we should do this
	       whole thing in a better way */
	    if (*s == '-') {
		if (*++s == '>') {
		    *(s-1) = '\0';
		    if (*++s == '\0')
			g_error("truncated path\n");
		    elements[elt_count] = s;
		    ++elt_count;
		    ++s;
		}
	    } else {
		++s;
	    }
	}
    } else {
	elements[0] = 0;	
    }
    return elt_count;
}

/* free storage allocated by parse_path */
void
free_path_elements(
    char **elements,
    int num_elements)
{
    g_free(elements[0]);
}

