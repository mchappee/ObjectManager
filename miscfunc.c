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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "include/objectman.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <glob.h>
#include <errno.h>
#include <dlfcn.h>
#include <time.h>
#include <xpm/usericon.xpm>
#include <xpm/foldericon.xpm>
#include <xpm/ofoldericon.xpm>

#include <gdk/gdkkeysyms.h>
#include "objects/tables/tablefuncs.h"

static GList *obj_pixmaps_directories = NULL;
static GdkPixmap *folder_pixmap=NULL, *ofolder_pixmap;
static GdkBitmap *folder_mask, *ofolder_mask;

typedef struct {
  gboolean scanned;
  gchar *placekeeper;
}objecttree;


void
dummy_func(int a)
{
    
}

/* This is an internally used function to check if a pixmap file exists. */
#ifndef G_DIR_SEPARATOR_S
#define G_DIR_SEPARATOR_S "/"
#endif
gchar*
obj_check_file_exists(
    gchar           *directory,
    gchar           *filename)
{
    gchar *full_filename;
    struct stat s;
    gint status;

    full_filename = g_malloc (strlen (directory) + 1 + strlen (filename) + 1);
    strcpy (full_filename, directory);
    strcat (full_filename, G_DIR_SEPARATOR_S);
    strcat (full_filename, filename);

    status = stat (full_filename, &s);
    if (status == 0 && S_ISREG (s.st_mode))
	return full_filename;
    g_free (full_filename);
    return NULL;
}


/* This is an internally used function to create pixmaps. */
GtkWidget*
obj_create_pixmap_yada(
    GtkWidget       *widget,
    gchar           *filename)
{
    gchar *found_filename = NULL;
    GdkColormap *colormap;
    GdkPixmap *gdkpixmap;
    GdkBitmap *mask;
    GtkWidget *pixmap;
    GList *elem;

    /* We first try any pixmaps directories set by the application. */
    elem = obj_pixmaps_directories;
    while (elem)
	{
	    found_filename = obj_check_file_exists ((gchar*)elem->data, filename);
	    if (found_filename)
		break;
	    elem = elem->next;
	}

    /* If we haven't found the pixmap, try the source directory. */
    if (!found_filename)
	{
	    found_filename = obj_check_file_exists ("", filename);
	}

    if (!found_filename)
	{
	    g_print ("Couldn't find pixmap file: %s", filename);
	    return NULL;
	}

    colormap = gtk_widget_get_colormap (widget);
    gdkpixmap = gdk_pixmap_colormap_create_from_xpm (NULL, colormap, &mask,
						     NULL, found_filename);
    g_free (found_filename);
    if (gdkpixmap == NULL)
	return NULL;
    pixmap = gtk_pixmap_new (gdkpixmap, mask);
    gdk_pixmap_unref (gdkpixmap);
    gdk_bitmap_unref (mask);
    return pixmap;
}

void clear_text (GtkWidget *text)
{ 

    int i;

    i = gtk_text_get_length (GTK_TEXT(text)); 
    gtk_text_freeze (GTK_TEXT (text));

    if (i > 0) {
	gtk_text_set_point (GTK_TEXT(text), i);
	gtk_text_backward_delete (GTK_TEXT(text), i);
    }
    gtk_text_thaw (GTK_TEXT (text));
  
}

void obj_destroy(
    GtkWidget *widget,
    gpointer   data )
{
    gtk_main_quit();
}  

int obj_do_post_login()
{	 

    playlistwin_show_dirbrowser();
    refresh_dbid_combo();
    return 0;
}

/* Return the users's home directory */
char *user_home_dir()
{
    static char *result = 0;
    char *env_home;
    
    if (result) {
	return result;
    }

    env_home=getenv("HOME");
    if (env_home) {
	result = g_strdup(env_home);
    } else {
	/* HOME not set - get home dir for real uid from password
	   file. */
	struct passwd *pwd;
        
        if ((pwd = getpwuid(getuid())) && pwd->pw_passwd) {
            result = g_strdup(pwd->pw_dir);
        }
    }
    if (result == 0) {
	fatal_error("Your home directory cannot be located\n"
		    "I've checked both the \"HOME\" environment variable\n"
		    "and your password entry but I still can't figure it out.");
    }
    if (access(result, W_OK) == -1) {
	char *msg = g_malloc(strlen(result) + 256);
	sprintf(msg, "Your home directory (%s)\n"
		"isn't writeable by you - which is a bit odd if you ask me.\n%s"
		"There may be problems later because of this.",
		result, 
		env_home ? "Check whether the \"HOME\" environment variable is correctly set\n" : 0);
	show_warning_msg(msg);
	g_free(msg);
    }
    return result;
}

/* Read info about other apps */
void obj_discovery(void)
{
    char   *pattern;
    glob_t globs;
    char   *homedir;
    int    count      = 0;
    int    allocated  = 20;
    int    i;

    obj_ginfo.totaldisc = 0;
    if ((homedir=user_home_dir()) == 0)
	return;

    obj_disc = g_malloc(sizeof(*obj_disc)*allocated);

    pattern = g_malloc(strlen(homedir)+32);
    sprintf(pattern, "%s/.oralinux/*.reg", homedir);
    
    if (glob(pattern, GLOB_NOSORT, 0, &globs) != 0) {
	g_free(pattern);
	return;
    }

    for (i = 0; i < globs.gl_pathc; i++) {
	int ifd = open(globs.gl_pathv[i], O_RDONLY);
	if (ifd < 0) {
	    file_open_error(globs.gl_pathv[i], errno, "read");
	} else {
	    struct stat	   s;
	    char	   *fbuf;
	    char	   *r;
	    obj_discstruct t;

	    fstat (ifd, &s);
	    if(s.st_size==0)
		break;
	    fbuf = g_malloc(s.st_size);
	    read(ifd, fbuf, s.st_size);
	    close(ifd);
	    if(strstr(fbuf,"\n")==NULL)
		break;
	    t.dname = t.path = t.icon = 0;
	    for (r = fbuf ; (r = strtok(r, "\n")) != 0; r = 0) {
		if (strncasecmp(r, "name=", 5) == 0) {
		    t.dname = g_strdup(r+5);
		} else if (strncasecmp(r, "exepath=", 8) == 0) {
		    t.path = g_strdup(r+8);
		} else if (strncasecmp(r, "icon=", 5) == 0) {
		    t.icon = g_strdup(r+5);
		}
	    }
	    g_free(fbuf);
	    if (t.dname && t.path && t.icon) {
		if (count > allocated) {
		    allocated += 20;
		    obj_disc = g_realloc(obj_disc, sizeof(*obj_disc)*allocated);
		}
		obj_disc[count++] = t;
	    } else {
		if (t.dname) g_free(t.dname);
		if (t.path)  g_free(t.path);
		if (t.icon)  g_free(t.icon);
	    }
	}
    }
    g_free(pattern);
    globfree(&globs);

    obj_ginfo.totaldisc = count;
}

/* Gather the export libraries */
void get_export_libs(void)
{
    char   *pattern;
    glob_t globs;
    char   *homedir;
    int    count      = 0;
    int    allocated  = 20;
    int    i;
    void   *handle;
    char   *(*dl_gettooltip)(void);
    char   *(*dl_getdescription)(void);
    char   *(*dl_getauthor)(void);
    char   *(*dl_getmenuitem)(void);
    char   *(*dl_getextension)(void);
    char   *error;
    
    obj_ginfo.total_dlinfo = 0;
    
    if ((homedir=user_home_dir()) == 0)
	return;

    dl_info = g_malloc(sizeof(*dl_info)*allocated);

    pattern = g_malloc(strlen(homedir)+32);
    sprintf(pattern, "%s/.orasoft/exportlibs/*.so*", homedir);
    
    if (glob(pattern, GLOB_NOSORT, 0, &globs) != 0) {
	g_free(pattern);
	return;
    }

    for (i = 0; i < globs.gl_pathc; i++) {
        handle = dlopen (globs.gl_pathv[i], RTLD_LAZY);
	if (!handle) {
	    msgbox(dlerror());
	} else {
	    DynamicExport t;
	    t.dl_filepath = t.dl_description = t.dl_tooltip = t.dl_menuitem = t.dl_author = 0;
	    
	    dl_gettooltip = dlsym(handle, "get_tooltip");
	    if ((error = dlerror()) != NULL)  {
	        msgbox("Error getting tooltip from dynamic library");
		break;
	    }
	    
	    dl_getdescription    = dlsym(handle, "get_description");
	    if ((error = dlerror()) != NULL)  {
	        msgbox("Error getting description from dynamic library");
		break;
	    }
	    
	    dl_getmenuitem    = dlsym(handle, "get_menuitem");
	    if ((error = dlerror()) != NULL)  {
	        msgbox("Error getting menu item from dynamic library");
		break;
	    }
	    
	    dl_getauthor    = dlsym(handle, "get_author");
	    if ((error = dlerror()) != NULL)  {
	        msgbox("Error getting author information from dynamic library");
		break;
	    }
	    
	    dl_getextension    = dlsym(handle, "get_extension");
	    if ((error = dlerror()) != NULL)  {
	        msgbox("Error getting file extension from dynamic library");
		break;
	    }
	    
	    t.dl_filepath     = g_strdup(globs.gl_pathv[i]);
	    t.dl_tooltip      = g_strdup(dl_gettooltip());
	    t.dl_description  = g_strdup(dl_getdescription());
            t.dl_menuitem     = g_strdup(dl_getmenuitem());
	    t.dl_author       = g_strdup(dl_getauthor());
	    t.dl_extension    = g_strdup(dl_getextension());
	    t.dl_id           = count;
	    
	    dlclose(handle);
	    
	    if (t.dl_tooltip && t.dl_description && t.dl_author && t.dl_menuitem && t.dl_extension) {
		if (count > allocated) {
		    allocated += 20;
		    dl_info = g_realloc(dl_info, sizeof(*dl_info)*allocated);
		}
		dl_info[count++] = t;
		
	    } else {
		if (t.dl_filepath) g_free(t.dl_filepath);
		if (t.dl_tooltip)  g_free(t.dl_tooltip);
		if (t.dl_description)  g_free(t.dl_description);
		if (t.dl_author) g_free(t.dl_author);
		if (t.dl_menuitem) g_free(t.dl_menuitem);
		if (t.dl_extension) g_free(t.dl_extension);
		count--;
	    }
	    
        }
    }
    g_free(pattern);
    globfree(&globs);

    obj_ginfo.total_dlinfo = count;

}

void obj_initglobals()
{
    char *logging;       

    obj_ginfo.totaldisc=0;
    obj_ginfo.crow = -1;
    obj_ginfo.objtype = 1;
    obj_ginfo.permission = 0;
    obj_ginfo.toolbar_owner=OMDB_None;
    obj_ginfo.tree_selecting = 0;    

    logging = prefs_get_char_var(PREF_LOGGING);
    if(logging)
        obj_ginfo.logsql = g_strdup(logging);
    else
        obj_ginfo.logsql = NULL;

    obj_ginfo.rows = prefs_get_int_var(PREF_ROWS);

    obj_ginfo.printcall = g_strdup(prefs_get_char_var(PREF_PRINTCALL));
        
    if(obj_ginfo.dynamicexport)
        get_export_libs();
	
    prefs_save_values();
    
}


static void
free_glist_string_item(
    gpointer list_data,
    gpointer user_data)
{
    g_free(list_data);
}


void generic_populate_tablespace(
    GtkWidget	     *combo,
    GtkWidget	     *combo_entry,
    char             *dbid)
{
    GList *combo1_items;

    combo1_items = glist_from_query("SELECT tablespace_name FROM user_tablespaces order by tablespace_name",
				    dbid, 0, TRUE, NULL);

    if (combo1_items)
	gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo1_items);
    else
	gtk_list_clear_items (GTK_LIST (GTK_COMBO (combo)->list), 0, -1);
	
    gtk_entry_set_text(GTK_ENTRY(combo_entry),obj_cinfo.login->default_tablespace);

    g_list_foreach (combo1_items, free_glist_string_item, 0);
    g_list_free (combo1_items);

}


void
generic_populate_schemas(
    GtkWidget	     *combo,
    GtkWidget	     *combo_entry,
    char             *dbid)
{
    GList *combo1_items;

    if(obj_cinfo.login->dbversion)   /* Oracle 8 */
        combo1_items = glist_from_query("SELECT username FROM all_users order by username",
				    dbid, 0, TRUE, NULL);
    else
        combo1_items = glist_from_query("SELECT name FROM all_users order by name",
				    dbid, 0, TRUE, NULL);

    if (combo1_items)
	gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo1_items);
    else
	gtk_list_clear_items (GTK_LIST (GTK_COMBO (combo)->list), 0, -1);
	
    gtk_entry_set_text(GTK_ENTRY(combo_entry),obj_cinfo.login->owner);

    g_list_foreach (combo1_items, free_glist_string_item, 0);
    g_list_free (combo1_items);
}

void
generic_populate_tables(
    GtkWidget	     *combo,
    char             *owner,
    char             *dbid)
{
    GList *combo1_items;
    char  *query;
    char  *qtemplate="SELECT table_name FROM all_tables where "
                         "owner = '%s' order by table_name";
    
    query = g_malloc(strlen(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    combo1_items = glist_from_query(query, dbid, 0, TRUE, NULL);

    if (combo1_items)
	gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo1_items);
    else
	gtk_list_clear_items (GTK_LIST (GTK_COMBO (combo)->list), 0, -1);
	
    g_list_foreach (combo1_items, free_glist_string_item, 0);
    g_free(query);
    g_list_free (combo1_items);
}

void
drop_object(
    const char    *dbid, 
    const char    *owner, 
    const char    *object,
    const char    *typetext,
    int            useowner)
{
    char    *qtemplateowner = "DROP %s %s.%s";
    char    *qtemplate =      "DROP %s %s";
    char    *query;
    
    if(useowner){
        query = g_malloc(strlen(qtemplate) + strlen(owner) + strlen(object) + strlen(typetext) + 1);
        sprintf(query, qtemplateowner, typetext, owner, object);
        if(execute_immediate_sql(dbid, query))
            msgbox("Object Dropped Successfully");
    }else{
        query = g_malloc(strlen(qtemplate) + strlen(object) + strlen(typetext) + 1);
        sprintf(query, qtemplate, typetext, object);
        if(execute_immediate_sql(dbid, query))
            msgbox("Object Dropped Successfully");
    }
    g_free(query);

}

void 
remove_selected_tree_node(GtkWidget *ctree)
{
    GtkCTreeNode *tnode;
    GList *list_node;

    list_node = GTK_CLIST(ctree)->selection;
    tnode = list_node->data;

    gtk_ctree_remove_node (GTK_CTREE(ctree),tnode);

}

char *
gettablespacename(
    char *dbid,
    char *owner,
    int  flag)
{
    char          *cdbid;
    char	  *query;
    char          *tsname = 0;
    char          *buf = 0;
    GList         *owners;

    cdbid=dbid;

    if(!flag)
       query = "SELECT default_tablespace FROM user_users";
    else
       query = "SELECT temporary_tablespace FROM user_users";

    owners = glist_from_query(query, dbid, 0, TRUE, (void *)dummy_func);       
	
    if(owners != NULL)
        tsname = g_strdup(owners->data);
	
    g_list_foreach (owners, free_glist_string_item, 0);
    g_list_free (owners);
    
    buf = tsname;
    buf = buf ? g_strdup(buf) : 0;
    
    return buf;
}


void generic_populate_profile(
    GtkWidget	     *combo,
    GtkWidget	     *combo_entry,
    char             *dbid)
{
    GList *combo1_items;

    combo1_items = glist_from_query("SELECT UNIQUE profile FROM sys.dba_profiles order by profile",
				    dbid, 0, TRUE, NULL);

    if (combo1_items)
	gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo1_items);
    else
	gtk_list_clear_items (GTK_LIST (GTK_COMBO (combo)->list), 0, -1);

    g_list_foreach (combo1_items, free_glist_string_item, 0);
    g_list_free (combo1_items);

}

void
refresh_dbid_combo()
{
    GtkCTreeNode  *tnode;
    int           i;
    DirNode       *dirnode;
    GList         *glist=NULL;
    
    
    for(i=0; i < GTK_CTREE(obj_mainwin.tree1)->clist.rows; i++){
        tnode = gtk_ctree_node_nth(GTK_CTREE(obj_mainwin.tree1),i);
	dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), tnode);
	if(!strstr(dirnode->path, "->"))
            glist = g_list_append(glist, dirnode->path);
    }
    
    gtk_combo_set_popdown_strings( GTK_COMBO(obj_mainwin.toolbarcombo1), glist);

}

void
get_special_owners(
    GtkCTreeNode      *rootnode, 
    GtkCTreeNode      *topnode, 
    char	      *objtype)
{
    GtkCTreeNode      *node;
    GtkCTreeNode      *objectnode;
    char	      *dummy   = "dummy";
    DirNode	      *dirnode, *parent_dirnode;
    gchar	      *text;
    GList             *owners;
    GList             *o;
    char              *prepsql="warn THIS";
    char              *rollbacktemplate = "SELECT distinct tablespace_name "
                                          "FROM dba_rollback_segs";
					
    char              *datafiletemplate = "SELECT tablespace_name "
                                          "FROM dba_tablespaces "
		  	                  "ORDER BY tablespace_name";
			       
    char              *sessiontemplate  = "SELECT DISTINCT username "
                                          "FROM v$session "
			                  "WHERE username IS NOT NULL "
			                  "ORDER BY username";

    if(!strncmp(objtype,"SESSIONS",8)){
        prepsql = sessiontemplate;
        folder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, user);
	ofolder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &ofolder_mask, NULL, user);
    }
    if(!strncmp(objtype,"DATAFILES",9)){
        prepsql = datafiletemplate;
	folder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, folder);
	ofolder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &ofolder_mask, NULL, ofolder);
    }
	
    if(!strncmp(objtype,"ROLLBACKS",9)){
        prepsql = rollbacktemplate;
	folder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &folder_mask, NULL, folder);
	ofolder_pixmap = gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &ofolder_mask, NULL, ofolder);
    }
		
    owners = glist_from_query(prepsql, obj_cinfo.login->dbid, 0, FALSE, active_step);
    active_clear();
    
    if(owners==NULL){
	gtk_ctree_node_set_selectable(GTK_CTREE(obj_mainwin.tree1),topnode, FALSE);
        return;
    }
    
    parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(obj_mainwin.tree1), topnode);
    

    for (o = owners; o; o = g_list_next(o)) {

	dirnode = new_dirnode(parent_dirnode, o->data);
	dirnode->menu_creator = create_generic_owner_menu;
	text = o->data;

        node = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), topnode, NULL, &text, 4,
                                     folder_pixmap, folder_mask, ofolder_pixmap, ofolder_mask, FALSE, FALSE);

        gtk_ctree_node_set_row_data_full(GTK_CTREE(obj_mainwin.tree1), node, dirnode, destroy_cb);

        objectnode = gtk_ctree_insert_node(GTK_CTREE(obj_mainwin.tree1), node, NULL, &dummy, 4,
                                           NULL, NULL, NULL, NULL, FALSE, FALSE);
        g_free(o->data);
    }

    
    return;
    
}

static void
show_sql                     (GtkButton       *button,
                              gpointer         user_data)
{

    if(!strncmp(gtk_object_get_data(GTK_OBJECT(user_data), "shown"), "0", 1))
    {
        gtk_widget_show(GTK_WIDGET(user_data));
        gtk_object_set_data (GTK_OBJECT (user_data), "shown", "1");
    }else{
        gtk_widget_hide(GTK_WIDGET(user_data));
        gtk_object_set_data (GTK_OBJECT (user_data), "shown", "0");
    }
}

GtkWidget *
add_sql_window(
    GtkWidget *container)
{
    GtkWidget    *scrolledwindow;
    GtkWidget    *sqlbox;
    GtkWidget    *vbox1;
    GtkWidget    *showsql_button;

    vbox1 = gtk_vbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (container), vbox1);
    gtk_widget_show(vbox1);

    showsql_button = gtk_button_new_with_label ("Show SQL");
    gtk_widget_show (showsql_button);
    gtk_container_add (GTK_CONTAINER (vbox1), showsql_button);
    gtk_widget_set_usize (showsql_button, 1, 20);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (scrolledwindow), "shown", "1");
    gtk_widget_ref (scrolledwindow);
    gtk_widget_show (scrolledwindow);
    gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    if(obj_ginfo.highlighting){
        sqlbox = gtk_editor_new(NULL, NULL);
	install_pat(GTK_EDITOR(sqlbox), obj_mainwin.window1);
    }else
        sqlbox = gtk_text_new(NULL, NULL);
	
    gtk_widget_ref (sqlbox);
    gtk_text_set_editable (GTK_TEXT (sqlbox), TRUE);
    gtk_text_set_word_wrap(GTK_TEXT (sqlbox),TRUE);
    gtk_widget_show (sqlbox);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), sqlbox);

    gtk_signal_connect (GTK_OBJECT (showsql_button), "clicked",
                        GTK_SIGNAL_FUNC (show_sql),
                        (gpointer)scrolledwindow);
    
    return sqlbox;

}

void
logsql(
    char    *sql)
{
    FILE        *stream;
    time_t      now;
    struct tm   *tm;
    
    if(obj_ginfo.logsql){
        stream = fopen(obj_ginfo.logsql, "a");
	if(stream){
	    now = time (NULL);
	    tm = localtime (&now);
	    fprintf(stream,"\n\nExecuted at: %02d:%02d:%02d %02d-%02d-%d", tm->tm_hour, tm->tm_min, 
	                                                                 tm->tm_sec, tm->tm_mon + 1, 
									 tm->tm_mday, tm->tm_year+1900);
	    fprintf(stream,"\n%s",sql);
	    fclose(stream);
	}
    }


}

/* From autofs */
static int
mkdir_path(
    const char	 *path, 
    mode_t	 mode)
{
    char	 *buf	  = g_malloc(strlen(path)+1);
    const char	 *cp	  = path;
    const char	 *lcp	  = path;
    char	 *bp	  = buf;

    do {
	if (cp != path && (*cp == G_DIR_SEPARATOR || *cp == '\0')) {
	    memcpy(bp, lcp, cp-lcp);
	    bp += cp-lcp;
	    lcp = cp;
	    *bp = '\0';
	    if (mkdir(buf, mode) == -1) {
		/* If it already exists, make sure it's a directory */
		if (errno == EEXIST) {
		    struct stat st;
		    
		    if (stat(buf, &st) == 0 &&
			!S_ISDIR(st.st_mode))
			errno = ENOTDIR;
		    else
			continue;
		}
                g_free(buf);
		return -1;
	    }
	}
    } while(*cp++ != '\0');
    g_free(buf);
    return 0;
}


void
validate_tnsnames()
{
    struct stat s;
    int         status;
    char        *tns_admin;
    char        *oracle_home;
    char        *tnsnames;
    char        *message;
    char        *putenv_c;

    tns_admin =   getenv("TNS_ADMIN");
    oracle_home = getenv("ORACLE_HOME");

    if(tns_admin){
        tnsnames = g_malloc(strlen(tns_admin) + 14);
	sprintf(tnsnames,"%s/tnsnames.ora",tns_admin);
	status = stat (tnsnames, &s);
	
        if (status == 0 && S_ISREG (s.st_mode)){
	    g_free(tnsnames);
	    return;
        }else{
	    if(confirm("You do not appear to have a valid tnsnames.ora file. " 
	               "Would you like to create one now?")){
                tns_win(tnsnames);
	        msgbox("OK, the tnsnames.ora file has been created under $TNS_ADMIN/tnsnames.ora");
		g_free(tnsnames);
		return;	
            }else{
	        msgbox("OK, but I doubt that this is going to work out well for you");
		g_free(tnsnames);
		return;
	    }
        }
    }
    
    if(oracle_home){
        tnsnames = g_malloc(strlen(oracle_home) + 30);
	sprintf(tnsnames,"%s/network/admin/tnsnames.ora",oracle_home);
	status = stat (tnsnames, &s);
        if (status == 0 && S_ISREG (s.st_mode)){
	    if(confirm("You have a tnsnames.ora file but the environment variable "
	            "TNS_ADMIN is not set.  This could cause problems with the "
		    "Oracle libraries.  Can I set the variable TNS_ADMIN for "
		    "this session?")){
		sprintf(tnsnames,"%s/network/admin",oracle_home);
                putenv_c = g_malloc(strlen(tnsnames) + 20);
                sprintf(putenv_c, "TNS_ADMIN=%s", tnsnames);
	        putenv(putenv_c);
                g_free(putenv_c);
		message = g_malloc(strlen(tnsnames) + 50);
		sprintf(message,"Remember to add \nTNS_ADMIN=%s\n to your profile.",tnsnames);
		msgbox(message);
		g_free(tnsnames);
		g_free(message);
            }else{
	        msgbox("OK, but I doubt that this is going to work out well for you");
		g_free(tnsnames);
		return;
	    }
	}else{
	
	}
    }else{	 
	 if(confirm("You have neither $ORACLE_HOME nor $TNS_ADMIN set in the "
	               "environment.  This is not good.  I can try to help by "
		       "adding these environment variables and creating a tnsnames.ora "
		       "file.  Shall I give it a try?")){
	        if(mkdir_path("/usr/local/oracle/8/network/admin", 755) != 0){
                    msgbox("Could not create tnsnames.ora, permission denied");
                    return;
                }
		putenv("TNS_ADMIN=/usr/local/oracle/8/network/admin");
		putenv("ORACLE_HOME=/usr/local/oracle/8");
		tns_win("/usr/local/oracle/8/network/admin/tnsnames.ora");
		msgbox("OK, I created a tnsnames.ora file in /usr/local/oracle/8/network/admin "
		       "I have also set the environment variables\n "
		       "TNS_ADMIN=/usr/local/oracle/8/network/admin\n"
		       "ORACLE_HOME=/usr/local/oracle/8\n\n Do yourself a favor and put "
		       "these settings in a profile somewhere");
		
         }else{
	        msgbox("OK, but I doubt that this is going to work out well for you");
		return;
	 }
    }	       
    

}


GtkWidget*
lookup_widget                          (GtkWidget       *widget,
                                        const gchar     *widget_name)
{
  GtkWidget *parent, *found_widget;

  for (;;)
    {
      if (GTK_IS_MENU (widget))
        parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
      else
        parent = widget->parent;
      if (parent == NULL)
        break;
      widget = parent;
    }

  found_widget = (GtkWidget*) gtk_object_get_data (GTK_OBJECT (widget),
                                                   widget_name);
  if (!found_widget)
    g_warning ("Widget not found: %s", widget_name);
  return found_widget;
}

