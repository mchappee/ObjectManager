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
#include "objects/views/viewfuncs.h"
#include "objects/tables/tablefuncs.h"
#include "objects/procedures/procfuncs.h"
#include "common-files/common.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>
#include <xpm/viewicon.xpm>

static GdkPixmap *folder_pixmap=NULL;
static GdkBitmap *folder_mask;

void 
loaduserviews(
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
    GList	  *views;
    GList	  *p;
    char	  qtemplate[] = 
	"SELECT view_name "
	"FROM all_views "
	"WHERE owner = '%s' order by view_name";


    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    views = glist_from_query(query, dbid, 0, TRUE, active_step);
    active_clear();
    g_free(query);

    if (!folder_pixmap)
	folder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, 
						     &folder_mask, NULL, viewicon);

    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    for (p = views; p; p = g_list_next(p)) {
	dirnode = new_dirnode(parent_dirnode, p->data);
	dirnode->menu_creator = create_viewmenu;

	text = p->data;
	node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4, 
				     folder_pixmap, folder_mask, NULL, NULL, TRUE, TRUE);
		
	gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	g_free(p->data);

    }
    g_list_free (views);
}

void
get_view_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *viewname,
    DirNode	     *dir)
{
    char	     *query;
    GList            *viewtext;
    GtkCList         *cols_list;
    char             *build_view;
    static char      qtexttemplate[] = 
	"SELECT text "
	"FROM  all_views "
	"WHERE view_name = '%s'"
	"      AND owner = '%s'";
	
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

    query = g_malloc(sizeof(qcoltemplate) + strlen(owner) + strlen(viewname) + 1);
    sprintf(query, qcoltemplate, viewname, owner);

    cols_list = clist_from_query(query, dbid, 0, "", NULL);
    g_free(query);

    if (cols_list == 0 || cols_list->rows == 0) {
	show_warning_msg("Can't find column list for table %s.%s\n\n"
			 "This usually means that the table has been dropped\n"
			 "from another session since the list of tables was\n"
			 "obtained", owner, viewname);
	return;
    }

    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_doviewview(cols_list);

    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_proc_toolbar(dir, 0);
    obj_ginfo.toolbar_owner = OMDB_View;

    query = g_malloc(sizeof(qtexttemplate) + strlen(owner) + strlen(viewname) + 1);
    sprintf(query, qtexttemplate, viewname, owner);
    viewtext = glist_from_query(query, dbid, 0, TRUE, 0);
    
    if (!viewtext || !viewtext->data) {
	show_warning_msg("Can't find the definition for the view %s.%s\n\n"
			 "This usually means that the view has been dropped\n"
			 "from another session since the list of views was\n"
			 "obtained", owner, viewname);
	return;
    }

    /* the returned list should have just a single element */
    assert (viewtext->next == 0);

    build_view = g_malloc(strlen(owner) + strlen(viewname) + strlen(viewtext->data) + 40);
    sprintf(build_view, "CREATE OR REPLACE VIEW %s.%s AS\n%s",owner,viewname, (char *)viewtext->data);

    gtk_text_insert (GTK_TEXT (obj_mainwin.proctext1), NULL, NULL,NULL,
		     build_view, strlen(build_view));
		     
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(obj_mainwin.proctext1));

    g_free(build_view);
    g_free(viewtext->data);
    g_list_free(viewtext);
    g_free(query);

}

void
show_generic_view_window(
    const char *dbversion,
    const char *dbid,
    const char *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;

    static char qviewtemplate80[] = 
	"SELECT   view_name, comments "
	"FROM     all_views, all_tab_comments "
	"WHERE    all_views.owner = '%s'"
	"         AND all_views.owner = all_tab_comments.owner(+)"
	"         AND all_views.view_name = all_tab_comments.table_name(+) "
	"ORDER BY view_name";

    static char qviewtemplate8i[] = 
	"SELECT   v.view_name \"Name\", "
	"         (select comments from all_tab_comments"
	"          where table_name = v.view_name and owner = v.owner) \"Comments\" "
	"FROM     all_views v "
	"WHERE    v.owner = '%s' "
	"ORDER BY v.view_name";

    static char	     tabtemplate[] = "Summary of views owned by user %s";

    if(dbversion)
        if (strstr(dbversion, "8.1") != 0) {
	    query = g_malloc(sizeof(qviewtemplate8i) + strlen(owner) + 1);
	    sprintf(query, qviewtemplate8i, owner);
        } else {
	    query = g_malloc(sizeof(qviewtemplate80) + strlen(owner) + 1);
	    sprintf(query, qviewtemplate80, owner);
        }
    else{
        query = g_malloc(sizeof(qviewtemplate80) + strlen(owner) + 1);
	sprintf(query, qviewtemplate80, owner);
    }
	    
    list = clist_from_query(query, dbid, 0, "", active_step);
    active_clear();
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

