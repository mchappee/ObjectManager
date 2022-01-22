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
#include "tablespacefuncs.h"
#include "objects/tables/tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void
show_generic_tablespace_window(
    const char	 *dbversion,
    const char	 *dbid,
    const char	 *owner)
{
    GtkCList	 *list;
    GtkCList	 *list2;
    GList        *glist;
    GList        *t;
    char	 *query;
    char	 *notebook_tab;
    char         *datafiles[8];
    char         *dirpath;
    int          clist_row=0;
    int          tmpnum=0;
    GdkColor	 bcolor;
    GdkColormap	 *bcmap;
    GdkColor	 fcolor;
    GdkColormap	 *fcmap;
    static char qtemplate73[] = 
	"SELECT   initial_extent \"Initial Extent\", next_extent \"Next Extent\", "
	"         min_extents \"Min Extents\", max_extents \"Max Extents\", "
	"         pct_increase \"%% Increase\", status \"Status\", contents \"Contents\" "
	"FROM     dba_tablespaces "
	"WHERE    tablespace_name = '%s' "
	"ORDER BY tablespace_name";
	
    static char qtemplate80[] = 
	"SELECT   initial_extent \"Initial Extent\", next_extent \"Next Extent\", "
	"         min_extents \"Min Extents\", max_extents \"Max Extents\", "
	"         pct_increase \"%% Increase\", status \"Status\", contents \"Contents\", "
	"         logging \"Logging\" "
	"FROM     dba_tablespaces "
	"WHERE    tablespace_name = '%s' "
	"ORDER BY tablespace_name";

    static char qtemplate8i[] = 
	"SELECT   initial_extent \"Initial Extent\", next_extent \"Next Extent\", "
	"         min_extents \"Min Extents\", max_extents \"Max Extents\", "
	"         pct_increase \"%% Increase\", status \"Status\", contents \"Contents\", "
	"         logging \"Logging\" "
	"FROM     dba_tablespaces "
	"WHERE    tablespace_name = '%s' "
	"ORDER BY tablespace_name";

    char	  qtemplate[] = "SELECT file_name FROM dba_data_files WHERE tablespace_name = '%s' "
                               "order by file_name";
    char	  utemplate[] = "SELECT username FROM dba_users WHERE default_tablespace = '%s'";
    char          ctemplate[] = "SELECT percent_extents_coalesced, percent_blocks_coalesced "
                                "FROM dba_free_space_coalesced WHERE tablespace_name = '%s'";

    static char	     tabtemplate[] = "Summary of tablespaces";

    dirpath = g_malloc(strlen(dbid) + strlen(owner) + 3);
    sprintf(dirpath, "%s~%s", dbid, owner);

    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_tablespace_toolbar(g_strdup(dirpath));
    obj_ginfo.toolbar_owner = OMDB_Tablespace;

    g_free(dirpath);

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
	sprintf(query, qtemplate73, owner);
    }
    
    /* fprintf(stderr, "%s\n", query); */
    list = clist_from_query(query, dbid, 0, "", NULL);
    g_free(query);

    notebook_tab = g_malloc(sizeof(tabtemplate)+ 1);
    strcpy(notebook_tab, tabtemplate);

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);

    glist = glist_from_query(query, dbid, 0, TRUE, 0);
    g_free(query);

    if (glist) {
    
        for(clist_row=0; clist_row < 8; clist_row++)
	    datafiles[clist_row]=NULL;
	    
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
	    
	gtk_clist_append(list, datafiles);
	gtk_clist_append(list, datafiles);
	datafiles[0]="Datafiles:";
	gtk_clist_append(list, datafiles);
	
        for (t = glist; t; t = g_list_next(t)) {
            datafiles[0]=t->data;
	    gtk_clist_append(list, datafiles);
        }
	
	tmpnum = list->rows;
	
	datafiles[0]=NULL;
	gtk_clist_append(list, datafiles);
	
	query = g_malloc(sizeof(qtemplate80) + strlen(owner) + 1);
        sprintf(query, utemplate, owner);

        glist = glist_from_query(query, dbid, 0, TRUE, 0);
	g_free(query);
	
	for (t = glist; t; t = g_list_next(t)) {
            datafiles[0]=t->data;
	    gtk_clist_append(list, datafiles);
        }
	
	datafiles[0]=NULL;
	gtk_clist_insert(list, tmpnum+1, datafiles);
	gtk_clist_insert(list, tmpnum+2, datafiles);
	
	datafiles[0]="Default Users:";
	gtk_clist_insert(list, tmpnum+3, datafiles);
	
	gtk_clist_set_foreground((GtkCList *)list,3,&fcolor);
        gtk_clist_set_background((GtkCList *)list,3,&bcolor);
	
	gtk_clist_set_foreground((GtkCList *)list,tmpnum+3,&fcolor);
        gtk_clist_set_background((GtkCList *)list,tmpnum+3,&bcolor);
	
	query = g_malloc(sizeof(ctemplate) + strlen(owner) + 1);
        sprintf(query, ctemplate, owner);

        list2 = clist_from_query(query, dbid, 0, "", NULL);
	g_free(query);
	
	datafiles[0]=NULL;
	gtk_clist_append(list, datafiles);
	gtk_clist_append(list, datafiles);
	
	datafiles[0]="Percent Extents Coalesced:";
	gtk_clist_get_text(list2,0,0,&datafiles[1]);
	gtk_clist_append(list, datafiles);
	
	datafiles[0]="Percent Blocks Coalesced:";
	gtk_clist_get_text(list2,0,1,&datafiles[1]);
	gtk_clist_append(list, datafiles);
	
	gtk_widget_destroy(GTK_WIDGET(list2));
	g_list_free (glist);
	
    }

    gtk_widget_destroy(obj_mainwin.notebook1);
    if (list) {
	glist_set_opt_col_width(list, 0);
	glist_set_opt_col_width(list, 1);

	obj_do_generic_view(list, notebook_tab);
    }
    
    g_free(notebook_tab);
}
