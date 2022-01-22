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
/* viewfuncs.h - definitions of functions in objectmanager/objects/viewes */

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

extern void view_populate_schemas (void);  
extern GtkWidget *obj_create_viewwin (void);  
extern void loaduserviews (GtkCTreeNode *, char *, const char *, const char *);
extern void get_view_info (const char *, const char *, const char *, const char *, DirNode *);
extern void obj_on_compile_clicked (GtkButton *button, gpointer user_data);  
extern void obj_create_view_toolbar (void);  
extern void on_view_notebook_switch_page (GtkNotebook *notebook, GtkNotebookPage *page, gint page_num, gpointer user_data);  
extern void obj_doviewview (GtkCList *column_list);  
extern void show_generic_view_window(const char *, const char *, const char *);

GtkWidget* create_viewmenu (gpointer data);
GtkWidget* create_all_viewmenu (gpointer data);

/* menu callbacks */
extern void on_view_refresh_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_create_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_delete_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_rename_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_clear_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_export_csv_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_export_html_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_export_sql_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_script_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_view_all_refresh_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void
on_view_grant_privilege_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data);
    
