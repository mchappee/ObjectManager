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
#include "logfilefuncs.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <signal.h>
#include <xpm/foldericon.xpm>
#include <xpm/ofoldericon.xpm>

static GdkPixmap *sequence_pixmap=NULL;
static GdkBitmap *sequence_mask;

static GdkPixmap *folder_pixmap=NULL, *ofolder_pixmap;
static GdkBitmap *folder_mask, *ofolder_mask;

#include <xpm/objecticon.xpm>

void 
loaduserlogfiles(
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
	"SELECT member "
	"FROM v$logfile "
	"WHERE group# = %s order by member";

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
	/*dirnode->menu_creator = create_sequencemenu;*/

	text = p->data;
	node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4, 
				     sequence_pixmap, sequence_mask, NULL, NULL, TRUE, TRUE);
		
	gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	g_free(p->data);

    }
    g_list_free (sequences);
}

void
get_logfile_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *name,
    DirNode *dir)
{
    char	     *query;
    GtkCList         *list, *clist;
    int              i=0;
    
    static char	     qtemplate[] = 
        "SELECT 'Group', a.group#, 'Thread', a.thread#, 'Sequence', a.sequence#, "
	"'Bytes', a.bytes, 'Members', a.members, 'Archived', a.archived, "
	"'Status', a.status, 'First Change', a.first_change#, "
	"'Created', a.first_time, 'Member Name', b.member "
        "FROM v$log a, v$logfile b "
        "WHERE a.group# = b.group# "
        "AND b.member = '%s'";

    query = g_malloc(sizeof(qtemplate) + strlen(name) + 1);
		     
    sprintf(query, qtemplate, name);

    clist    = GTK_CLIST(gtk_clist_new(2));
    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    while (i < 20){
        char *clist_xfer[2];
        gtk_clist_get_text(GTK_CLIST(list), 0, i, &clist_xfer[0]);
        gtk_clist_get_text(GTK_CLIST(list), 0, i+1, &clist_xfer[1]);
	gtk_clist_append( (GtkCList *) clist, clist_xfer);
	i += 2;
    }
    
    gtk_widget_destroy(GTK_WIDGET(list));
    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_logfile_toolbar(clist);
    obj_ginfo.toolbar_owner = OMDB_Logfile;
    
    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_dologfileview(clist, "yada");

}

void
show_generic_logfile_window(
    const char *dbversion,
    const char *dbid,
    const char *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;
    static char	     qtemplate[] = 
	"SELECT a.member \"Name\", b.bytes \"Bytes\", b.status \"Status\" "
	"FROM v$logfile a, v$log b "
	"WHERE a.group# = %s "
	"AND a.group# = b.group#";
    static char	     tabtemplate[] = "Summary of Logfiles in group %s";

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

void
get_logfile_groups(
    GtkCTreeNode      *rootnode, 
    GtkCTreeNode      *topnode, 
    char	      *objtype)
{
    GtkCTreeNode      *node;
    GtkCTreeNode      *objectnode;
    char	      *dummy   = "dummy";
    DirNode	      *dirnode, *parent_dirnode;
    gchar	      *text;
    GList             *owners;
    GList             *o;
    char              *prepsql="warn THIS";
    char              *logfiletemplate = "SELECT distinct group# "
                                          "FROM v$logfile";
					
	
    prepsql = logfiletemplate;
    folder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, folder);
    ofolder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &ofolder_mask, NULL, ofolder);
	
    owners = glist_from_query(prepsql, obj_cinfo.login->dbid, 0, FALSE, active_step);
    active_clear();
    
    if(owners==NULL){
        gtk_ctree_node_set_selectable(GTK_CTREE(obj_mainwin.tree1),topnode, FALSE);
        return;
    }
    
    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), topnode);
    

    for (o = owners; o; o = g_list_next(o)) {

	dirnode = new_dirnode(parent_dirnode, o->data);
	dirnode->menu_creator = create_generic_owner_menu;
	text = o->data;

        node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), topnode, NULL, &text, 4,
                                     folder_pixmap, folder_mask, ofolder_pixmap, ofolder_mask, FALSE, FALSE);

        gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);

        objectnode = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), node, NULL, &dummy, 4,
                                           NULL, NULL, NULL, NULL, FALSE, FALSE);
        g_free(o->data);
    }

    
    return;
    
}

