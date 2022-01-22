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
#include "objects/tables/tablefuncs.h"
#include "indexfuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void
obj_on_ndxanalyze_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    int       num_elements  = 0;
    char      *dbid, *owner,*object;
    char      *objecttype,*path_elements[4];
    char      *otype=NULL;
    char      *query=NULL;
    DirNode   *dir          = (DirNode *)user_data;
    char      qtemplate[]   = "ANALYZE %s %s.%s %s";
    char      *success_msg;
    
    success_msg = "Somethin' ain't right";
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    objecttype = path_elements[DIRPATH_TYPE];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];
    
    if(!strcmp(objecttype,"Tables"))
        otype = "TABLE";
    if(!strcmp(objecttype,"Indexes"))
        otype = "INDEX";
    
    if(otype==NULL)
        return;
    
    switch(analyzewin()) {
        case 0: {
            query = g_malloc(strlen(qtemplate) + strlen(owner) + 
	                     strlen(object)    + strlen(otype) +
			     strlen("COMPUTE STATISTICS")   + 1);
            sprintf(query, qtemplate, otype, owner, object, "COMPUTE STATISTICS");
	    success_msg = "Analysis Computed Successfully";
	    break;
    
        }
	case 1: {
	    query = g_malloc(strlen(qtemplate) + strlen(owner) + 
	                     strlen(object)    + strlen(otype) +
			     strlen("ESTIMATE STATISTICS")   + 1);
            sprintf(query, qtemplate, otype, owner, object, "ESTIMATE STATISTICS");
	    success_msg = "Analysis Estimated Successfully";
	    break;
	
	}
	case 2: {
	    query = g_malloc(strlen(qtemplate) + strlen(owner) + 
	                     strlen(object)    + strlen(otype) +
			     strlen("DELETE STATISTICS")   + 1);
            sprintf(query, qtemplate, otype, owner, object, "DELETE STATISTICS");
	    success_msg = "Statistics Deleted Successfully";
	    break;
	
	}
	case 3: {
	    query = g_malloc(strlen(qtemplate) + strlen(owner) + 
	                     strlen(object)    + strlen(otype) +
			     strlen("VALIDATE REF UPDATE")   + 1);
            sprintf(query, qtemplate, otype, owner, object, "VALIDATE REF UPDATE");
	    success_msg = "References Validated Successfully";
	    break;
	
	}
    }
    
    if (execute_immediate_sql(dbid,query)) {
	msgbox(success_msg);
    }

    g_free(query);
    free_path_elements(path_elements, num_elements);    
}

void
obj_on_ndxrename_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_show (obj_create_renamewin(user_data));
}

void
obj_on_ndxrebuild_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    DirNode	 *dir	    = (DirNode *)user_data;
    int		 num_elements=0;
    char	 *query,*owner,*object,*dbid,*path_elements[4];
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];
    
    query = g_malloc(strlen(owner) + strlen(object) + 30);
        
    sprintf(query,"ALTER INDEX %s.%s REBUILD",owner,object);
    
    if (execute_immediate_sql(dbid, query)) {
	msgbox("Rebuild completed");
    }
    
    g_free(query);
    free_path_elements(path_elements, num_elements);
}

void
obj_on_index_unusable_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    DirNode	 *dir	    = (DirNode *)user_data;
    int		 num_elements=0;
    char	 *query,*owner,*object,*dbid,*path_elements[4];
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];
    
    query = g_malloc(strlen(owner) + strlen(object) + 30);
    
    if (GTK_TOGGLE_BUTTON (button)->active==0)
        sprintf(query,"alter index %s.%s unusable",owner, object);
    else
        sprintf(query,"alter index %s.%s rebuild",owner, object);
    
    if (execute_immediate_sql(dbid, query)) {
	msgbox("Index Altered");
    }
    
    g_free(query);
    free_path_elements(path_elements, num_elements);

}

void
on_index_unallocate_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    int          num_elements=0;
    char         *dbid, *owner,*object,*path_elements[4];
    char         *query;
    DirNode      *dir  = (DirNode *)user_data;
    char         qtemplate[] = "ALTER INDEX %s.%s DEALLOCATE UNUSED";

    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];

    query = g_malloc(strlen(qtemplate) + strlen(owner) + strlen(object) + 1);
    sprintf(query, qtemplate, owner, object);
    
    if(execute_immediate_sql(dbid, query)) {
	msgbox("Index Deallocated");
    }
    
    g_free(query);
    free_path_elements(path_elements, num_elements);
}

