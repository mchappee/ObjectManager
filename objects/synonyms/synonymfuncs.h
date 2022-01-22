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
/* synonymfuncs.h - definitions of functions in objectmanager/objects/synonymes */

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

extern void obj_dosynonymview (GtkWidget *);
extern void build_syn_sql (void);  
extern void on_sname_changed (GtkEditable *editable, gpointer user_data);  
extern void on_sschema_changed (GtkEditable *editable, gpointer user_data);  
extern void on_otype_changed (GtkEditable *editable, gpointer user_data);  
extern void on_tschema_changed (GtkEditable *editable, gpointer user_data);  
extern void on_tobject_changed (GtkEditable *editable, gpointer user_data);  
extern void on_cdb_toggled (GtkToggleButton *togglebutton, gpointer user_data);  
extern void on_rdb_toggled (GtkToggleButton *togglebutton, gpointer user_data);  
extern void on_syncancel_clicked (GtkButton *button, gpointer user_data);  
extern void on_synok_clicked (GtkButton *button, gpointer user_data);  
extern void on_syn_checkbutton1_clicked (GtkToggleButton *togglebutton, gpointer user_data);  
extern void syn_populate_schemas (void);  
extern void syn_loadobjtype (int a);  
extern GtkWidget *obj_create_synwin (void);  
extern void loadusersynonyms (GtkCTreeNode *, char *, const char *, const char *);
extern void get_synonym_info (const char *, const char *, const char *, const char *, DirNode *);
extern void obj_on_synonymclist1_select_row (GtkCList *clist, gint row, gint column,
					     GdkEvent *event, gpointer user_data);  
extern void show_generic_synonym_window(const char *, const char *, const char *);
extern GtkWidget* create_synonymmenu (gpointer data);
extern void on_syn_refresh_activate(GtkMenuItem     *menuitem,gpointer         user_data);
extern void on_syn_create_activate(GtkMenuItem     *menuitem,gpointer         user_data);
extern void on_syn_delete_activate(GtkMenuItem     *menuitem,gpointer         user_data);
extern GtkWidget* create_all_synonymmenu (gpointer data);
extern void on_syn_all_refresh_activate(GtkMenuItem     *menuitem,gpointer         user_data);

