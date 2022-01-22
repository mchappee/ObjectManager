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
/* procfuncs.h - definitions of functions in objectmanager/objects/procedures */

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

void loaduserpackages(GtkCTreeNode *,char *, const char *, const char *);
void get_package_info(const char *, const char *, const char *, const char *, DirNode *);
void obj_dopackageview(char *elements);
void loadpackageprocs(GtkCTreeNode *parent_node, const char *dbid, const char *owner,
		      const char *package);
void on_package_expand_cb(DirNode *dir, GtkWidget *widget, GtkCTreeNode *parent_node);
void obj_dopackageprocview(GtkCList *args);
void on_package_notebook_switch_page(GtkNotebook *notebook,GtkNotebookPage *page,
     gint page_num,gpointer user_data);
void show_generic_package_window(const char *dbversion,const char *dbid,const char *owner);


