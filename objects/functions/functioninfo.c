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
#include "objects/procedures/procfuncs.h"
#include "objects/functions/functionfuncs.h"
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

static GdkPixmap *proc_pixmap=NULL;
static GdkBitmap *proc_mask;

#include <xpm/procicon.xpm>

void 
loaduserfunctions(
    GtkCTreeNode  *parent_node,
    char	  *owner, 
    const char	  *dbversion,
    const char	  *dbid)
{
    if (!proc_pixmap)
	proc_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window,
						       &proc_mask, NULL, procpixmap);
    loadusercode(parent_node, dbid, owner, "FUNCTION", 
		 NULL, NULL, proc_pixmap, proc_mask, NULL, NULL);
}

void
get_function_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *proc_name,
    DirNode	     *dir)
{
    char *procsource;
    
    obj_ginfo.procfilename=NULL;

    procsource = getprocsource(dbid, owner, proc_name, "FUNCTION");
    if(procsource != NULL){
        gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
        obj_create_proc_toolbar(dir, 0);
        obj_ginfo.toolbar_owner = OMDB_Procedure;
    
        gtk_widget_destroy(obj_mainwin.notebook1);
        obj_doprocview();
    
        getprocstatus(owner,proc_name,dbid,0);
    
        gtk_text_freeze (GTK_TEXT (obj_mainwin.proctext1));
        gtk_text_insert(GTK_TEXT(obj_mainwin.proctext1), NULL, NULL, NULL, procsource, -1);
	if(obj_ginfo.highlighting)
	    gtk_editor_hilite_buffer (GTK_EDITOR(obj_mainwin.proctext1));
        gtk_text_thaw (GTK_TEXT (obj_mainwin.proctext1));
    }else{
        gtk_widget_destroy(obj_mainwin.notebook1);
        obj_doorasoft();
        gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
        obj_create_blank_toolbar();
        obj_ginfo.toolbar_owner = OMDB_None;
    }
}

void
show_generic_function_window(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner)
{
    GtkCList	     *list;
    char	     *query;
    static char	     qtemplate[] = 
	"SELECT owner \"Schema\", object_name               "
	"\"Function\", TO_CHAR(created, 'DD-MON-YYYY')     "
	"\"Created\", TO_CHAR(last_ddl_time, 'DD-MON-YYYY') "
	"\"Last Modified\", NLS_INITCAP(status) \"Status\"  "
	"FROM all_objects  WHERE object_type =              "
	"'FUNCTION' and owner = '%s' ORDER by object_name  ";

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    list = clist_from_query(query, dbid, 0, "<NULL>", active_step);
    active_clear();
    g_free(query);

    gtk_widget_destroy(obj_mainwin.notebook1);
    glist_set_opt_col_width(list, 0);
    glist_set_opt_col_width(list, 1);
    glist_set_opt_col_width(list, 2);
    glist_set_opt_col_width(list, 3);
    
    obj_do_generic_view(list, "Functions");
}

