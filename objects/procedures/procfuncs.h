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

void loadusercode(GtkCTreeNode *parent_node, const char *dbid, 
		  const char *owner, const char *codetype, 
		  MenuCreationFunc menu_creator, ExpandCB expand_callback,
		  GdkPixmap *pixmap, GdkBitmap *mask,
		  GdkPixmap *exp_pixmap, GdkBitmap *exp_mask);
void loaduserprocs(GtkCTreeNode *, char *, const char *, const char *);
char *getprocsource(const char *dbid, const char *owner,
		    const char *procname, const char *otype);
void get_proc_info(const char *, const char *, const char *, const char *, DirNode *);
void obj_doprocview(void);

GtkWidget *create_execute_statement(GtkCList *, char *);

/* Proc Toolbar */
void 
obj_create_proc_toolbar(gpointer data, int options);

void
on_procsearch_clicked                     (GtkButton       *button,
                                        gpointer         user_data);
void
on_procbutton1_pressed                     (GtkButton       *button,
                                        gpointer         user_data);
void
on_procbutton2_pressed                     (GtkButton       *button,
                                        gpointer         user_data);
void
on_procbutton3_pressed                     (GtkButton       *button,
                                        gpointer         user_data);
void
on_procbutton4_pressed                     (GtkButton       *button,
                                        gpointer         user_data);
void
on_procbutton5_pressed                     (GtkButton       *button,
                                        gpointer         user_data);
void
on_procbutton6_pressed                     (GtkButton       *button,
                                        gpointer         user_data);
void
on_procbutton_execute_pressed(GtkButton	 *button,gpointer	 user_data);

void getprocstatus(const char *owner, const char *object, const char *qdbid, int pbflag);

/* These are from codecreate.c */
GtkWidget *create_procedure(void);
GtkWidget *create_function(void);
GtkWidget *create_package(void);
GtkWidget *create_package_body(void);
void on_codebutton1_clicked (GtkButton *button,gpointer user_data);
void on_codebutton2_clicked (GtkButton *button,gpointer user_data);
GtkWidget *create_codewindow (char *, OMDBObjectType);

int get_error_byte(int line,int position, char *owner);
void show_generic_procedure_window(const char *dbversion,const char *dbid,const char *owner);
GString *get_package_body(const char *owner,const char *package,const char *dbid);
void obj_on_trigger_clicked(GtkButton *button,gpointer user_data);
void get_trigger_status(const char *owner, const char *object, const char *qdbid, DirNode *data);

extern void on_proc_refresh_activate(GtkMenuItem *menuitem,gpointer user_data);
extern void on_proc_all_refresh_activate(GtkMenuItem *menuitem,gpointer user_data);
extern void on_proc_create_activate(GtkMenuItem *menuitem,gpointer user_data);
extern void on_proc_delete_activate(GtkMenuItem *menuitem,gpointer user_data);
extern GtkWidget *create_procmenu (gpointer data);
extern void on_proc_grant_privilege_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data);
    
