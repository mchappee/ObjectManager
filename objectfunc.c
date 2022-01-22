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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "include/objectman.h"

#include "objects/dblinks/dblinkfuncs.h"
#include "objects/indexes/indexfuncs.h"
#include "objects/synonyms/synonymfuncs.h"
#include "objects/tables/tablefuncs.h"
#include "objects/views/viewfuncs.h"
#include "objects/datafiles/datafilefuncs.h"
#include "objects/sessions/sessionfuncs.h"
#include "objects/procedures/procfuncs.h"
#include "objects/functions/functionfuncs.h"
#include "objects/types/typefuncs.h"
#include "objects/packages/packagefuncs.h"
#include "objects/triggers/triggerfuncs.h"
#include "objects/sequences/sequencefuncs.h"
#include "objects/tablespaces/tablespacefuncs.h"
#include "objects/security/securityfuncs.h"
#include "objects/rollbacks/rollbackfuncs.h"
#include "objects/logfiles/logfilefuncs.h"

#include <sys/time.h>

static GdkPixmap *folder_pixmap=NULL, *ofolder_pixmap;
static GdkBitmap *folder_mask, *ofolder_mask;
GtkWidget *obj_create_dblwin(void);
GtkWidget *obj_create_twwinmain(void);
GtkWidget *obj_create_indexwin(void);
GtkWidget *obj_create_synwin(void);
GtkWidget *obj_create_viewwin(void);
void obj_getdatafiles_ts(GtkCTreeNode *, GtkCTreeNode *, char *);
void obj_getsessions(GtkCTreeNode *, GtkCTreeNode *, char *);
GtkWidget *dfwindow1;

#include <xpm/foldericon.xpm>
#include <xpm/ofoldericon.xpm>

DBObject Objects[] = {
	{"Datafiles", 
	 NULL, get_special_owners, "DATAFILES",
	 loaduserdatafiles, get_datafile_info,
	 show_generic_datafile_window,
	 create_datafilewin, OMDB_Datafile},
	{"DBLinks",   
	 NULL, get_dblink_owners, "db_links",
	 loaduserdblinks, get_dblink_info,
	 show_generic_dblink_window,
	 obj_create_dblwin, OMDB_DBLink},
	{"Functions", 
	 NULL, obj_getowners, "FUNCTION",
	 loaduserfunctions, get_function_info,
	 show_generic_function_window,
	 create_function, OMDB_Function},
	{"Indexes",   
	 create_oindex_menu, obj_getowners, "INDEX",
	 loaduserindexes, get_index_info,
	 show_generic_index_window,
	 obj_create_indexwin, OMDB_Index},
	{"Logfiles", 
	 NULL, get_logfile_groups, "LOGFILE",
	 loaduserlogfiles, get_logfile_info, 
	 show_generic_logfile_window,
	 create_logfile_window1, OMDB_Logfile},
	{"Packages",  
	 NULL, obj_getowners, "PACKAGE",
	 loaduserpackages, get_package_info,
	 show_generic_package_window,
	 create_package, OMDB_Package},
	{"Procedures",
	 create_oprocedure_menu, obj_getowners, "PROCEDURE",
	 loaduserprocs, get_proc_info,
	 show_generic_procedure_window, 
	 create_procedure, OMDB_Procedure},
	 {"Rollbacks",
	 NULL, get_special_owners, "ROLLBACKS",
	 loadrollbacks, get_rollback_info,
	 show_generic_rollback_window,
	 create_rollbackwin, OMDB_Rollback},
	 {"Security",     
	 NULL, get_security_owners, "SECURITY",
	 load_security_objects, get_security_object_info,
	 show_generic_security_window,
	 create_security_object, OMDB_Security},
	{"Sequences", 
	 NULL, obj_getowners, "SEQUENCE",
	 loadusersequences, get_sequence_info, 
	 show_generic_sequence_window,
	 create_seqwin_window1, OMDB_Sequence},
	{"Sessions",  
	 NULL, get_special_owners, "SESSIONS",
	 loadusersessions, get_session_info,
	 show_generic_session_window,
	 NULL, OMDB_Session},
	{"Synonyms",  
	 create_osynonym_menu, obj_getowners, "SYNONYM",
	 loadusersynonyms, get_synonym_info, 
	 show_generic_synonym_window,
	 obj_create_synwin, OMDB_Synonym},
	{"Tables",    
	 create_otab_menu, obj_getowners, "TABLE",
	 loadusertables, get_table_info, 
	 show_generic_table_window,
	 get_twwinmain_widget, OMDB_Table},
	{"Tablespaces", 
	 create_otablespace_menu, obj_gettablespaces, NULL,
	 NULL, NULL,
	 show_generic_tablespace_window,
	 create_tablespace, OMDB_Tablespace},
	{"Triggers",  
	 NULL, obj_getowners, "TRIGGER",
	 loadusertriggers, get_trigger_info,
	 show_generic_trigger_window,
	 create_trigger_window1, OMDB_Trigger},
	{"Types",     
	 NULL, obj_getowners, "TYPE",
	 loadusertypes, get_type_info,
	 show_generic_type_window,
	 obj_create_typewin, OMDB_Type},
	{"Views",     
	 create_oview_menu, obj_getowners, "VIEW",
	 loaduserviews, get_view_info,
	 show_generic_view_window,
	 obj_create_viewwin, OMDB_View}
};



void
get_object_list(
    DBObject **objects,
    int      *nobjects)
{
    assert(objects != 0);
    assert(nobjects != 0);
    *objects = Objects;
    *nobjects = sizeof(Objects)/sizeof(Objects[0]);
}

/* Map object type name to integer code */
int
obj_getctype(
    char	 *object_type)
{
    char	 *objecttypes[]	 = {
	"Tables", "Sequences", 	"Synonyms", "Indexes", "Views",     
	"Procedures", "Functions", "Types", "Packages", "Triggers",  
	"DBLinks", "Datafiles", "Sessions", "Tablespaces", "Security",
	"Rollbacks", "Logfiles"};
    int i;

    obj_cinfo.currenttype=0;
    /*gtk_widget_set_sensitive(obj_toolbarbuttons.button2,0);*/

    for (i = 0; i < sizeof(objecttypes)/ sizeof(objecttypes[0]); i++) {
	if(strcasecmp(object_type,objecttypes[i]) == 0) {
	    obj_cinfo.currenttype=i+1;
	    break;
	}
    }
    return obj_cinfo.currenttype;
}

void
obj_create_node(
    GtkCTreeNode  *root_node,
    DBObject	  *obj)
{
    DirNode	  *dirnode;
    GtkCTreeNode  *node;

    if (!folder_pixmap)
	folder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, folder);
    if (!ofolder_pixmap)
	ofolder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &ofolder_mask, NULL, ofolder);

    node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), root_node, 
				 NULL, &(obj->labeltext), 4, 
				 folder_pixmap, folder_mask, 
				 ofolder_pixmap, ofolder_mask, 
				 FALSE, FALSE);
    dirnode = new_dirnode(NULL, NULL);
    dirnode->path=g_malloc(strlen(obj_cinfo.login->dbid)+strlen(obj->labeltext)+3);
    sprintf(dirnode->path,"%s->%s",obj_cinfo.login->dbid, obj->labeltext);
    dirnode->scanned = FALSE;
    dirnode->menu_creator = obj->menu_creator;
    dirnode->db_object = obj;
    gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
    if (obj->list_owners)
	obj->list_owners(root_node,node,obj->list_owners_type);
}

void
obj_create_nodes(GtkCTreeNode *root_node)
{
    int		  i;
    int		  nitems;


    obj_progwin.window4 = obj_create_progressbar();
    gtk_widget_show (obj_progwin.window4);
    gtk_widget_realize (obj_progwin.label1);
   

    gtk_label_set_text(GTK_LABEL (obj_progwin.label1), "Scanning Database, please wait");

    gtk_clist_freeze(GTK_CLIST(obj_mainwin.tree1));
    
    nitems = sizeof(Objects)/sizeof(Objects[0]);
    for (i = 0; i < nitems; i++) {
	assert(Objects[i].omdb_type == obj_getctype(Objects[i].labeltext));
	obj_create_node(root_node, &(Objects[i]));
    }
      
    /* clear out object/owners cache */
    obj_reset_owners();

    gtk_clist_thaw(GTK_CLIST(obj_mainwin.tree1));
   
    gtk_progress_bar_update( GTK_PROGRESS_BAR(obj_progwin.progressbar1),.99);
    gtk_widget_realize (obj_progwin.progressbar1);
    while (gtk_events_pending())
	gtk_main_iteration();
	    
    gtk_widget_destroy (obj_progwin.window4);
    obj_progwin.window4 = 0;
    obj_progwin.progressbar1 = 0;
    obj_progwin.label1 = 0;
}

