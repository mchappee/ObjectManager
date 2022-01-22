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
#include "objects/tables/tablefuncs.h"
#include "sequencefuncs.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <signal.h>

static GdkPixmap *sequence_pixmap=NULL;
static GdkBitmap *sequence_mask;

#include <xpm/objecticon.xpm>

void 
loadusersequences(
    GtkCTreeNode     *parent_node,
    char	     *owner, 
    const char	     *dbversion,
    const char	     *dbid)
{
    char	     *query;
    GtkCTreeNode     *node;
    DirNode	     *dirnode;
    DirNode	     *parent_dirnode;
    char	     *text;
    GList	     *sequences;
    GList	     *p;
    static char	     qtemplate[] = 
	"SELECT sequence_name "
	"FROM all_sequences "
	"WHERE sequence_owner = '%s' order by sequence_name";


    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    sequences = glist_from_query(query, dbid, 0, TRUE, NULL);
    g_free(query);

    if (!sequence_pixmap)
	sequence_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window,
						       &sequence_mask, NULL, object);

    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    for (p = sequences; p; p = g_list_next(p)) {
	dirnode = new_dirnode(parent_dirnode, p->data);
	dirnode->menu_creator = create_sequencemenu;

	text = p->data;
	node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4, 
				     sequence_pixmap, sequence_mask, NULL, NULL, TRUE, TRUE);
		
	gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	g_free(p->data);

    }
    g_list_free (sequences);
}

void
get_sequence_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *name,
    DirNode *dir)
{
    char	     *query;
    GtkCList         *list;
    char             *parm[6];
  
    static char	     qtemplate[] = 
	"SELECT  min_value, max_value, increment_by, "
	"       cycle_flag, order_flag, cache_size, last_number "
	"FROM all_sequences "
	"WHERE sequence_owner = '%s'"
        "      AND sequence_name = '%s'";

    if (obj_ginfo.toolbar_owner != OMDB_Sequence) {
	gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
	obj_create_blank_toolbar();
	obj_ginfo.toolbar_owner = OMDB_Sequence;
    }

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + strlen(name) + 1);
    sprintf(query, qtemplate, owner, name);

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    gtk_clist_get_text(GTK_CLIST(list), 0, 0, &parm[0]);
    gtk_clist_get_text(GTK_CLIST(list), 0, 1, &parm[1]);
    gtk_clist_get_text(GTK_CLIST(list), 0, 2, &parm[2]);
    gtk_clist_get_text(GTK_CLIST(list), 0, 3, &parm[3]);
    gtk_clist_get_text(GTK_CLIST(list), 0, 4, &parm[4]);
    gtk_clist_get_text(GTK_CLIST(list), 0, 5, &parm[5]);

    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_dosequenceview((char *)name,(char *)owner,parm[0],parm[1],parm[2],parm[3],parm[4],atol(parm[5]));
    gtk_widget_destroy(GTK_WIDGET(list));

}

void
show_generic_sequence_window(
    const char *dbversion,
    const char *dbid,
    const char *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;
    static char	     qtemplate[] = 
	"SELECT sequence_name \"Name\", last_number \"Last Number\" "
	"FROM all_sequences "
	"WHERE sequence_owner = '%s' order by sequence_name";
    static char	     tabtemplate[] = "Summary of sequences owned by %s";

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    notebook_tab = g_malloc(sizeof(tabtemplate) + strlen(owner) + 1);
    sprintf(notebook_tab, tabtemplate, owner);

    gtk_widget_destroy(obj_mainwin.notebook1);
    glist_set_opt_col_width(list, 0);

    obj_do_generic_view(list, notebook_tab);
    g_free(notebook_tab);
}
