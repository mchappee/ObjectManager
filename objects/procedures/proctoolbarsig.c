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
#include "procfuncs.h"
#include "common-files/common.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


void
on_procsearch_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_show(create_search_window (GTK_TEXT(obj_mainwin.proctext1)));
}

void
on_procbutton1_pressed(
    GtkButton       *button,
    gpointer         user_data)
{
    char *filename,*buffer;
    FILE *stream;
    struct stat sb;

    filename=filewin("Select File");
    if(filename!=NULL){
	stream=fopen(filename,"r");
	if(stream==NULL){
	    fatal_error("File IO error");
	    return;  
	}

	stat(filename,&sb);
	buffer=g_malloc(sb.st_size+1);
	fread(buffer, 1, sb.st_size, stream);
	clear_text(obj_mainwin.proctext1);
	gtk_text_insert(GTK_TEXT(obj_mainwin.proctext1), NULL, NULL, NULL, buffer, sb.st_size);
	g_free(buffer);
	g_free(filename);  
	fclose(stream);
    }
}


void
on_procbutton2_pressed(
    GtkButton	 *button,
    gpointer	 user_data)
{
    FILE	 *stream;
    char	 *cp;

    if(obj_ginfo.procfilename==NULL)
	obj_ginfo.procfilename=filewin("Select File");

    cp = gtk_editable_get_chars(GTK_EDITABLE (obj_mainwin.proctext1),(gint) 0,
				(gint) gtk_text_get_length (GTK_TEXT (obj_mainwin.proctext1)));
    
    if(obj_ginfo.procfilename!=NULL){
        stream=fopen(obj_ginfo.procfilename,"w");
        if(stream==NULL){ 
	    fatal_error("File IO error");
        } else { 
            fprintf(stream,"%s",cp);
        fclose(stream);
        }
	
    }
    
    g_free(cp);
      
}

void
on_procbutton_execute_pressed(
    GtkButton	 *button,
    gpointer	 user_data)
{
    int		 num_elements=0;
    char	 *dbid, *owner,*object,*path_elements[4];
    DirNode	 *dir	    = (DirNode *)user_data;
    char         *query;
    GtkCList     *list;
    char         *object_id;
    char         argtemplate[] = "SELECT argument_name, data_type "
                                 "FROM all_arguments "
				 "WHERE owner = '%s' "
				 "AND object_name = '%s' "
				 "AND package_name IS NULL "
				 "ORDER BY sequence";

    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];
    
    query = g_malloc(sizeof(argtemplate) + strlen(owner) + strlen(object) + 1);
    sprintf(query, argtemplate, owner, object);

    object_id = g_malloc(strlen(owner) + strlen(object) + 2);
    sprintf(object_id,"%s.%s",owner,object);

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    gtk_widget_show(create_execute_statement(list, object_id));
    
    g_free(object_id);
    gtk_widget_destroy(GTK_WIDGET(list));
    free_path_elements(path_elements, num_elements);
}


void
on_procbutton3_pressed(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *cp,*printcommand;
    FILE	 *stream;
    GString	 *printbuf  = g_string_new("\n\n\n");
    int		 num_elements=0;
    char	 *owner,*object,*path_elements[4];
    DirNode	 *dir	    = (DirNode *)user_data;

    num_elements=parse_path(dir->path, path_elements, 4);
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];

    if(num_elements > 3){
        printcommand=printwin(obj_ginfo.printcall);

        if(printcommand!=NULL){
 
	    cp = gtk_editable_get_chars(GTK_EDITABLE (obj_mainwin.proctext1),(gint) 0,
				    (gint) gtk_text_get_length (GTK_TEXT (obj_mainwin.proctext1)));

	    g_string_sprintfa(printbuf,"Procedure: %s.%s\n\n%s\n",owner,object,cp);

	    stream=popen(printcommand,"w");
	    if (stream) {
	        fprintf(stream,"%s", printbuf->str);
	        pclose(stream);
	    }
	    g_free(printcommand);
	    g_free(cp);
	    g_string_free(printbuf, 1);
        }
    }
    free_path_elements(path_elements, num_elements);
}

void
on_procbutton4_pressed(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *cp;
    int		 num_elements, package_body;
    const char	 *dbid;
    const char	 *owner,*object,*otype;
    char	 *path_elements[4];
    DirNode	 *dir	  = (DirNode *)user_data;

    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    otype = path_elements[DIRPATH_TYPE];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];

    if(num_elements > 3){
        if(obj_mainwin.notebook1 != NULL && strncmp(otype,"Packages",8)==0)
            package_body = gtk_notebook_get_current_page(GTK_NOTEBOOK(obj_mainwin.notebook1));
        else
            package_body = 0;

        cp = gtk_editable_get_chars(GTK_EDITABLE (obj_mainwin.proctext1),(gint) 0,
				(gint) gtk_text_get_length (GTK_TEXT (obj_mainwin.proctext1)));

        execute_immediate_sql(dbid, cp);
        g_free(cp);

        getprocstatus(owner,object,dbid,package_body);
        gtk_label_set_text(GTK_LABEL (obj_mainwin.proclabelchanged)," ");
   
    }
    free_path_elements(path_elements, num_elements);
}

void
on_procbutton5_pressed(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *dbid,*query;
    int		 num_elements=0;
    char	 *owner,*object,*otype,*path_elements[4];
    DirNode	 *dir	  = (DirNode *)user_data;
    GtkCList     *list;
    char	 qtemplate[] = 
	"SELECT   text,line,position,sequence "
	"FROM     all_errors "
	"WHERE    name = '%s'"
	"         AND owner = '%s' "
	"ORDER BY sequence ASC";
	
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    otype = path_elements[DIRPATH_TYPE];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + strlen(object) + 1);
    sprintf(query, qtemplate, object,owner);

    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);
    
    msgbox_x(NULL,578,300,0,list);
    
    free_path_elements(path_elements, num_elements);
    return;
      
}

void
on_procbutton6_pressed(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *dbid,*query;
    int		 line=0,position=0;
    int		 num_elements=0, ebyte=0, len=0;
    char	 *owner,*object,*otype,*path_elements[4];
    char         *charline, *charposition;
    DirNode	 *dir	  = (DirNode *)user_data;
    GtkCList     *owners;
    char	 qtemplate[] = 
	"SELECT   line,position "
	"FROM     all_errors "
	"WHERE    name = '%s'"
	"         AND owner = '%s' "
	"ORDER BY sequence ASC";
	
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    otype = path_elements[DIRPATH_TYPE];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];

    if(num_elements > 3){

        len = gtk_text_get_length(GTK_TEXT(obj_mainwin.proctext1));

        query = g_malloc(sizeof(qtemplate) + strlen(owner) + strlen(object) + 1);
        sprintf(query, qtemplate, object,owner);

        owners = clist_from_query(query, dbid, 0, "<NULL>", NULL);       
        g_free(query);
	
	if(owners->rows){
	    gtk_clist_get_text(owners, 0, 0, &charline);
	    gtk_clist_get_text(owners, 0, 1, &charposition);
            line = atoi(charline);
	    position = atoi(charposition);	    
	
            gtk_widget_destroy(GTK_WIDGET(owners));		
	
            ebyte = get_error_byte(line,position,owner);
            if(ebyte > 0 && ebyte < len+1){
                gtk_editable_set_position (GTK_EDITABLE (obj_mainwin.proctext1), ebyte);
                gtk_editable_select_region (GTK_EDITABLE (obj_mainwin.proctext1),ebyte+6,ebyte-1);    
            }
    
        }
    
    }
    
    free_path_elements(path_elements, num_elements);
    return;

}

void 
getprocstatus(
    const char	 *owner, 
    const char	 *object, 
    const char	 *qdbid,
    int          pbflag)
{
    const char	 *dbid;
    char	 *tstatus,*query;
    GString      *packagebody;
    char	 *text;
    GList	 *status;
    char	 qtemplate[] = 
	"SELECT   status "
	"FROM     all_objects "
	"WHERE    object_name = '%s' "
	"         AND owner = '%s'"
	"         AND object_type not like '%% %%'";
    char	 qpbtemplate[] = 
	"SELECT   status "
	"FROM     all_objects "
	"WHERE    object_name = '%s' "
	"         AND owner = '%s'"
	"         AND object_type = 'PACKAGE BODY'";


    dbid = qdbid;

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + strlen(object) + 10);

    /* pbflag > 0 means we're dealing with a package body */
    if(pbflag==0)
      sprintf(query, qtemplate, object,owner);
    else
      sprintf(query, qpbtemplate, object,owner);
    
    status = glist_from_query(query, dbid, 0, TRUE, NULL);
    g_free(query);

    /* Only one possible return */
    if(status != NULL){
        text = status->data;
        g_list_free (status);

        tstatus = g_malloc(strlen(object) + 32);

        if(strncmp(text,"INVALID",7) != 0){
             sprintf(tstatus,"  %s         VALID",object);
	     if(GTK_IS_WIDGET(obj_mainwin.procbutton5)){
	       gtk_widget_hide (obj_mainwin.procbutton5);
	       gtk_widget_hide (obj_mainwin.procbutton6);
	       gtk_label_set_text(GTK_LABEL (obj_mainwin.proclabelvalid),"Valid");
	     }
        }else{
	     sprintf(tstatus,"  %s         INVALID",object);
	     if(GTK_IS_WIDGET(obj_mainwin.procbutton5)){
	         gtk_widget_show (obj_mainwin.procbutton5);
	         gtk_widget_show (obj_mainwin.procbutton6);
		 gtk_label_set_text(GTK_LABEL (obj_mainwin.proclabelvalid),"Invalid");
	     }
        }  

        g_free(tstatus);

    }else{
        /* This is a little strange.  If we get here then we have a Package without */
        /* a Package Body.  Since the next logical thing to do is create one, we'll */
        /* just take the liberty of doing so. Also, Package Body isn't a valid OM   */
        /* object, so there's no way to create one.                                 */
	
	if(confirm("There is no Package Body as of yet.  Would you like "
	           "for me to create one based on the Package header information?")){ 
            /* get_package_body is at the end of this file */
	    packagebody = get_package_body(owner,object,dbid);
	
            if(packagebody != NULL){
	        query = g_malloc(strlen(owner) + strlen(object) + packagebody->len + 60);
                sprintf(query,"CREATE OR REPLACE PACKAGE BODY %s.%s \nAS\n\n%s\n\nEND;\n",owner,object,packagebody->str);
            }else{
	        query = g_malloc(strlen(owner) + strlen(object) + 60);
	        sprintf(query,"CREATE OR REPLACE PACKAGE BODY %s.%s \nAS\n\n\n\nEND;\n",owner,object);
	    }	
            execute_immediate_sql(dbid, query);
            clear_text(obj_mainwin.proctext1);
            gtk_text_insert(GTK_TEXT(obj_mainwin.proctext1), NULL, NULL, NULL, query, strlen(query));
	    g_free(query);
	}else{
	    query = "No Package Body Available";
	    clear_text(obj_mainwin.proctext1);
            gtk_text_insert(GTK_TEXT(obj_mainwin.proctext1), NULL, NULL, NULL, query, strlen(query));
	    gtk_widget_set_sensitive(obj_mainwin.dynamictoolbar1,0);
	    obj_ginfo.toolbar_owner = OMDB_None;
	    
	}
    }
}


int 
get_error_byte(
    int    line,
    int    position,
    char   *owner)
{
    int i,len=0,a=0;
    char *cp;
    
    cp = gtk_editable_get_chars(GTK_EDITABLE (obj_mainwin.proctext1),(gint) 0,
				(gint) gtk_text_get_length (GTK_TEXT (obj_mainwin.proctext1)));
				
    len=strlen(cp);
    for (i = 0; i <= len; i++){
        if(cp[i]=='\n')
	    a++;
	 if(a==(line-1))
	    break;
   }
   
   g_free(cp);
   return (i+1+position);

}

GString *
get_package_body(
    const char    *owner,
    const char    *package,
    const char    *dbid)
{
    char	     *query;
    char	     *text;
    GList	     *packages;
    GList	     *p;
    GString	     *arglist = g_string_new(" ");
    char	     qtemplate[] = 
	"SELECT DISTINCT object_name "
	"FROM all_arguments "
	"WHERE owner = '%s'"
	"      AND package_name = '%s'";

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + strlen(package) + 1);
    sprintf(query, qtemplate, owner, package);
    packages = glist_from_query(query, dbid, 0, TRUE, NULL);
    g_free(query);

    if(packages != NULL){
        for (p = packages; p; p = g_list_next(p)) { 
            text = p->data;
            g_string_sprintfa(arglist, "\n/* Start Procedure %s */\n%s(  )\nAS\nBEGIN\n"
	                               "/* Procedure Body Here */\nEND;\n",text,text);
            g_free(p->data);
        }
    }else
        arglist = NULL;
	
    g_list_free (packages);
    return arglist;
}

void
obj_on_trigger_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    DirNode	 *dir	  = (DirNode *)user_data;
    int          num_elements = 0;
    char         *path_elements[4];
    char         *dbid, *otype, *owner, *object;
    char         *query;

    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    otype = path_elements[DIRPATH_TYPE];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];
    
    query = g_malloc(strlen(owner) + strlen(object) + 30);
    
    if (GTK_TOGGLE_BUTTON (obj_mainwin.toggletrigger)->active==0){
        sprintf(query,"alter trigger %s.%s enable",owner,object);
	if(execute_immediate_sql(dbid, query))
	    msgbox("The trigger has been enabled");
    }else{
        sprintf(query,"alter trigger %s.%s disable",owner,object);
	if(execute_immediate_sql(dbid, query))
	    msgbox("The trigger has been disabled");
    }

    g_free(query);
    free_path_elements(path_elements, num_elements);

}

void 
get_trigger_status(
    const char	 *owner, 
    const char	 *object, 
    const char	 *qdbid,
    DirNode      *data)
{
    const char	 *dbid;
    char	 *tstatus,*query;
    char	 *text;
    GList	 *status;
    char         qtriggerstatus[] = 
        "SELECT   status "
	"FROM     all_triggers "
	"WHERE    trigger_name = '%s' "
	"AND      owner = '%s'";

    dbid = qdbid;

    query = g_malloc(sizeof(qtriggerstatus) + strlen(owner) + strlen(object) + 1);
    sprintf(query, qtriggerstatus, object, owner);
    
    status = glist_from_query(query, dbid, 0, TRUE, NULL);
    g_free(query);

    if(status != NULL){
        text = status->data;	
        g_list_free (status);

        tstatus = g_malloc(strlen(object) + 32);

        gtk_signal_handler_block_by_func (GTK_OBJECT (obj_mainwin.toggletrigger),
            GTK_SIGNAL_FUNC (obj_on_trigger_clicked),(gpointer)data);
	
        if(strncmp(text,"ENABLED",7) == 0){
	    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (obj_mainwin.toggletrigger), TRUE);
        }else{
	    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (obj_mainwin.toggletrigger), FALSE);
        }  
			
        g_free(tstatus);

    }

}
