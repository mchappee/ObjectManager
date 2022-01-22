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
#include "include/objectman.h"
#include "objects/indexes/indexfuncs.h"
#include "objects/tables/tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

static GdkPixmap *folder_pixmap=NULL;
static GdkBitmap *folder_mask;

#include <xpm/indexicon.xpm>

void 
loaduserindexes(
    GtkCTreeNode  *parent_node,
    char	  *owner,
    const char	  *dbversion,
    const char	  *dbid)
{

    static char	  qtemplate[] = "SELECT index_name FROM all_indexes WHERE owner	= '%s' "
                                "order by index_name";
    DirNode	  *dirnode, *parent_dirnode;
    gchar	  *text;
    GtkCTreeNode  *node;
    char          *query;
    GList         *owners;
    GList         *o;

    query = g_malloc(strlen(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    
    owners = glist_from_query(query, dbid, 0, TRUE, active_step);
    active_clear();
    g_free(query);
    
    if(owners==NULL)
        return;
    
    if (! folder_pixmap)
	folder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, indexpixm);
    
    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    for (o = owners; o; o = g_list_next(o)) {

	dirnode = new_dirnode(parent_dirnode, o->data);
	dirnode->menu_creator = create_indexmenu;
	text = o->data;

        node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4,
                                     folder_pixmap, folder_mask, NULL, NULL, FALSE, FALSE);

        gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);

        g_free(o->data);
    }
    
    g_list_free (owners);

}

void
get_index_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *index_name,
    DirNode	     *dir)
{
    char	*query;
    GtkCList    *list;
    GtkWidget   *clist;
    GList       *owners;
    GList       *o;
    int         i=0;
    char        *clist_xfer[2];
    static char  qinfotemplate8[] = 
	"SELECT "
	"'Index Type:'    , index_type      , "
	"'Table Owner:'   , table_owner     , "
	"'Table Name:'    , table_name      , "
	"'Table Type:'    , table_type      , "
	"'Uniqueness'     , uniqueness      , "
	"'Tablespace:'    , tablespace_name , "
	"'Initial Trans:' , ini_trans       , "
	"'Max Trans:'     , max_trans       , "
	"'Initial Extent:', initial_extent  , "
	"'Next Extent:'   , next_extent     , "
	"'Min Extents:'   , min_extents     , "
	"'Max Extents:'    , max_extents     , "
	"'%% Increase:'   , pct_increase    , "
	"'Status:'        , status          , "
	"'Buffer Pool:'   , buffer_pool       "
	"FROM   all_indexes "
	"WHERE  index_name = '%s'"
	"       AND owner = '%s'";

    static char  qinfotemplate7[] = 
	"SELECT "
	"'Table Owner:'   , table_owner     , "
	"'Table Name:'    , table_name      , "
	"'Table Type:'    , table_type      , "
	"'Uniqueness'     , uniqueness      , "
	"'Tablespace:'    , tablespace_name , "
	"'Initial Trans:' , ini_trans       , "
	"'Max Trans:'     , max_trans       , "
	"'Initial Extent:', initial_extent  , "
	"'Next Extent:'   , next_extent     , "
	"'Min Extents:'   , min_extents     , "
	"'Max Extents:'    , max_extents     , "
	"'%% Increase:'   , pct_increase    , "
	"'Status:'        , status           "
	"FROM   all_indexes "
	"WHERE  index_name = '%s'"
	"       AND owner = '%s'";

    static char  qcolstemplate[] =
	"SELECT column_name "
	"FROM  all_ind_columns "
	"WHERE index_name = '%s'"
	"      AND index_owner = '%s'";

    clist = gtk_clist_new(2);

    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_index_toolbar(dir);
    obj_ginfo.toolbar_owner = OMDB_Index;

    query = g_malloc(sizeof(qinfotemplate8) + strlen(owner) + 
		     strlen(index_name) + 1);
		     
    if(dbversion)  /* Oracle 8 */
        sprintf(query, qinfotemplate8, index_name, owner);
    else           /* Oracle 7 */
        sprintf(query, qinfotemplate7, index_name, owner);

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    while (i < 29){
        char *clist_xfer[2];
        gtk_clist_get_text(GTK_CLIST(list), 0, i, &clist_xfer[0]);
        gtk_clist_get_text(GTK_CLIST(list), 0, i+1, &clist_xfer[1]);
	gtk_clist_append( (GtkCList *) clist, clist_xfer);
	i += 2;
    }

    query = g_malloc(sizeof(qcolstemplate) + strlen(owner) + 
		     strlen(index_name) + 1);
    sprintf(query, qcolstemplate, index_name, owner);
    
    owners = glist_from_query(query, dbid, 0, TRUE, active_step);
    active_clear();
    g_free(query);
    
    if(owners==NULL)
        return;

    clist_xfer[0]=NULL;
    clist_xfer[1]=NULL;
    gtk_clist_append( (GtkCList *) clist, clist_xfer);
    clist_xfer[0]="Indexed Columns";
    clist_xfer[1]=NULL;
    gtk_clist_append( (GtkCList *) clist, clist_xfer);

    for (o = owners; o; o = g_list_next(o)) {
	clist_xfer[0]=NULL;
	clist_xfer[1]=o->data;
        gtk_clist_append( (GtkCList *) clist, clist_xfer);
        g_free(o->data);
    }

    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_doindexview(clist);

    gtk_widget_destroy(GTK_WIDGET(list));
    g_list_free (owners);

}

void
show_generic_index_window(
    const char *dbversion,
    const char *dbid,
    const char *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;

    static char qtemplate8[] = 
	"SELECT index_name \"Name\", index_type \"Type\", "
	"       table_owner || '.' || table_name \"On table\" "
	"FROM   all_indexes "
	"WHERE  owner = '%s' order by index_name";

    static char qtemplate7[] = 
	"SELECT index_name \"Name\", "
	"       table_owner || '.' || table_name \"On table\" "
	"FROM   all_indexes "
	"WHERE  owner = '%s' order by index_name";

    static char	     tabtemplate[] = "Summary of indexes owned by %s";

    query = g_malloc(sizeof(qtemplate8) + strlen(owner) + 1);
    
    if(dbversion)
        sprintf(query, qtemplate8, owner);
    else
        sprintf(query, qtemplate7, owner);
	
    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    notebook_tab = g_malloc(sizeof(tabtemplate) + strlen(owner) + 1);
    sprintf(notebook_tab, tabtemplate, owner);

    gtk_widget_destroy(obj_mainwin.notebook1);
    glist_set_opt_col_width(list, 0);
    glist_set_opt_col_width(list, 1);

    obj_do_generic_view(list, notebook_tab);
    g_free(notebook_tab);
}

