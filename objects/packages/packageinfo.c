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
#include "objects/tables/tablefuncs.h"
#include "packagefuncs.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <signal.h>

static GdkPixmap *pkg_open_pixmap=NULL;
static GdkBitmap *pkg_open_mask;

#include <xpm/openpackageicon.xpm>

static GdkPixmap *pkg_pixmap=NULL;
static GdkBitmap *pkg_mask;

#include <xpm/closedpackageicon.xpm>

static GdkPixmap *proc_pixmap=NULL;
static GdkBitmap *proc_mask;

#include <xpm/procicon.xpm>

void 
loaduserpackages(
    GtkCTreeNode     *parent_node,
    char	     *owner, 
    const char	     *dbversion,
    const char	     *dbid)
{
    if (!pkg_open_pixmap)
	pkg_open_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window,
						       &pkg_open_mask, NULL, pkgopen);

    if (!pkg_pixmap)
	pkg_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window,
						       &pkg_mask, NULL, pkgclosed);

    loadusercode(parent_node, dbid, owner, "PACKAGE", 
		 NULL, on_package_expand_cb,
		 pkg_pixmap, pkg_mask,
		 pkg_open_pixmap, pkg_open_mask);
}

void
get_package_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *proc_name,
    DirNode	     *dir)
{
    char	     *path_elements[2];
    int		     num_elements;
    char             *create_path_elements;

    gtk_widget_destroy(obj_mainwin.notebook1);

    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_proc_toolbar(dir, 0);
    obj_ginfo.toolbar_owner = OMDB_Package;

    num_elements = parse_path(proc_name, path_elements, 2);
    
    if (num_elements == 1) {
        char *procsource;
    
        obj_ginfo.procfilename=NULL;

        procsource = getprocsource(dbid, owner, proc_name, "PACKAGE");
        if(procsource != NULL){
            create_path_elements=g_malloc(strlen(owner)+strlen(dbid)+strlen(path_elements[0])+5);
	    sprintf(create_path_elements,"%s->%s->%s",owner,dbid,path_elements[0]);

            obj_dopackageview(create_path_elements);

            g_free(create_path_elements);
            getprocstatus(owner,path_elements[0],dbid,0);
	    
	    gtk_text_insert(GTK_TEXT(obj_mainwin.proctext3), NULL, NULL, NULL, procsource, -1);
	    if(obj_ginfo.highlighting)
	        gtk_editor_hilite_buffer (GTK_EDITOR(obj_mainwin.proctext3));
            g_free(procsource);

            procsource = getprocsource(dbid, owner, proc_name, "PACKAGE BODY");
            gtk_text_insert(GTK_TEXT(obj_mainwin.proctext2), NULL, NULL, NULL, procsource, -1);
	    if(obj_ginfo.highlighting)
	        gtk_editor_hilite_buffer (GTK_EDITOR(obj_mainwin.proctext2));
            g_free(procsource);
        }else{
            gtk_widget_destroy(obj_mainwin.notebook1);
            obj_doorasoft();
            gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
            obj_create_blank_toolbar();
            obj_ginfo.toolbar_owner = OMDB_None;
        }

    } else {
	char	     *query;
	GtkCList     *args;
	static char  qtemplate[] = 
	    "SELECT   in_out \"In/Out\", "
	    "         argument_name \"Name\", "
	    "         data_type \"Type\", "
	    "         data_length \"Length\", "
	    "         data_precision \"Precision\" "
	    "         "
	    "FROM     all_arguments "
	    "WHERE    owner = '%s'"
	    "         AND object_name = '%s'"
	    "         AND package_name = '%s' "
	    "ORDER BY position";
	query = g_malloc(sizeof(qtemplate) + strlen(owner) +
			 strlen(path_elements[0]) + strlen(path_elements[1]));
	sprintf(query, qtemplate, owner, path_elements[1], path_elements[0]);
	args = clist_from_query(query, dbid, 0, "", NULL);
	glist_set_opt_col_width(args, 0);
	glist_set_opt_col_width(args, 1);
	glist_set_opt_col_width(args, 2);
	g_free(query);
	
	obj_dopackageprocview(args);
	gtk_widget_show(GTK_WIDGET(args));
    }
}

void 
loadpackageprocs(
    GtkCTreeNode     *parent_node,
    const char	     *dbid,
    const char	     *owner,
    const char	     *package)
{
    char	     *query;
    GtkCTreeNode     *node;
    DirNode	     *dirnode;
    DirNode	     *parent_dirnode;
    char	     *text;
    GList	     *packages;
    GList	     *p;
    char	     qtemplate[] = 
	"SELECT DISTINCT object_name "
	"FROM all_arguments "
	"WHERE owner = '%s'"
	"      AND package_name = '%s'";

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + strlen(package) + 1);
    sprintf(query, qtemplate, owner, package);
    packages = glist_from_query(query, dbid, 0, TRUE, NULL);
    g_free(query);

    if (!proc_pixmap)
	proc_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window,
						       &proc_mask, NULL, procpixmap);

    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    for (p = packages; p; p = g_list_next(p)) {
	dirnode = new_dirnode(parent_dirnode, p->data);
	dirnode->menu_creator = NULL;

	text = p->data;
	node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4, 
				     proc_pixmap, proc_mask, NULL, NULL, TRUE, TRUE);
		
	gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	g_free(p->data);

    }
    g_list_free (packages);
}

void
show_generic_package_window(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner)
{
    GtkCList	     *list;
    char	     *query;
    static char	     qtemplate[] = 
	"SELECT owner \"Schema\", object_name               "
	"\"Package\", TO_CHAR(created, 'DD-MON-YYYY')     "
	"\"Created\", TO_CHAR(last_ddl_time, 'DD-MON-YYYY') "
	"\"Last Modified\", NLS_INITCAP(status) \"Status\"  "
	"FROM all_objects  WHERE object_type =              "
	"'PACKAGE' and owner = '%s' ORDER by object_name  ";

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
    
    obj_do_generic_view(list, "Package");
}

