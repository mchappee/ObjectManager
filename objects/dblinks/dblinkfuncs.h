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
/* dblinkfuncs.h - definitions of functions in objectmanager/objects/dblinks */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

/*
 * Standard gettext macros.
 */
#ifndef OBJ_NLS_SETUP
#define OBJ_NLS_SETUP
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif
#endif

extern void loaduserdblinks (GtkCTreeNode *, char *, const char *, const char *);
extern void get_dblink_info (const char *, const char *, const char *, const char *, DirNode *);  
extern void obj_on_dblink_clicked (GtkButton *button, gpointer user_data);  
extern GtkWidget* obj_create_dblwin (void);
extern void obj_dodblinkview(GtkWidget *);
extern void obj_create_dblink_toolbar(gpointer data);
extern void show_generic_dblink_window(const char *, const char *, const char *);

extern GtkWidget* create_dblinkmenu (gpointer data);
extern void on_dbl_refresh_activate(GtkMenuItem     *menuitem,gpointer         user_data);
extern void on_dbl_create_activate(GtkMenuItem     *menuitem,gpointer         user_data);
extern void on_dbl_delete_activate(GtkMenuItem     *menuitem,gpointer         user_data);
extern void on_dbl_rename_activate(GtkMenuItem     *menuitem,gpointer         user_data);
extern void on_dbl_test_activate(GtkMenuItem     *menuitem,gpointer         user_data);
extern GtkWidget* create_all_dblinkmenu (gpointer data);
extern void on_dbl_all_refresh_activate(GtkMenuItem     *menuitem,gpointer         user_data);

void get_dblink_owners(GtkCTreeNode *rootnode, GtkCTreeNode *topnode, char *objtype);
