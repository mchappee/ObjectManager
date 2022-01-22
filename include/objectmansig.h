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
#ifndef __OBJECTMANSIG_H__
#define __OBJECTMANSIG_H__

/* Menu Declarations */

void
obj_on_Quit_activate            (GtkMenuItem     *menuitem,
                                 gpointer         user_data);

void
obj_on_Save_Connection_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
obj_on_Forget_Connection_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
obj_on_About_Object_Manager_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
obj_on_About_OraLinux_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_About_modules_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data);

void
obj_on_Discovery_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
									
GtkWidget* obj_create_login (void);

typedef struct _logininfo{
    char	       *dbid;
    char	       *dbversion;
    char	       *login;
    int		       allobjects;
    char	       *owner;
    char               *default_tablespace;
    char               *temp_tablespace;
    struct _logininfo  *next;
}obj_logininfo;


typedef struct currentinfo_str{
    obj_logininfo    *login;
    char	     currentobject[500];
    OMDBObjectType   currenttype;
    char	     currentobjectowner[500];
    char	     currentobject_noowner[500];
    GtkCTreeNode     *current_node;
}obj_currentinfo;

extern obj_logininfo *obj_logins;
extern obj_currentinfo obj_cinfo;

typedef struct dynamicinfo_str{
    char            *dl_filepath;
    char            *dl_description;
    char            *dl_tooltip;
    char            *dl_author;
    char            *dl_menuitem;
    char            *dl_extension;
    int             dl_id;
}DynamicExport;

DynamicExport   *dl_info;

typedef struct globalinfo_str{
    char	    *globallogin;
    gint	    crow;
    int		    globalprint;
    int		    totaldisc;
    char	    regfile[255];
    char	    *iconpath;
    int		    stopflag;
    int		    killflag;
    int		    objtype;
    int		    operation;
    int		    cached;
    char	    newtable[255];
    OMDBObjectType  toolbar_owner;
    int		    datacrow;
    int		    permission;
    char            *procfilename;
    int             dynamicexport;
    int             total_dlinfo;
    int             highlighting;
    char	    *logsql;
    int 	    rows;
    char	    *printcall;
    int             runprocmode;
    int             tree_selecting;
}obj_globalinfo;

extern obj_globalinfo obj_ginfo;
					
#endif
