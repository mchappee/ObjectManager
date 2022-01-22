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
#include "synonymfuncs.h"

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
loadusersynonyms(
    GtkCTreeNode  *parent_node,
    char	  *owner, 
    const char	  *dbversion,
    const char	  *dbid)
{
    char	  *query;
    char	  *qtemplate = "SELECT synonym_name FROM all_synonyms WHERE owner = '%s' "
                               "order by synonym_name";
    char	  *text;
    DirNode	  *dirnode, *parent_dirnode;
    GtkCTreeNode  *node;
    GList             *owners;
    GList             *o;

    query = g_malloc(strlen(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    
    owners = glist_from_query(query, dbid, 0, TRUE, active_step);
    active_clear();
    
    if(owners==NULL)
        return;
    
    if (! folder_pixmap)
	folder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, object);
    
    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    for (o = owners; o; o = g_list_next(o)) {

	dirnode = new_dirnode(parent_dirnode, o->data);
	dirnode->menu_creator = create_synonymmenu;
	text = o->data;

        node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4,
                                     folder_pixmap, folder_mask, NULL, NULL, FALSE, FALSE);

        gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);

        g_free(o->data);
    }
    g_list_free (owners);
}

void
get_synonym_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *synonym_name,
    DirNode	     *dir)
{
    char	     *query;
    GtkCList         *list;
    GtkWidget        *clist;
    int              i=0;
    static char	     qtemplate[] = 
	"SELECT "
	"'Table Owner:'    ,table_owner, "
	"'Table Name:'     ,table_name, "
	"'DBLink:'         ,NVL(db_link,'none') "
	"FROM   all_synonyms "
	"WHERE  synonym_name = '%s'"
	"       AND owner = '%s'";

    clist = gtk_clist_new(2);

    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_blank_toolbar();
    obj_ginfo.toolbar_owner = OMDB_None;

    query = g_malloc(sizeof(qtemplate) + strlen(synonym_name) + 
		     strlen(owner) + 1);
    sprintf(query, qtemplate, synonym_name, owner);

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    while (i < 5){
        char *clist_xfer[2];
        gtk_clist_get_text(GTK_CLIST(list), 0, i, &clist_xfer[0]);
        gtk_clist_get_text(GTK_CLIST(list), 0, i+1, &clist_xfer[1]);
	gtk_clist_append( (GtkCList *) clist, clist_xfer);
	i += 2;
    }
    
    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_dosynonymview(clist);
    
    gtk_widget_destroy(GTK_WIDGET(list));

}

void
show_generic_synonym_window(
    const char *dbversion,
    const char *dbid,
    const char *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;

    static char qtemplate[] = 
	"SELECT synonym_name \"Name\", table_owner || '.' || table_name \"For Object\" "
	"FROM   all_synonyms "
	"WHERE  owner = '%s' order by synonym_name";

    static char	     tabtemplate[] = "Summary of synonyms for user %s";

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
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

