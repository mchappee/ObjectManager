/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright ? Matthew Chappee <matthew@orasoft.org>
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

#include <oci.h>
#include "gtkeditor/gtkeditor.h"

#ifndef __MISCFUNC_H__
#define __MISCFUNC_H__

#define glist_set_opt_col_width(clist, col)     \
    gtk_clist_set_column_width (GTK_CLIST(clist), (col), \
				gtk_clist_optimal_column_width (GTK_CLIST(clist), (col)))

void
obj_add_pixmap_directory                   (gchar           *directory);

gchar*
obj_check_file_exists                      (gchar           *directory,
                                        gchar           *filename);

void
refresh_dbid_combo(void);

GtkWidget*
obj_create_pixmap                          (GtkWidget       *widget,
                                        gchar           *filename);
/* PTF 01 Jan 2000 */
char *user_home_dir(void);

/* void 
obj_on_tree1_tree_expand               (GtkCTree        *ctree,
                                        GtkCTreeNode    *node);*/
					
void obj_destroy( GtkWidget *widget,gpointer   data );
int obj_do_post_login(void);
void obj_discovery(void);
void obj_initglobals(void);
void do_regfile(char *regfile);
void obj_sql_error(void);
void clear_text (GtkWidget *);
void obj_create_toolbar(void);

/* from objectmansig.c */
GtkWidget *create_runproc_win(char *);

/* from preferences.c */
GtkWidget*
create_preferences_window (void);

void
validate_tnsnames(void);

/* from main menu */
void
on_preferences_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data);

GtkWidget* obj_create_aboutbox (void);
void obj_progcancel_clicked(GtkButton *button, gpointer user_data);

void generic_populate_schemas(GtkWidget *combo,GtkWidget *combo_entry, char *dbid);
void generic_populate_tablespace(GtkWidget *combo,GtkWidget *combo_entry, char *dbid);
void generic_populate_tables(GtkWidget *combo, char *owner, char *dbid);
void generic_populate_profile(GtkWidget *combo,GtkWidget *combo_entry, char *dbid);
    

/* storage.c */
extern GtkWidget *obj_create_storagewin (GtkWidget *window, GtkWidget *packer, int packtype);  

/* indexer.c */
extern GtkWidget *obj_create_indexwin (void);  
extern void populate_schemas (void);  
extern void populate_tablespace (void);  
extern void populate_index_columns (void);  
extern void populate_index_tables (const char *);  

/* Forward declaration */
struct DirNode;

/* printwin.c */
char * printwin(char *msg, ...);

/* treecontrol.c */
void on_ctree1_tree_select_row (GtkCTree *ctree, GList *node, 
				gint column, gpointer user_data);
GtkWidget *obj_create_dir_browser(gchar * title, gchar * current_path, 
				  GtkSelectionMode mode, void (*handler) (DirNode *));
void set_dbid(const char *rnode);
gboolean on_dir_button_press(GtkWidget *widget, GdkEventButton *event,
			     gpointer user_data);

/* loginwinsig.c */
char *getdbid(const char *l);
char *getusername(const char *l);
char *getpassword(const char *l);
char *getdb(const char *l);

/* msgbox.c */
void msgbox_adios(void);

/* objectfunc.c */
void obj_show_object(char *buf2,char *buf3,char *buf4);
int  obj_getctype(char *buf2);
void get_object_list(DBObject **objects, int *nobjects);
void loaduserobjects(char *buf2,char *buf3,GtkCTreeNode *parent_node);
void obj_create_node(GtkCTreeNode  *root_node, DBObject	  *obj);
void obj_create_nodes(GtkCTreeNode *root_node);
int getpubcount(char *buf2);
void create_new_object(int);
void get_special_owners(
    GtkCTreeNode      *rootnode, 
    GtkCTreeNode      *topnode, 
    char	      *objtype);
    
/* object menu creation */
GtkWidget* create_otab_menu (gpointer data);
GtkWidget* create_otablespace_menu (gpointer data);
GtkWidget* create_osynonym_menu (gpointer data);
GtkWidget* create_oindex_menu (gpointer data);
GtkWidget* create_oview_menu (gpointer data);
GtkWidget* create_generic_owner_menu (gpointer data);
GtkWidget* create_oprocedure_menu (gpointer data);
GtkWidget  *create_root_menu (gpointer data);


/* object menu callbacks */
void on_objtype_refresh_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_gowner_refresh_activate(GtkMenuItem *menuitem,gpointer user_data);
void on_otab_create_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_otablespace_create_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_otab_script_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_oindex_create_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_oview_create_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_oview_script_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_oproc_exportall_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_root_refresh_activate(GtkMenuItem *menuitem,gpointer user_data);
void on_root_disconnect_activate(GtkMenuItem *menuitem,gpointer user_data);

/* Activity indicator routines */
extern int active_step(int counter);
extern void active_clear(void);
extern GtkWidget *active_init(GtkWidget *);

/* This is the SQL window displayed while creating objects */
GtkWidget *add_sql_window(GtkWidget *container);

/* This is in oracle_oci.c */
char *source_from_query(const char *sql_query,const char *databaseid, const char *owner, const char *procname);
int execute_immediate_sql(const char *databaseid,const char *sql_stmt);
char *getrowcount(char    *databaseid,char    *sql_query);
int
execute_nonblocking_sql(
    const char	 *databaseid,
    const char	 *sql_stmt,
    GtkWidget    *timelabel);
    
/* New wizard framework (this shouldn't actually be used at the moment
   - it's not finished */
extern GtkWidget* create_witch (int npages, GtkWidget **pages);
extern void drop_object(const char *, const char *, const char *, const char *, int);

extern void remove_selected_tree_node(GtkWidget *);
extern void get_export_libs(void);

void 
obj_create_blank_toolbar(void);

void 
obj_getdatafiles_ts(GtkCTreeNode *rootnode, GtkCTreeNode *topnode, char *objtype);

void dummy_func(int a);
void install_pat (GtkEditor *, GtkWidget *);

/* From pixmap_handler.c */
GtkWidget*
create_proc_pixmap(
    int    pixmap_id);
    
GtkWidget*
create_table_pixmap(
    int    pixmap_id);

GtkWidget*
create_main_pixmap(
    int    pixmap_id);

GtkWidget*
create_login_pixmap(
    int    pixmap_id);
    
GtkWidget*
create_logfile_pixmap(
    int    pixmap_id);
    
GtkWidget*
create_session_pixmap(
    int    pixmap_id);
    
GtkWidget*
create_misc_pixmap(
    int    pixmap_id);
    
GtkWidget*
create_rollback_pixmap(
    int    pixmap_id);
    
GtkWidget*
create_tablespace_pixmap(
    int    pixmap_id);

GtkWidget*
lookup_widget                          (GtkWidget       *widget,
                                        const gchar     *widget_name);

/* From tns_win.c */
int tns_win(char *);

/* OCI Stuff */
extern int oracle_initialise(void);
extern int oracle_finalise(void);
extern int oracle_login(const char *cstring, const char *dbid);
extern int oracle_do_query(const char *sql_query,const char *databaseid,
			   int  max_rows, 
			   int (*rowcallback)(int, int, char **, short *, char **, gushort *, short *, int *, gpointer),
			   int (*rowcallback_calback)(int),
                           gpointer user_data);
int
glist_from_query_callback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *indicators, 
    char	     **values, 
    ub2		     *values_l, 
    short	     *types, 
    int              (*rowcallback)(int),
    gpointer	     user_data);
int
clist_from_query_callback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *indicators, 
    char	     **values, 
    ub2		     *values_l, 
    short	     *types, 
    int              (*rowcallback)(int),
    gpointer	     user_data);
GtkCList *
clist_from_query(
    const char *sql_query,
    const char *databaseid,
    int  max_rows,
    char *nullsubst,
    int (*rowcallback)(int));
void
set_clist_from_query(
    GtkCList   *clist,
    const char *sql_query,
    const char *databaseid,
    int  max_rows,
    int (*rowcallback)(int));
GList *
glist_from_query(
    const char    *sql_query,
    const char    *databaseid,
    int           max_rows,
    gboolean      showerrors,
    int          (*rowcallback)(int));
GtkCList *
clist_from_query_by_row(
    const char *sql_query,
    const char *databaseid,
    int  ncols,
    char **titles,
    int (*rowcallback)(int));
int
source_from_query_callback(
    int		     row, 
    int		     ncols, 
    char	     **colnames, 
    short	     *indicators, 
    char	     **values, 
    ub2		     *values_l, 
    short	     *types, 
    int              (*rowcallback)(int),
    gpointer	     user_data);
char *
insert_code_owner(
    char    *code,
    char    *owner,
    char    *procname);
    
void
logsql(
    char    *sql);
    
#endif
