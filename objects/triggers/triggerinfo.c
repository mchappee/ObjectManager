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
#include "objects/procedures/procfuncs.h"
#include "triggerfuncs.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <oci.h>

#include "xpm/triggericon.xpm"

static GdkPixmap *trig_pixmap=NULL;
static GdkBitmap *trig_mask;

void 
loadusertriggers(
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
    GList	     *triggers;
    GList	     *p;
    char	     qtemplate[] = 
	"SELECT trigger_name "
	"FROM all_triggers "
	"WHERE owner = '%s' order by trigger_name";


    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    triggers = glist_from_query(query, dbid, 0, TRUE, NULL);
    g_free(query);

    if (!trig_pixmap)
	trig_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window,
						       &trig_mask, NULL, triggerpixmap);

    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    for (p = triggers; p; p = g_list_next(p)) {
	dirnode = new_dirnode(parent_dirnode, p->data);
	dirnode->menu_creator = create_triggermenu;

	text = p->data;
	node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4, 
				     trig_pixmap, trig_mask, NULL, NULL, TRUE, TRUE);
		
	gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	g_free(p->data);

    }
    g_list_free (triggers);
}

static int
trigger_info_rowcallback(
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
    if ( row > -1 ){
        assert(ncols == 2);
        *(char **)user_data = g_malloc(32 + strlen(values[0]) + strlen(values[1]));
        sprintf(*(char **)user_data, "Create or Replace TRIGGER %s\n%s", 
        values[0],values[1]);
    }
    return FALSE;
}

void
get_trigger_info(
    const char	 *dbversion,
    const char	 *dbid,
    const char	 *owner,
    const char	 *name,
    DirNode	 *dir)
{
    GtkCList	 *list;
    char	 *body;
    char	 *query;
    static char  *titles[] = {"Tag", "Value"};
    static char	 qtemplate80[] = 
	"SELECT trigger_name \"Name\", "
        "       trigger_type \"Type\", "
        "       triggering_event \"Triggering Event\", "
	"       table_owner || '.' || table_name \"On\", "
	"       referencing_names \"Referencing Names\", "
	"       when_clause \"When Clause\", "
	"       status \"Status\" "
	"FROM   all_triggers "
	"WHERE  owner = '%s'"
	"       AND trigger_name = '%s'";

    static char	 qtemplate8i[] = 
	"SELECT trigger_name \"Name\", "
        "       trigger_type \"Type\", "
        "       triggering_event \"Triggering Event\", "
	"       table_owner || '.' || table_name \"On\", "
	"       column_name \"Column Name\", "
	"       action_type \"Action Type\", "
	"       referencing_names \"Referencing Names\", "
	"       when_clause \"When Clause\", "
	"       status \"Status\" "
	"FROM   all_triggers "
	"WHERE  owner = '%s'"
	"       AND trigger_name = '%s'";

    static char	 qdeftemplate[] = 
	"SELECT description, trigger_body "
	"FROM   all_triggers "
	"WHERE  owner = '%s'"
	"       AND trigger_name = '%s'";

    if(dbversion)
        if (strstr(dbversion, "8.1") != 0) {
	    query = g_malloc(sizeof(qtemplate8i) + strlen(owner) + strlen(name) + 1);
	    sprintf(query, qtemplate8i, owner, name);
        } else {
	    query = g_malloc(sizeof(qtemplate80) + strlen(owner) + strlen(name) + 1);
	    sprintf(query, qtemplate80, owner, name);
        }
    else{
        query = g_malloc(sizeof(qtemplate80) + strlen(owner) + strlen(name) + 1);
	sprintf(query, qtemplate80, owner, name);
    }
    
    list = clist_from_query_by_row(query, dbid, 2, titles, NULL);
    glist_set_opt_col_width(list, 0);
    g_free(query);

    query = g_malloc(sizeof(qdeftemplate) + strlen(owner) + strlen(name) + 1);
    sprintf(query, qdeftemplate, owner, name);

    oracle_do_query(query, dbid, 0, (void *)trigger_info_rowcallback, NULL, &body);
    /*do_query(query, dbid, 1, trigger_info_rowcallback, &body);*/
    g_free(query);

    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_dotriggerview(list, body);
    gtk_widget_show(GTK_WIDGET(list));

    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_proc_toolbar(dir, 2);
    gtk_widget_show (obj_mainwin.toggletrigger);
    obj_ginfo.toolbar_owner = OMDB_Trigger;

    get_trigger_status(owner,name,dbid,dir);
    
    gtk_signal_handler_unblock_by_func (GTK_OBJECT (obj_mainwin.toggletrigger),
        GTK_SIGNAL_FUNC (obj_on_trigger_clicked),(gpointer)dir);
		
    getprocstatus(owner,name,dbid,0);

    g_free(body);
}

void
show_generic_trigger_window(
    const char	 *dbversion,
    const char	 *dbid,
    const char	 *owner)
{
    GtkCList	 *list;
    char	 *query;
    char	 *notebook_tab;

    static char qtemplate[] = 
	"SELECT trigger_name \"Name\", "
        "       trigger_type || triggering_event \"Type\", "
	"       table_owner || '.' || table_name \"On\", "
	"       status \"Status\" "
	"FROM   all_triggers "
	"WHERE  owner = '%s' order by trigger_name";

    static char	     tabtemplate[] = "Summary of triggers owned by %s";

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    notebook_tab = g_malloc(sizeof(tabtemplate) + strlen(owner) + 1);
    sprintf(notebook_tab, tabtemplate, owner);

    gtk_widget_destroy(obj_mainwin.notebook1);
    glist_set_opt_col_width(list, 0);
    glist_set_opt_col_width(list, 1);
    glist_set_opt_col_width(list, 2);

    obj_do_generic_view(list, notebook_tab);
    g_free(notebook_tab);
}
