/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright � Matthew Chappee <matthew@orasoft.org>
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
/* triggerfuncs.h - definitions of functions in objectmanager/objects/triggers */

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

void loadusertriggers(GtkCTreeNode *,char *, const char *, const char *);
void get_trigger_info(const char *, const char *, const char *, const char *, DirNode *);
void show_generic_trigger_window(const char *, const char *, const char *);
void obj_dotriggerview(GtkCList *info, const char *);

/* create functions */
GtkWidget *create_trigger_window1 (void);

/* menu functions */ 
GtkWidget *create_triggermenu (gpointer data);
void on_trg_refresh_activate(GtkMenuItem *menuitem,gpointer user_data);
void on_trg_create_activate(GtkMenuItem *menuitem,gpointer user_data);
void on_trg_delete_activate(GtkMenuItem *menuitem,gpointer user_data);

