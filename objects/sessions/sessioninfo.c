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
#include "sessionfuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

static GdkPixmap *ofolder_pixmap, *activepic, *inactivepic;
static GdkBitmap *ofolder_mask, *active_mask, *inactive_mask;

#include <xpm/activesesicon.xpm>
#include <xpm/inactivesesicon.xpm>

void 
get_session_info(
    const char	 *dbversion,
    const char	 *dbid,
    const char	 *owner,
    const char	 *sessionid,
    DirNode *dir)
{
    char	 *query;
    char	 *s, *sid, *sqltext;
    char	 *process;
    int          i=0;
    GdkColor	 bcolor;
    GdkColormap	 *bcmap;
    GdkColor	 fcolor;
    GdkColormap	 *fcmap;
    char         *clist_xfer[3];
    GtkCList     *list1;
    GtkCList     *list2;

    static char qvsession[] =
	"SELECT "
	"'User Name:'    ,username, "
	"'Status:'       ,status, "
	"'OS User:'      ,osuser, "
	"'Acct Type:'    ,type, "
	"'Logon Time:'   ,to_char(logon_time,'HH24:MI:SS'), "
        "'Schema:'       ,schemaname, "
	"'Application:'  ,program, "
	"'Machine:'      ,machine, "
	"'SID:'          ,sid, "
	"'Serial ID:'    ,serial# "
        "FROM v$session "
        "WHERE process = '%s'";

    static char qvsqlarea[] =
	"SELECT "
	"'SQLText:'          ,NVL(sql_text,'No Text Available'), "
	"'Sorts:'            ,sorts, "
	"'Loads:'            ,loads, "
	"'Disk Reads:'       ,disk_reads, "
	"'Buffer Gets:'      ,buffer_gets, "
	"'Rows Processed:'   ,rows_processed, "
	"'Parse Calls:'      ,parse_calls, "
	"'Optimizer:'        ,NVL(optimizer_mode,'NA'), "
	"'Sharable Mem:'     ,sharable_mem, "
	"'Persistent Mem:'   ,persistent_mem, "
	"'Runtime Mem:'      ,runtime_mem "
        "from v$session, v$sqlarea "
        "where sql_address = address "
        "and sid = %s";

/*    static char qvopencursor[] =
        "SELECT count(*) from v$open_cursor "
        "WHERE sid = %d";*/

    s = g_strdup(sessionid);
    
    query=strtok(s,"~");
    process=strtok(NULL,"~");
    if(query == NULL || process == NULL){
      msgbox("Malformed sessionid, returning");
      return;
    }

    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_dosessionview();
    
    g_free(s);
    
    query = g_malloc(sizeof(qvsession) + strlen(process) + 10);
    sprintf(query, qvsession, process);

    list1 = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    if(!list1->rows){
        msgbox("The session that you have selected is no longer valid.  It looks like they've disconnected.");
	gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
	return;
    }

    gtk_clist_get_text(GTK_CLIST(list1), 0, 17, &sid);
    query = g_malloc(sizeof(qvsqlarea) + 10);
    sprintf(query, qvsqlarea, sid);

    list2 = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    if(!list2->rows){
        msgbox("The session that you have selected is no longer valid.  It looks like they've disconnected.");
	gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
	return;
    }

    gtk_clist_get_text(GTK_CLIST(list2), 0, 1, &sqltext);
    if(strlen(sqltext) > 0 && strlen(sqltext) < 1001)
      gtk_text_insert (GTK_TEXT (obj_mainwin.sessiontext1), NULL, NULL,
		     NULL, sqltext, strlen(sqltext));

    else
      gtk_text_insert (GTK_TEXT (obj_mainwin.sessiontext1), NULL, NULL,
		     NULL, "No text available", 17);
		     
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(obj_mainwin.sessiontext1));

    clist_xfer[0]=NULL;
    while (i < 19){
        gtk_clist_get_text(GTK_CLIST(list1), 0, i, &clist_xfer[1]);
        gtk_clist_get_text(GTK_CLIST(list1), 0, i+1, &clist_xfer[2]);
	gtk_clist_append( (GtkCList *) obj_mainwin.sessionclist2, clist_xfer);
	i += 2;
    }

    i = 2;
    while (i < 21){
        gtk_clist_get_text(GTK_CLIST(list2), 0, i, &clist_xfer[1]);
        gtk_clist_get_text(GTK_CLIST(list2), 0, i+1, &clist_xfer[2]);
	gtk_clist_append( (GtkCList *) obj_mainwin.sessionclist2, clist_xfer);
	i += 2;
    }

    gtk_widget_destroy(GTK_WIDGET(list1));
    gtk_widget_destroy(GTK_WIDGET(list2));

    bcmap = gdk_colormap_get_system();
       bcolor.red = 32000;
       bcolor.green = 32000;
       bcolor.blue = 32000;
       if (!gdk_color_alloc(bcmap, &bcolor)) {
         g_error("couldn't allocate color");
       }
       
    fcmap = gdk_colormap_get_system();
       fcolor.red = 65534;
       fcolor.green = 65534;
       fcolor.blue = 65534;
       if (!gdk_color_alloc(fcmap, &fcolor)) {
         g_error("couldn't allocate color");
       }

    clist_xfer[0]="User Information:";
    clist_xfer[1]=NULL;
    clist_xfer[2]=NULL;
    gtk_clist_prepend( (GtkCList *) obj_mainwin.sessionclist2, clist_xfer);
    
    clist_xfer[0]="Query Information:";
    gtk_clist_insert( (GtkCList *) obj_mainwin.sessionclist2, 11, clist_xfer);
    
    clist_xfer[0]="Memory Information:";
    gtk_clist_insert( (GtkCList *) obj_mainwin.sessionclist2, 19, clist_xfer);

    gtk_clist_set_foreground((GtkCList *)obj_mainwin.sessionclist2,0,&fcolor);
    gtk_clist_set_background((GtkCList *)obj_mainwin.sessionclist2,0,&bcolor);
    
    gtk_clist_set_foreground((GtkCList *)obj_mainwin.sessionclist2,11,&fcolor);
    gtk_clist_set_background((GtkCList *)obj_mainwin.sessionclist2,11,&bcolor);
    
    gtk_clist_set_foreground((GtkCList *)obj_mainwin.sessionclist2,19,&fcolor);
    gtk_clist_set_background((GtkCList *)obj_mainwin.sessionclist2,19,&bcolor);

    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_session_toolbar(obj_mainwin.sessionclist2);
    obj_ginfo.toolbar_owner = OMDB_Session;

}


void 
loadusersessions(
    GtkCTreeNode  *parent_node,
    char	  *owner, 
    const char	  *dbversion,
    const char	  *dbid)
{
    char	  *identifier;
    char	  *query;
    char	  *machine;
    char	  *process;
    char	  *status;
    char	  *sid;
    char	  *text;
    int           i=0;
    DirNode	  *dirnode, *parent_dirnode;
    GtkCTreeNode  *node;
    GtkCList      *list;
    char	  *qtemplate = "SELECT "
                               "machine, process, sid, status "
			       "FROM v$session "
		               "WHERE machine IS NOT NULL "
			       "AND username = '%s' "
		               "ORDER BY machine";

    activepic = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &active_mask, NULL, active);
    inactivepic = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &inactive_mask, NULL, inactive);

    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    query = g_malloc(strlen(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    while(i < list->rows){
	gtk_clist_get_text(GTK_CLIST(list), i, 0, &machine);
	gtk_clist_get_text(GTK_CLIST(list), i, 1, &process);
	gtk_clist_get_text(GTK_CLIST(list), i, 2, &sid);
	gtk_clist_get_text(GTK_CLIST(list), i, 3, &status);

	identifier=g_malloc(strlen(machine) + strlen(process) + strlen(sid) + 3);
	sprintf(identifier,"%s~%s~%s",machine, process, sid);

	dirnode = new_dirnode(parent_dirnode, identifier);
	dirnode->menu_creator = create_sessionmenu;

	g_free(identifier);
	text = machine;

	if(strncmp(status,"A",1)==0)
	    node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4, 
					 activepic, active_mask, ofolder_pixmap, ofolder_mask, TRUE, TRUE);
	else
	    node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4, 
					 inactivepic, inactive_mask, ofolder_pixmap, ofolder_mask, TRUE, TRUE);

	gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	
	i++;
    }
    
    gtk_widget_destroy(GTK_WIDGET(list));

}

void
show_generic_session_window(
    const char *dbversion,
    const char *dbid,
    const char *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;

    static char qtemplate[] = 
	"SELECT machine \"Machine\", status \"Status\" "
	"FROM   v$session "
	"where machine IS NOT NULL "
	"AND username = '%s' order by machine";

    static char	     tabtemplate[] = "Summary of sessions for user %s";

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


