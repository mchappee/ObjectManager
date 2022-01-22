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
#include "objects/rollbacks/rollbackfuncs.h"
#include "objects/tables/tablefuncs.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <signal.h>

#define sqlerr() { obj_sql_error(); goto stopover; }

static GdkPixmap *folder_pixmap=NULL;
static GdkBitmap *folder_mask;

#include <xpm/objecticon.xpm>

void 
loadrollbacks(
    GtkCTreeNode  *parent_node,
    char	  *owner, 
    const char	  *dbversion,
    const char	  *dbid)
{
    char          *query;
    char	  *qtemplate = "SELECT segment_name FROM dba_rollback_segs "
                               "where tablespace_name = '%s' order by segment_name";
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
	/*dirnode->menu_creator = create_datafilemenu;*/
	text = o->data;

        node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4,
                                     folder_pixmap, folder_mask, NULL, NULL, FALSE, FALSE);

        gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);

        g_free(o->data);
    }
    g_list_free (owners);
}

void
get_rollback_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *filename,
    DirNode	     *dir)
{
    float	     pctused;
    char	     *query;
    char             *disptext;
    char             *mathquery[4];
    int              i=0;
    int              toggled=0;
    GtkCList         *list;
    GtkCList         *mathlist;
    GtkWidget        *clist;
    GtkWidget        *progress;

    char qmathtemplate[] = 
           "select extents,curext from sys.v_$rollstat where usn in "
           "(select segment_id from dba_rollback_segs where segment_name = '%s')";
    char qmisctemplate[] = 
         "SELECT "
           "'Segment Name:', a.segment_name, "
	   "'Owner:', a.owner, "
	   "'Tablespace:' ,a.tablespace_name, "
	   "'Segment ID:', a.segment_id, "
	   "'File ID:', a.file_id, "
	   "'Initial Extent:', a.initial_extent, "
	   "'Next Extent:', a.next_extent, "
	   "'Status:', a.status, "
	   "'Writes:', b.writes, "
           "'Gets:', b.gets, "
	   "'Waits:', b.waits, "
	   "'Shrinks:', b.shrinks, "
	   "'Extends:', b.extends "
           "FROM "
           "dba_rollback_segs a,sys.v_$rollstat b "
	   "WHERE a.segment_name = '%s'";

    clist    = gtk_clist_new(2);
    progress = gtk_progress_bar_new ();
       
    query = g_malloc(sizeof(qmathtemplate) + strlen(filename) + 1);
    sprintf(query, qmathtemplate, filename);

    mathlist = clist_from_query(query, dbid, 0, "<NULL>", NULL);

    if(mathlist->rows){
        gtk_clist_get_text(GTK_CLIST(mathlist), 0, 0, &mathquery[0]);
        gtk_clist_get_text(GTK_CLIST(mathlist), 0, 1, &mathquery[1]);
    }
    
    g_free(query);
    
    query = g_malloc(sizeof(qmisctemplate) + strlen(filename) + 1);
		     
    sprintf(query, qmisctemplate, filename);

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    while (i < 24){
        char *clist_xfer[2];
        gtk_clist_get_text(GTK_CLIST(list), 0, i, &clist_xfer[0]);
        gtk_clist_get_text(GTK_CLIST(list), 0, i+1, &clist_xfer[1]);
	gtk_clist_append( (GtkCList *) clist, clist_xfer);
	i += 2;
    }

    disptext=g_malloc(50);

    if(mathlist->rows){
        pctused = atof(mathquery[1]) / atof(mathquery[0]);	
        gtk_progress_bar_update( GTK_PROGRESS_BAR(progress),pctused);
    
        sprintf(disptext,"Percent Usage: %f ",pctused * 100);
    
        gtk_progress_set_format_string(GTK_PROGRESS(progress),disptext);
        gtk_progress_set_show_text(GTK_PROGRESS(progress),TRUE);
        toggled=1;

    }else{
    
        strcpy(disptext,"Offline");
        gtk_progress_set_format_string(GTK_PROGRESS(progress),disptext);
        gtk_progress_set_show_text(GTK_PROGRESS(progress),TRUE);
	toggled=0;
    }
    
    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_rollback_toolbar(dir,toggled);
    obj_ginfo.toolbar_owner = OMDB_Rollback;
    
    g_free(disptext);
    
    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_dorollbackview(clist, progress);

    gtk_widget_destroy(GTK_WIDGET(list));
    gtk_widget_destroy(GTK_WIDGET(mathlist));

}

void
show_generic_rollback_window(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;
    static char	     qtemplate[] = 
	"SELECT segment_name \"Segment Name\", owner \"Owner\", status \"Status\" "
	"FROM   dba_rollback_segs "
	"WHERE tablespace_name = '%s' order by segment_name";
    static char	     tabtemplate[] = "Summary of rollback segs in tablespace %s";

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    list = clist_from_query(query, dbid, 0, "<NULL>", active_step);
    active_clear();
    g_free(query);

    notebook_tab = g_malloc(sizeof(tabtemplate) + strlen(owner) + 1);
    sprintf(notebook_tab, tabtemplate, owner);

    gtk_widget_destroy(obj_mainwin.notebook1);
    glist_set_opt_col_width(list, 0);
    glist_set_opt_col_width(list, 1);

    obj_do_generic_view(list, notebook_tab);
    g_free(notebook_tab);
}

