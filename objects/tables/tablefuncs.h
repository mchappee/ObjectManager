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
/* tablefuncs.h - definitions of funstions in objectmanager/objects/tables */

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

extern GtkWidget* get_twwinmain_widget (void);
extern GtkWidget* obj_create_twwin (void);
extern GtkWidget* obj_create_twstoragewin (void);

void 
obj_create_table_toolbar(gpointer data, const char *);

extern void obj_dotableview(GtkCList *column_list, GtkWidget *, GtkWidget *, GtkWidget *);
extern void obj_on_clist10_select_row (GtkCList *clist, gint row, gint column, 
				       GdkEvent *event, gpointer user_data);  
extern void obj_on_entry10_changed (GtkEditable *editable, gpointer user_data);  
extern void obj_on_addcolumnwin_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_canceladd_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_entry11_changed (GtkEditable *editable, gpointer user_data);  
extern void loadusertables (GtkCTreeNode *, char *, const char *, const char *);
extern void get_table_info (const char *, const char *, const char *, const char *, DirNode *);
extern void obj_on_cancelrename_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_okrename_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_createtable_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_analyze_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_sample_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_cache_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_addcolumn_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_rename_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_truncate_clicked (GtkButton *button, gpointer user_data);  
extern void obj_on_addindex_clicked (GtkButton *button, gpointer user_data); 
extern void on_table_unallocate_clicked(GtkButton *button,gpointer user_data); 
extern void on_table_notebook_switch_page (GtkNotebook *notebook, GtkNotebookPage *page, 
					   gint page_num, gpointer user_data);  
extern void on_tab_refresh_activate (GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_create_activate (GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_delete_activate (GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_rename_activate (GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_clear_activate  (GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_grant_privilege_activate  (GtkMenuItem *menuitem, gpointer user_data);
extern GtkWidget* create_tablemenu (gpointer data);
extern GtkWidget* obj_create_renamewin (gpointer data);
extern GtkWidget* obj_create_twwinmain (void);
extern GtkWidget* obj_create_addcolumn (gpointer data);
extern void tw_populate_schemas(void);
extern void tw_populate_tablespace(void);
extern void show_generic_table_window(const char *, const char *, const char *);

GtkWidget* create_export_fileselection (DirNode *dir,
					void (*exporter)(const char *, GtkCList *), void *);

/* menu callbacks */
extern void on_tab_refresh_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_create_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_delete_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_rename_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_clear_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_export_csv_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_export_html_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_export_sql_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_dl_export_activate(GtkMenuItem *menuitem,gpointer user_data);
extern void on_tab_script_activate(GtkMenuItem *menuitem, gpointer user_data);
extern void on_tab_all_refresh_activate(GtkMenuItem *menuitem, gpointer user_data);
extern GtkWidget* create_all_tablemenu (gpointer data);

void obj_on_rowcount_clicked(GtkButton *button,gpointer user_data);

extern void on_fs_ok_button_clicked(GtkButton *button, gpointer user_data);
extern void on_fs_cancel_button_clicked(GtkButton *button, gpointer user_data);

extern void do_query(const char *sql_query,const char *databaseid,int  max_rows,
		     int (*rowcallback)(int, int, char **, short *, short **, char **, int *, short *, gpointer),
		     gpointer user_data);

extern int execute_sql(const char *databaseid, const char *sql_stmt, ...);

/* Analyzewin */
void
on_analyze_compute_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_analyze_estimate_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_analyze_delete_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_analyze_validate_clicked            (GtkButton       *button,
                                        gpointer         user_data);

GtkWidget*
create_analyzewin (int *response);

int 
analyzewin(void);

