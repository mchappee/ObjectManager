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
/* indexfuncs.h - definitions of functions in objectmanager/objects/indexes */

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

void obj_doindexview (GtkWidget *);
void 
obj_create_index_toolbar(gpointer data);
extern void loaduserindexes (GtkCTreeNode *, char *, const char *, const char *);
extern void get_index_info (const char *, const char *, const char *, const char *, DirNode *);
extern void obj_on_ndxanalyze_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_ndxrename_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_ndxrebuild_clicked(GtkButton *button, gpointer user_data);
extern void obj_on_index_unusable_clicked(GtkButton *button,gpointer user_data);
extern void on_index_unallocate_clicked(GtkButton *button,gpointer user_data);
extern void obj_on_button6_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_button7_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_button8_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_indexclist1_select_row (GtkCList *clist, gint row, gint column, 
					   GdkEvent *event, gpointer user_data);  
extern void show_generic_index_window(const char *, const char *, const char *);
extern GtkWidget* create_indexmenu (gpointer data);
    
extern void on_ndx_refresh_activate(GtkMenuItem *menuitem,gpointer   user_data);
extern void on_ndx_create_activate(GtkMenuItem *menuitem,gpointer   user_data);
extern void on_ndx_delete_activate(GtkMenuItem *menuitem,gpointer   user_data);
extern void on_ndx_rename_activate(GtkMenuItem *menuitem,gpointer   user_data);
extern void on_ndx_script_activate(GtkMenuItem *menuitem,gpointer   user_data);
extern GtkWidget* create_all_indexmenu (gpointer data);
extern void on_ndx_all_refresh_activate(GtkMenuItem *menuitem,gpointer   user_data);

