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
#include "objects/security/securityfuncs.h"
#include "objects/tables/tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

static GdkPixmap *folder_pixmap=NULL, *ofolder_pixmap;
static GdkBitmap *folder_mask, *ofolder_mask;

#include <xpm/ofoldericon.xpm>
#include <xpm/foldericon.xpm>
#include <xpm/usericon.xpm>

void
get_security_owners(
    GtkCTreeNode  *rootnode,
    GtkCTreeNode  *topnode,
    char	  *objtype)
{
    char	 *securitytypes[] = {
	         "Users", "Roles"};
    int i;
    DirNode	 *dirnode, *parent_dirnode;
    char	  *dummy = "dummy";
    gchar	  *text;
    GtkCTreeNode  *node,    *objectnode;

    if (!glist_from_query("SELECT username FROM sys.dba_users "
                          "ORDER BY username",
			  obj_cinfo.login->dbid, 0, FALSE, 0)) {
	gtk_ctree_node_set_selectable(GTK_CTREE(obj_mainwin.tree1),topnode, FALSE);
	return;
    }

    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), topnode);
    if (!folder_pixmap)
	folder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, user);

    if (!ofolder_pixmap)
	ofolder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &ofolder_mask, NULL, ofolder);

    for (i = 0; i < sizeof(securitytypes)/ sizeof(securitytypes[0]); i++) {
    
        dirnode = new_dirnode(parent_dirnode, securitytypes[i]);
	dirnode->menu_creator = create_generic_owner_menu;
	text = securitytypes[i];
	    
	node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), topnode, NULL, &text, 4, 
				 folder_pixmap, folder_mask, ofolder_pixmap, ofolder_mask, FALSE, FALSE);
	    
	gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);
	    
	objectnode = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), node, NULL, &dummy, 4, 
					   NULL, NULL, NULL, NULL, FALSE, FALSE);
    
    }
}


void 
load_security_objects(
    GtkCTreeNode  *parent_node,
    char	  *owner,
    const char	  *dbversion,
    const char	  *dbid)
{

    static char	  usertemplate[] = "SELECT username FROM sys.dba_users ORDER BY username";
    static char	  roletemplate[] = "SELECT role FROM sys.dba_roles order by role";
    DirNode	  *dirnode, *parent_dirnode;
    gchar	  *text;
    GtkCTreeNode  *node;
    char          *query;
    GList         *owners;
    GList         *o;

    query = g_malloc(strlen(usertemplate) + 1);
    
    if(!strncmp(owner,"User",4))
        sprintf(query, usertemplate);
    else
        sprintf(query, roletemplate);
    
    owners = glist_from_query(query, dbid, 0, TRUE, active_step);
    active_clear();
    g_free(query);
    
    if(owners==NULL)
        return;
    
    if (! folder_pixmap)
	folder_pixmap = 
	    gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, folder);
    
    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), parent_node);
    parent_dirnode->scanned = TRUE;

    for (o = owners; o; o = g_list_next(o)) {

	dirnode = new_dirnode(parent_dirnode, o->data);
	dirnode->menu_creator = NULL;
	text = o->data;

        node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), parent_node, NULL, &text, 4,
                                     folder_pixmap, folder_mask, NULL, NULL, FALSE, FALSE);

        gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);

        g_free(o->data);
    }
    
    g_list_free (owners);

}

void
get_security_object_info(
    const char	     *dbversion,
    const char	     *dbid,
    const char	     *owner,
    const char	     *user_name,
    DirNode	     *dir)
{
    char	*query;
    char        *role_packer[2];
    GtkCList    *userlist;
    GtkCList    *privlist;
    GtkCList    *rolelist;
    GtkCList    *quotalist;
    int         a=0;

    static char  qusertemplate[] = "SELECT username, password, "
                                   "default_tablespace, temporary_tablespace, "
				   "profile "
				   "FROM sys.dba_users "
				   "WHERE username = '%s'";

    static char  qroletemplate[] = "SELECT granted_role, admin_option "
                                   "FROM sys.dba_role_privs "
				   "WHERE grantee = '%s'";

    static char  qprivtemplate[] = "SELECT privilege, admin_option "
                                   "FROM sys.dba_sys_privs "
				   "WHERE grantee = '%s'";
				   
    static char  quotatemplate[] = "SELECT tablespace_name, max_bytes "
                                   "FROM dba_ts_quotas "
				   "WHERE username = '%s'";

    if(!strncmp(owner,"User",4))
        a = 1;
    else
        a = 0;

    gtk_widget_destroy(obj_mainwin.dynamictoolbar1);
    obj_create_blank_toolbar();
    obj_ginfo.toolbar_owner = OMDB_None;

    if(a){
        query = g_malloc(strlen(qusertemplate) + strlen(user_name) + 1);
        sprintf(query, qusertemplate, user_name);
        userlist = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    }else{
        userlist = GTK_CLIST(gtk_clist_new(2));
	role_packer[0]=(char *)user_name;
	role_packer[1]=NULL;
	gtk_clist_append(userlist, role_packer);
    }

    query = g_malloc(strlen(qprivtemplate) + strlen(user_name) + 1);
    sprintf(query, qprivtemplate, user_name);
    privlist = clist_from_query(query, dbid, 0, "<NULL>", NULL);

    query = g_malloc(strlen(qroletemplate) + strlen(user_name) + 1);
    sprintf(query, qroletemplate, user_name);
    rolelist = clist_from_query(query, dbid, 0, "<NULL>", NULL);

    if(a){
        query = g_malloc(strlen(quotatemplate) + strlen(user_name) + 1);
        sprintf(query, quotatemplate, user_name);
        quotalist = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    }else
        quotalist = NULL;

    gtk_widget_destroy(obj_mainwin.notebook1);
    gtk_widget_show(create_db_user (a, userlist, privlist, rolelist, quotalist));

}

void
show_generic_security_window(
    const char *dbversion,
    const char *dbid,
    const char *owner)
{
     GtkCList	     *list;
     char	     *query;
     char	     *notebook_tab;

    static char usertemplate8[] = 
	"SELECT username \"Name\", account_status \"Status\", "
	"       default_tablespace \"Default TS\", "
	"       temporary_tablespace \"Temp TS\" "
	"FROM   sys.dba_users";

     static char usertemplate7[] = 
	"SELECT username \"Name\", "
	"       default_tablespace \"Default TS\", "
	"       temporary_tablespace \"Temp TS\" "
	"FROM   sys.dba_users";

     static char roletemplate[] = 
	"SELECT role \"Role\", password_required \"Password Required\" "
	"FROM   sys.dba_roles";

    static char tabtemplate[] = "Security Items";

    query = g_malloc(sizeof(usertemplate8) + 1);
    
    if(!strncmp(owner, "User", 4))
        if(dbversion)
            sprintf(query, usertemplate8);
	else
	    sprintf(query, usertemplate7);
    else
        sprintf(query, roletemplate);
	
    list = clist_from_query(query, dbid, 0, "<NULL>", NULL);
    g_free(query);

    notebook_tab = g_malloc(sizeof(tabtemplate) + strlen(owner) + 1);
    sprintf(notebook_tab, tabtemplate, owner);

    gtk_widget_destroy(obj_mainwin.notebook1);
    glist_set_opt_col_width(list, 0);
    glist_set_opt_col_width(list, 1);
    if(!strncmp(owner, "User", 4)){
        glist_set_opt_col_width(list, 2);
        glist_set_opt_col_width(list, 3);
    }

    obj_do_generic_view(list, notebook_tab);
    g_free(notebook_tab);
}

