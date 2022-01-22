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
/* securityfuncs.h - definitions of functions in objectmanager/objects/security */

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

extern void load_security_objects (GtkCTreeNode *, char *, const char *, const char *);
extern void get_security_object_info (const char *, const char *, const char *, const char *, DirNode *);
extern void show_generic_security_window(const char *, const char *, const char *);
extern GtkWidget* create_securitymenu (gpointer data);
void get_security_owners(GtkCTreeNode  *rootnode,GtkCTreeNode  *topnode,char  *objtype);

GtkWidget *
create_db_user (int, GtkCList *, GtkCList *, GtkCList *, GtkCList *);

GtkWidget*
create_security_object (void);

void
on_create_user_password_changed        (GtkEditable     *editable,
                                        gpointer         user_data);
void
on_create_user_ttablespace_changed     (GtkEditable     *editable,
                                        gpointer         user_data);
void
on_create_user_username_changed        (GtkEditable     *editable,
                                        gpointer         user_data);
void
on_create_user_quotaval_changed        (GtkEditable     *editable,
                                        gpointer         user_data);
void
on_create_user_dtablespace_changed     (GtkEditable     *editable,
                                        gpointer         user_data);
void
on_create_user_quota_unlimited_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void
on_create_user_quoteM_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void
on_create_user_quotaK_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void
on_create_user_profile_changed         (GtkEditable     *editable,
                                        gpointer         user_data);
void
on_create_user_adminflag_clicked       (GtkButton       *button,
                                        gpointer         user_data);
void
on_create_user_added_click_column      (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);
void
on_create_user_priv_click_column       (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);
void
on_create_user_roles_click_column      (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);
void
on_quota_click_column                  (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);
void
create_user_execute_clicked            (GtkButton       *button,
                                        gpointer         user_data);
void
on_create_user_cancel_clicked          (GtkButton       *button,
                                        gpointer         user_data);
void
on_create_user_os_password_toggled     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void
on_create_user_removeperm_clicked       (GtkButton       *button,
                                        gpointer         user_data);

/* Quota Stuff */
void
on_quota_none_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void
on_quota_unlimited_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void
on_quota_limited_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void
on_quota_add_clicked                   (GtkButton       *button,
                                        gpointer         user_data);
GtkWidget*
create_quota_selection ();
int
get_quota();

