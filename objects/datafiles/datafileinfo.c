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
#include "objects/datafiles/datafilefuncs.h"
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
loaduserdatafiles(
    GtkCTreeNode  *parent_node,
    char	  *owner, 
    const char	  *dbversion,
    const char	  *dbid)
{
    char          *query;
    char	  *qtemplate = "SELECT file_name FROM dba_data_files WHERE tablespace_name = '%s' "
                               "order by file_name";
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
	dirnode->menu_creator = create_datafilemenu;
	text = o->data;

        node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4,
                                     folder_pixmap, folder_mask, NULL, NULL, FALSE, FALSE);

        gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);

        g_free(o->data);
    }
    g_list_free (owners);
}

void
get_datafile_info(
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
    char             *alter_params[3];
    int              i=0;
    int              clistcount;
    GtkCList         *list;
    GtkCList         *mathlist;
    GtkWidget        *clist;
    GtkWidget        *progress;

    char qmathtemplate[] = 
      "SELECT d.file_name,d.tablespace_name, "
             "v.status,TO_CHAR((d.bytes / 1024 / 1024), '99999990.000'), "
             "NVL(TO_CHAR(((d.bytes - SUM(s.bytes)) / 1024 / 1024), '99999990.000'), "
             "TO_CHAR((d.bytes / 1024 / 1024),'99999990.000')), "
	     "NVL(LTRIM(TO_CHAR(((d.bytes - SUM(s.bytes)) / 1024),'9999999999')),'NA') "
      "FROM dba_data_files d,dba_free_space s, sys.v_$datafile v "
      "WHERE (s.file_id (+)= d.file_id) "
      "AND (d.file_name = v.name) "
      "AND (d.file_name = '%s') "
      "AND d.tablespace_name = '%s' "
      "GROUP BY d.tablespace_name,d.file_name, v.status, d.bytes";

    char qmisctemplate8[] = 
         "SELECT "
           "'File ID:', a.file_id, "
	   "'Bytes:', a.bytes / 1024, "
	   "'Initial Bytes:' ,b.create_bytes / 1024, "
	   "'Used Bytes:', '%s', "
	   "'Max Bytes:', a.maxbytes / 1024, "
	   "'Blocks:', a.blocks, "
	   "'Max Blocks:', a.maxblocks, "
	   "'Increment:', a.increment_by, "
	   "'Status:', '%s', "
           "'Enabled:', b.enabled, "
	   "'VStatus:', b.status, "
	   "'Creation Date:', to_char(b.creation_time,'MM-DD-YYYY') "
           "FROM "
           "dba_data_files a,v$datafile b "
	   "WHERE a.file_name = '%s' "
	   "AND a.file_id = b.file#";

    char qmisctemplate7[] = 
         "SELECT "
           "'File ID:', a.file_id, "
	   "'Bytes:', a.bytes / 1024, "
	   "'Initial Bytes:' ,b.create_bytes / 1024, "
	   "'Used Bytes:', '%s', "
	   "'Blocks:', a.blocks, "
	   "'Status:', '%s', "
           "'Enabled:', b.enabled, "
	   "'VStatus:', b.status "
           "FROM "
           "dba_data_files a,v$datafile b "
	   "WHERE a.file_name = '%s' "
	   "AND a.file_id = b.file#";

    clist    = gtk_clist_new(2);
    progress = gtk_progress_bar_new ();

    if(obj_ginfo.toolbar_owner != OMDB_Datafile){
	gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
	obj_create_blank_toolbar();
	obj_ginfo.toolbar_owner = OMDB_Datafile;
    }
       
    query = g_malloc(sizeof(qmathtemplate) + strlen(owner) + strlen(filename) +1);
    sprintf(query, qmathtemplate, filename, owner);

    mathlist = clist_from_query(query, dbid, 0, "<NULL>", NULL);

    gtk_clist_get_text(GTK_CLIST(mathlist), 0, 2, &mathquery[0]);
    gtk_clist_get_text(GTK_CLIST(mathlist), 0, 3, &mathquery[1]);
    gtk_clist_get_text(GTK_CLIST(mathlist), 0, 4, &mathquery[2]);
    gtk_clist_get_text(GTK_CLIST(mathlist), 0, 5, &mathquery[3]);

    g_free(query);
    
    query = g_malloc(sizeof(qmisctemplate8) + strlen(filename) + 
                     strlen(mathquery[3]) + strlen(mathquery[0]) + 1);

    if(dbversion){  /* This means 8.x */
        sprintf(query, qmisctemplate8, mathquery[3], mathquery[0], filename);
	clistcount = 23;
    }else{           /* This means 7.3.x */
        sprintf(query, qmisctemplate7, mathquery[3], mathquery[0], filename);
	clistcount = 15;
    }

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    while (i < clistcount){
        char *clist_xfer[2];
        gtk_clist_get_text(GTK_CLIST(list), 0, i, &clist_xfer[0]);
        gtk_clist_get_text(GTK_CLIST(list), 0, i+1, &clist_xfer[1]);
	gtk_clist_append( (GtkCList *) clist, clist_xfer);
	i += 2;
    }
    

    pctused = atof(mathquery[2]) / atof(mathquery[1]);	
    gtk_progress_bar_update( GTK_PROGRESS_BAR(progress),pctused);
    
    disptext=g_malloc(50);
    sprintf(disptext,"Percent Usage: %f ",pctused * 100);
    
    gtk_progress_set_format_string(GTK_PROGRESS(progress),disptext);
    gtk_progress_set_show_text(GTK_PROGRESS(progress),TRUE);

    g_free(disptext);

    gtk_clist_get_text(GTK_CLIST(clist), 1, 1, &alter_params[0]);
    gtk_clist_get_text(GTK_CLIST(clist), 4, 1, &alter_params[1]);
    
    if(dbversion)  /* This means 8.x */
        gtk_clist_get_text(GTK_CLIST(clist), 7, 1, &alter_params[2]);
    else           /* This means 7.3.x */
        alter_params[2]="NA in Oracle 7.x";

    gtk_widget_destroy(obj_mainwin.notebook1);
    obj_dodatafileview(clist, progress);

    alter_datafilewin ((char *)filename,(char *)owner,alter_params[0],alter_params[1],alter_params[2]);
    gtk_widget_destroy(GTK_WIDGET(list));
    gtk_widget_destroy(GTK_WIDGET(mathlist));

}

void
show_generic_datafile_window(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner)
{
    GtkCList	     *list;
    char	     *query;
    char	     *notebook_tab;
    static char	     qtemplate[] = 
	"SELECT file_name \"File Name\", bytes \"Bytes\", status \"Status\" "
	"FROM   dba_data_files "
	"WHERE  tablespace_name = '%s' order by file_name";
    static char	     tabtemplate[] = "Summary of datafiles in tablespace %s";

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

