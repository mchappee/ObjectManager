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
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void
obj_on_sesbutton3_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    int		 b=0;
    char	 *tmp0,*tmp1,*tmp2;
    char	 *cp,*printcommand;
    FILE	 *stream;
    GString	 *printbuf  = g_string_new("\n\n\n");

    printcommand=printwin(obj_ginfo.printcall);
    
    if(printcommand!=NULL){
 	while (b < 24){
	    gtk_clist_get_text(GTK_CLIST(user_data), b, 0, &tmp0);
	    gtk_clist_get_text(GTK_CLIST(user_data), b, 1, &tmp1);
	    gtk_clist_get_text(GTK_CLIST(user_data), b, 2, &tmp2);
	
	    g_string_sprintfa(printbuf, "%13.13s %-20.20s %-40.40s\n",tmp0==NULL?" ":tmp0,tmp1==NULL?" ":tmp1,tmp2==NULL?" ":tmp2);
	    b++;
	    tmp0=NULL;tmp1=NULL;tmp2=NULL;
	}

	cp = gtk_editable_get_chars(GTK_EDITABLE (obj_mainwin.sessiontext1),(gint) 0,
				    (gint) gtk_text_get_length (GTK_TEXT (obj_mainwin.sessiontext1)));

	g_string_append(printbuf,"\n\nExecuting Sql:\n");      
	g_string_sprintfa(printbuf,"%s\n",cp);

	stream=popen(printcommand,"w");
	if (stream) {
	    fprintf(stream,"%s", printbuf->str);
	    pclose(stream);
	}
	g_string_free(printbuf, 1);
	g_free(cp);
	g_free(printcommand);
    }
}

void
obj_on_sesbutton4_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *killstring;
    char	 *serial;
    char	 *sid;

    gtk_clist_get_text(GTK_CLIST(obj_mainwin.sessionclist2), 9, 2, &sid);
    gtk_clist_get_text(GTK_CLIST(obj_mainwin.sessionclist2), 10, 2, &serial);
    
    if( confirm("You are about to disconnect a user.\n"
		"This will probably cause your phone to ring.\n\n"
		"Are you sure?")) {
	killstring=g_malloc(strlen(sid)+strlen(serial)+100);
	sprintf(killstring,"ALTER SYSTEM DISCONNECT SESSION '%s,%s' POST_TRANSACTION",
		sse(sid),sse(serial));

	execute_immediate_sql(obj_cinfo.login->dbid, killstring);
	g_free(killstring);
    }
}

void
obj_on_sesbutton5_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *killstring;
    char	 *serial;
    char	 *sid;

    gtk_clist_get_text(GTK_CLIST(obj_mainwin.sessionclist2), 9, 2, &sid);
    gtk_clist_get_text(GTK_CLIST(obj_mainwin.sessionclist2), 10, 2, &serial);
    
    if (confirm("You are about to kill a user's session.\n"
		"This will probably cause your phone to ring.\n\n"
		"Are you sure?")) {
	killstring=g_malloc(strlen(sid)+strlen(serial)+100);
	sprintf(killstring,"ALTER SYSTEM KILL SESSION '%s,%s'",
		sse(sid),sse(serial));
	printf("\nkillstring:-%s-",killstring);
	execute_immediate_sql(obj_cinfo.login->dbid, killstring);
	g_free(killstring);
    }
}



