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
#ifndef __OBJECTMANSRC_H__
#define __OBJECTMANSRC_H__

/* SQL Error handling snippet which looks like
   a function call to keep the preprocessor happy */

#define sqlerr() { obj_sql_error(); goto stopover; }

/* Toolbar Menu Stuff */
void obj_on_text1_activate (GtkMenuItem     *menuitem,gpointer         user_data);
void obj_on_icons1_activate (GtkMenuItem     *menuitem,gpointer         user_data);
void obj_on_text_and_icons1_activate (GtkMenuItem     *menuitem,gpointer         user_data);
void obj_create_main_toolbar(GtkWidget *);

void obj_create_menu(GtkWidget *);

typedef struct disc_str{
  char *dname;
  char *path;
  char *icon;
}obj_discstruct;

obj_discstruct *obj_disc;

struct _DirNode;

typedef enum {
    OMDB_None = 0,
    OMDB_Table,
    OMDB_Sequence,
    OMDB_Synonym,
    OMDB_Index,
    OMDB_View,
    OMDB_Procedure,
    OMDB_Function,
    OMDB_Type,
    OMDB_Package,
    OMDB_Trigger,
    OMDB_DBLink,
    OMDB_Datafile,
    OMDB_Session,
    OMDB_Tablespace,
    OMDB_Security,
    OMDB_Rollback,
    OMDB_Logfile,
    OMDB_PackageBody} OMDBObjectType;

typedef enum {
    PROCTOOLBAR_ENABLE = 0,
    PROCTOOLBAR_OPEN,
    PROCTOOLBAR_SAVE, 
    PROCTOOLBAR_PRINT,
    PROCTOOLBAR_MESSAGE,
    PROCTOOLBAR_POSITION,
    PROCTOOLBAR_SUBMIT,
    PROCTOOLBAR_EXECUTE,
    PROCTOOLBAR_SEARCH,
    TABLETOOLBAR_ANALYZE,
    TABLETOOLBAR_COUNT,
    TABLETOOLBAR_CACHE,
    TABLETOOLBAR_ADD_COLUMN,
    TABLETOOLBAR_RENAME,
    TABLETOOLBAR_TRUNCATE,
    TABLETOOLBAR_INDEX,
    TABLETOOLBAR_DEALLOCATE,
    INDEXTOOLBAR_REBUILD,
    MAINTOOLBAR_QUIT,
    MAINTOOLBAR_LOGIN,
    LOGINBUTTON_QUIT,
    LOGINBUTTON_LOGIN,
    LOGINBUTTON_PIXMAP,
    LOGFILETOOLBAR_DROP,
    SESSIONTOOLBAR_KILL,
    ADVANTIO_PIXMAP,
    ROLLBACKTOOLBAR_TOGGLE,
    ROLLBACKTOOLBAR_SHRINK,
    TABLEWITCH_PIXMAP,
    TUXLOGO_PIXMAP,
    TABLESPACETOOLBAR_COALESCE,
    PRINTER_PIXMAP} Pixmap_Icons_Enum;

typedef void (*ObjectListOwnersFunc)(GtkCTreeNode *,GtkCTreeNode *, char *);
typedef void (*LoadUserObjectsFunc)(GtkCTreeNode *,char *, const char *, const char *);
typedef void (*ObjectInfoFunc)(const char *, const char *, const char *, const char *,struct _DirNode *);
typedef void (*GeneralInfoFunc)(const char *, const char *, const char *);
typedef void (*ExpandCB)(struct _DirNode *, GtkWidget *widget, GtkCTreeNode *parent_node);
typedef GtkWidget* (*CreateDialogFunc) (void);

typedef GtkWidget *(*MenuCreationFunc)(gpointer);

typedef struct {
    char		  *labeltext;
    MenuCreationFunc	  menu_creator;
    ObjectListOwnersFunc  list_owners;
    char		  *list_owners_type;
    LoadUserObjectsFunc	  load_user_objects;
    ObjectInfoFunc        object_info_func;
    GeneralInfoFunc       general_info_func;
    CreateDialogFunc      create_dialog_func;
    int                   omdb_type;
} DBObject;

typedef struct _DirNode
{
    gboolean		 scanned;
    gchar		 *path;
    MenuCreationFunc	 menu_creator;
    ExpandCB	         expand_callback;
    DBObject      	 *db_object;
    /* The following are set by the menu callbacks, don't
     * assume any useful value if not in a menu callback */
    GtkMenu		 *menu;
    GtkCTree		 *tree;
    GtkCTreeNode	 *node;
    gint		 row;
}
DirNode;

void obj_doorasoft(void);
GtkWidget *msgbox (char *);
GtkWidget *msgbox_x (char *msg, int w, int h, int modal, GtkCList *clist);

void obj_do_generic_view(GtkCList *clist,const char *notebook_tab);

/* Dirnode functions */
DirNode *new_dirnode(DirNode *parent, const char *name);
int parse_path(const char *path, char **elements, int max_elements);
void free_path_elements(char **elements, int num_elements);

/* Path element indices */
#define DIRPATH_DBID   0
#define DIRPATH_TYPE   1
#define DIRPATH_OWNER  2
#define DIRPATH_OBJECT 3

#endif
