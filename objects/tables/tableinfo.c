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
#include "common-files/common.h"
#include "include/objectman.h"
#include "tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <xpm/objecticon.xpm>
static GdkPixmap *folder_pixmap=NULL;
static GdkBitmap *folder_mask;

void 
loadusertables(
    GtkCTreeNode  *parent_node,
    char	  *owner, 
    const char	  *dbversion,
    const char	  *dbid)
{
    char	  *query;
    GtkCTreeNode  *node;
    DirNode	  *dirnode;
    DirNode	  *parent_dirnode;
    char	  *text;
    GList	  *tables;
    GList	  *p;
    char	  qtemplate[] = 
	"SELECT table_name "
	"FROM all_tables "
	"WHERE owner = '%s' order by table_name";


    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    tables = glist_from_query(query, dbid, 0, TRUE, active_step);
    active_clear();
    g_free(query);

    if (!folder_pixmap)
	folder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, 
						     &folder_mask, NULL, object);

    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    for (p = tables; p; p = g_list_next(p)) {
	dirnode = new_dirnode(parent_dirnode, p->data);
	dirnode->menu_creator = create_tablemenu;

	text = p->data;
	node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4, 
				     folder_pixmap, folder_mask, NULL, NULL, TRUE, TRUE);
		
	gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	g_free(p->data);

    }
    g_list_free (tables);
}


void
get_table_info(
    const char	 *dbversion,
    const char	 *dbid,
    const char	 *owner,
    const char	 *table_name,
    DirNode *dir)
{
    char	 *cache;
    char	 *query;
    int          i=0;
    int          clistcount;
    GtkCList     *list;
    GtkCList     *cols_list;
    GtkCList     *clist2;
    GtkWidget    *clist4;
    GtkCList     *clist5;
    static char qindextemplate[] = 
	"SELECT index_name, uniqueness "
	"FROM   all_indexes "
	"WHERE  table_name = '%s'"
	"       AND owner = '%s'";

#if 0
    /* this query seems to be faster, but is not supported by 8.0.5 */
    static char qcoltemplate[] = 
	"SELECT    t.column_name \"Name\", "
	"          (select comments from all_col_comments "
	"           where table_name=t.table_name "
	"       	  and column_name = t.column_name "
        "       	  and owner = t.owner) \"Comment\", "
	"          t.data_type \"Type\", "
	"          t.data_length \"Length\", "
	"          t.nullable \"Nullable\", "
	"          t.data_precision \"Precision\", "
	"          t.data_scale \"Scale\", "
	"          t.data_default \"Default Value\" "
	"FROM      all_tab_columns t "
	"WHERE     t.table_name = '%s'"
	"          AND t.owner = '%s'"
	" ORDER BY t.column_id";
#endif

    static char qcoltemplate[] = 
	"SELECT    t.column_name \"Name\", "
	"          c.comments \"Comments\", "
	"          t.data_type \"Type\", "
	"          t.data_length \"Length\", "
	"          t.nullable \"Nullable\", "
	"          t.data_precision \"Precision\", "
	"          t.data_scale \"Scale\", "
	"          t.data_default \"Default Value\" "
	"FROM      all_tab_columns t, all_col_comments c "
	"WHERE     t.table_name = '%s'"
	"          AND t.owner = '%s'"
	"          AND c.table_name = t.table_name "
       	"          AND c.column_name = t.column_name "
       	"          AND c.owner = t.owner "
	" ORDER BY t.column_id";

    static char qinfotemplate8[] = 
	"SELECT '%% Free:'           ,NVL(pct_free, 0)          ,'%% Used:'       ,NVL(pct_used, 0), "
	"       'Min Extents:'       ,NVL(min_extents, 0)       ,'Max Extents:'   ,NVL(max_extents, 0), "
	"       'Initial Extent:'   ,NVL(initial_extent, 0)    ,'Next Extent:'   ,NVL(next_extent, 0), "
	"       'Logging:'       ,NVL(logging, '?'), "
	"       'Cluster Name:'      ,NVL(cluster_name, 'none') ,'IOT Name:'      ,NVL(iot_name, 'none'), "
	"       '%% Increase:'       ,NVL(pct_increase, 0)      ,'Blocks:'        ,NVL(blocks, 0), "
	"       'Instances:'         ,NVL(instances, 'none'), "
	"       'Cache:'             ,NVL(ltrim(cache), 'none')        ,'Buffer Pool:'   ,NVL(buffer_pool, 'none'), "
	"       'Init Trans:'        ,NVL(ini_trans, 0)         ,'Max Trans:'     ,NVL(max_trans, 0), "
	"       'Avg Space:'         ,NVL(avg_space, 0)         ,'Chain Count:'   ,NVL(chain_cnt, 0), "
	"       'Nested:'            ,NVL(nested, 'none')       ,'Last Analyzed:' ,NVL(to_char(last_analyzed,'MM-DD-YYYY'),'Not Analyzed') "
	" FROM  all_tables"
	" WHERE table_name = '%s'"
	"       AND owner = '%s'";

    static char qinfotemplate7[] = 
	"SELECT '%% Free:'           ,NVL(pct_free, 0)          ,'%% Used:'       ,NVL(pct_used, 0), "
	"       'Min Extents:'       ,NVL(min_extents, 0)       ,'Max Extents:'   ,NVL(max_extents, 0), "
	"       'Initial Extent:'   ,NVL(initial_extent, 0)    ,'Next Extent:'   ,NVL(next_extent, 0), "
	"       'Cluster Name:'      ,NVL(cluster_name, 'none') , "
	"       '%% Increase:'       ,NVL(pct_increase, 0)      ,'Blocks:'        ,NVL(blocks, 0), "
	"       'Instances:'         ,NVL(instances, 'none'), "
	"       'Cache:'             ,NVL(ltrim(cache), 'none')        , "
	"       'Init Trans:'        ,NVL(ini_trans, 0)         ,'Max Trans:'     ,NVL(max_trans, 0), "
	"       'Avg Space:'         ,NVL(avg_space, 0)         ,'Chain Count:'   ,NVL(chain_cnt, 0) "
	" FROM  all_tables"
	" WHERE table_name = '%s'"
	"       AND owner = '%s'";

    static char qconstraints[] = "select constraint_name, constraint_type, status, deferred "
				 "from all_constraints "
				 "where table_name = '%s' "
				 "and owner = '%s' "
				 "order by status";

    clist4 = gtk_clist_new(2);

    query = g_malloc(sizeof(qcoltemplate) + strlen(owner) + strlen(table_name) + 1);
    sprintf(query, qcoltemplate, table_name, owner);
    cols_list = clist_from_query(query, dbid, 0, "", NULL);
    g_free(query);

    if (cols_list == 0 || cols_list->rows == 0) {
	show_warning_msg("Can't find column list for table %s.%s\n\n"
			 "This usually means that the table has been dropped\n"
			 "from another session since the list of tables was\n"
			 "obtained", owner, table_name);
	return;
    }
    
    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_table_toolbar(dir, dbversion);
    obj_ginfo.toolbar_owner = OMDB_Table;

    query = g_malloc(sizeof(qindextemplate) + strlen(owner) + strlen(table_name) + 1);
    sprintf(query, qindextemplate, table_name, owner);

    clist2 = clist_from_query(query, (char *)dbid, 0, "<null>", NULL);
    g_free(query);

    query = g_malloc(sizeof(qinfotemplate8) + strlen(owner) + strlen(table_name) + 1);
    
    if(dbversion){  /* Oracle 8 */
        sprintf(query, qinfotemplate8, table_name, owner);
	clistcount = 39;
    }else{           /* Oracle 7 */
        sprintf(query, qinfotemplate7, table_name, owner);
	clistcount = 29;
    }

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    while (i < clistcount){
        char *clist_xfer[2];
        gtk_clist_get_text(GTK_CLIST(list), 0, i, &clist_xfer[0]);
        gtk_clist_get_text(GTK_CLIST(list), 0, i+1, &clist_xfer[1]);
	gtk_clist_append( (GtkCList *) clist4, clist_xfer);
	i += 2;
    }
     
     
    query = g_malloc(sizeof(qconstraints) + strlen(owner) + strlen(table_name) + 1);
    sprintf(query, qconstraints, table_name, owner);
    clist5 = clist_from_query(query, dbid, 0, "", NULL);
    g_free(query);
    
    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_dotableview(cols_list, GTK_WIDGET(clist2), clist4, GTK_WIDGET(clist5));
    
    gtk_widget_destroy(GTK_WIDGET(list));

    if(dbversion)  /* Oracle 8 */
        gtk_clist_get_text(GTK_CLIST(clist4), 12, 1, &cache);
    else
        gtk_clist_get_text(GTK_CLIST(clist4), 10, 1, &cache);
    
    gtk_signal_handler_block_by_func (GTK_OBJECT (obj_mainwin.togglecache),
        GTK_SIGNAL_FUNC (obj_on_cache_clicked),dir);
    if(strstr(cache,"N")!=0)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (obj_mainwin.togglecache), FALSE);
    else
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (obj_mainwin.togglecache), TRUE);

    gtk_signal_handler_unblock_by_func (GTK_OBJECT (obj_mainwin.togglecache),
        GTK_SIGNAL_FUNC (obj_on_cache_clicked),dir);

}

void
show_generic_table_window(
    const char *dbversion,
    const char *dbid,
    const char *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;

    static char qtemplate80[] = 
	"SELECT   t.table_name \"Name\", t.tablespace_name \"Tablespace\", "
	"         c.comments \"Comments\" "
	"FROM     all_tables t, all_tab_comments c "
	"WHERE    t.owner = '%s'"
	"         AND t.owner = c.owner(+)"
	"         AND t.table_name = c.table_name(+) "
	"ORDER BY t.table_name";

    static char qtemplate8i[] = 
	"SELECT   t.table_name \"Name\", t.tablespace_name \"Tablespace\", "
	"         (select comments from all_tab_comments"
	"          where table_name = t.table_name and owner = t.owner) \"Comments\" "
	"FROM     all_tables t "
	"WHERE    t.owner = '%s' "
	"ORDER BY t.table_name";

    static char	     tabtemplate[] = "Summary of tables owned by user %s";

    if(dbversion){
        if (strstr(dbversion, "8.1") != 0) {
	    query = g_malloc(sizeof(qtemplate8i) + strlen(owner) + 1);
	    sprintf(query, qtemplate8i, owner);
        } else {
	    query = g_malloc(sizeof(qtemplate80) + strlen(owner) + 1);
	    sprintf(query, qtemplate80, owner);
        }
    }else{
        query = g_malloc(sizeof(qtemplate80) + strlen(owner) + 1);
	sprintf(query, qtemplate80, owner);
    }
    
    /* fprintf(stderr, "%s\n", query); */
    list = clist_from_query(query, dbid, 0, "", NULL);
    g_free(query);

    notebook_tab = g_malloc(sizeof(tabtemplate) + strlen(owner) + 1);
    sprintf(notebook_tab, tabtemplate, owner);

    gtk_widget_destroy(obj_mainwin.notebook1);
    if (list) {
	glist_set_opt_col_width(list, 0);
	glist_set_opt_col_width(list, 1);

	obj_do_generic_view(list, notebook_tab);
    }
    g_free(notebook_tab);
}
