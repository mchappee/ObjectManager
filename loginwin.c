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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "include/objectman.h"
#include "objects/tables/tablefuncs.h"

GtkWidget *login;
GtkWidget *username_entry;
GtkWidget *password_entry;
GtkWidget *tns_entry;
GtkWidget *checkbutton1;

char *
getusername(
    const char *l)
{
    char *tmp;
    char *sep;

    /* copy input */
    tmp=g_strdup(l);

    /* accept "user/passwd@tns", "user@tns" or "user" return user */
    if((sep = strstr(tmp,"/")) ||
       (sep = strstr(tmp,"@"))) {
	*sep = '\0';
    }
    return tmp;
}

char *
getpassword(
    const char *l)
{
    char *passwd;
    char *tmp;
    char *sep1;
    char *sep2;

    /* copy input */
    tmp=g_strdup(l);

    /* Accept "user/pass@tns", return password */
    if((sep1 = strstr(tmp,"/"))) {
        sep2 = strstr(sep1+1,"@");
        if(sep2)
            *sep2 = '\0';
	passwd = g_strdup(sep1+1);
	g_free(tmp);
	return passwd;
	
    }
    g_free(tmp);
    return 0;
}

char *
getdb(
    const char *l)
{
    char *passwd;
    char *tmp;
    char *sep;

    /* copy input */
    tmp=g_strdup(l);

    /* Accept "user/pass@tns", "user@tns", return tns */
    if((sep = strstr(tmp, "@"))) {
	passwd = g_strdup(sep+1);
	g_free(tmp);
	return passwd;
    }
    g_free(tmp);
    return 0;
}

char *
getdbid(
    const char *l)
{
    char *user = getusername(l);
    char *db = getdb(l);
    char *dbid;

    if (db) {
	dbid = g_malloc(strlen(user) + strlen(db) + 2);
	sprintf(dbid, "%s@%s", user, db);
    } else {
	dbid = g_malloc(strlen(user) + 7);
	sprintf(dbid, "%s@local", user);
    }
    g_free(user);
    g_free(db);
    return dbid;
}

gboolean
obj_on_login_key_press_event(
    GtkWidget       *widget,
    GdkEventKey     *event,
    gpointer         user_data)
{

    if(event->keyval==65293){
	obj_on_login_ok_clicked(GTK_BUTTON(gtk_button_new()), NULL);
	/* we handled this event */
	return TRUE;
    }
    return FALSE;
}

void obj_on_login_cancel_clicked (GtkButton *button,gpointer user_data)
{
    if(obj_logins==0)
	gtk_main_quit();
    else
	gtk_widget_destroy(login);
}

static int
get_db_version(
    const char	 *databaseid,
    char	 **version,
    char	 **compat)
{
    int		 ret;
    *version = g_malloc(32);
    *compat = g_malloc(32);
    
    memset(*version, ' ', 31);
    memset(*compat, ' ', 31);
    (*version)[31] = '\0';
    (*compat)[31] = '\0';

    ret = execute_sql(databaseid,
			     "begin dbms_utility.db_version(:version, :compat); end;",
			     *version, *compat);
    if (ret != 0) {
	g_free(*version);
	g_free(*compat);
	*version = *compat = 0;
    }
    
    return ret;
}

void obj_on_login_ok_clicked (
    GtkButton	     *button,
    gpointer	     user_data)
{
    char	     *cstring;
    char	     *user;
    char	     *pass;
    char	     *tns;
    char	     *dbid;
    obj_logininfo    *new_login	= g_malloc(sizeof(*new_login));
    char	     *dbversion;
    char	     *dbcompat;
    GtkWidget        *get_active;

    if(!button){
        cstring=(char *)g_strdup(obj_ginfo.globallogin);
    }else{
        cstring=(char *)g_strdup(gtk_entry_get_text(GTK_ENTRY(username_entry)));
        if(strstr(cstring,"/")==NULL){
            user=cstring;
            pass=gtk_entry_get_text(GTK_ENTRY(password_entry));
            tns=gtk_entry_get_text(GTK_ENTRY(tns_entry));

            if(!strlen(tns)){
                tns = NULL;
                cstring=g_malloc(strlen(user) + strlen(pass) + 2);
                sprintf(cstring,"%s/%s",user,pass);
            }else{
                cstring=g_malloc(strlen(user) + strlen(pass) + strlen(tns) + 3);
                sprintf(cstring,"%s/%s@%s",user,pass,tns);
            }

	    g_free(user);
        }
	g_free(obj_ginfo.globallogin);
        obj_ginfo.globallogin = g_strdup(cstring);
    }

    new_login->owner = getusername(cstring);
    new_login->dbid = getdbid(cstring);
    new_login->default_tablespace = 0;
    new_login->temp_tablespace = 0;

    if(new_login->dbid==NULL){
	msgbox("Invalid Login Data");
	if (new_login->owner)
	    g_free(new_login->owner);
	return;
    }

    if (GTK_TOGGLE_BUTTON (checkbutton1)->active==0){
	new_login->allobjects = 0;
    } else {
	new_login->allobjects = 1;
    }

    dbid = new_login->dbid;

    if(!oracle_login(cstring,dbid)){
        msgbox("Bad username, password, or tnsname");
        g_free(new_login->owner);
        g_free(new_login->dbid);
        g_free(new_login->default_tablespace);
        g_free(new_login->temp_tablespace);
        g_free(new_login);
	return;
    }

    g_free(cstring);

    if (!get_db_version(dbid, &dbversion, &dbcompat)) {
	new_login->dbversion = g_malloc(strlen(dbversion) + strlen(dbcompat) + 32);
	sprintf(new_login->dbversion, "oracle[%s/%s]", dbversion, dbcompat);
	g_free(dbversion);
	g_free(dbcompat);
    } else {
	new_login->dbversion = 0;
    }

    execute_immediate_sql(dbid,"BEGIN DBMS_APPLICATION_INFO.SET_CLIENT_INFO('Advantio ObjectManager'); END;");

    new_login->default_tablespace = gettablespacename(new_login->dbid,new_login->owner,0);
    new_login->temp_tablespace = gettablespacename(new_login->dbid,new_login->owner,1);

    new_login->next = obj_logins;
    obj_logins = new_login;
    obj_cinfo.login = new_login;

    strcpy(obj_cinfo.currentobject,"none");

    if(!obj_ginfo.runprocmode){
        gtk_widget_show (obj_mainwin.window1);
    
        /* We can't do this until the main window has a window */
        get_active=active_init(obj_mainwin.window1);
        if(get_active)
          gtk_box_pack_start (GTK_BOX (obj_mainwin.hbox2), get_active, FALSE, FALSE, 0);
        gtk_widget_destroy (login);

        obj_do_post_login();
    }

    return;
    
}

GtkWidget*
obj_create_login ()
{
    GtkWidget *fixed1;
    GtkWidget *pixmap1;
    GtkWidget *toolbar1;
    GtkWidget *tmp_toolbar_icon;
    GtkWidget *toolbar2;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *label3;
    GtkWidget *login_cancel;
    GtkWidget *login_ok;
  
    login = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (login), "login", login);
    gtk_signal_connect (GTK_OBJECT (login), "key_press_event",
			GTK_SIGNAL_FUNC (obj_on_login_key_press_event),
			NULL);
    gtk_window_set_title (GTK_WINDOW (login), "Oracle Login");
    gtk_window_set_policy (GTK_WINDOW (login), TRUE, TRUE, FALSE);
    gtk_window_set_position (GTK_WINDOW (login), GTK_WIN_POS_CENTER);

    fixed1 = gtk_fixed_new ();
    gtk_object_set_data (GTK_OBJECT (login), "fixed1", fixed1);
    gtk_widget_set_usize (fixed1, 398, 280);
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (login), fixed1);

    username_entry = gtk_entry_new ();
    gtk_object_set_data (GTK_OBJECT (login), "username_entry", username_entry);
    gtk_widget_show (username_entry);
    gtk_fixed_put (GTK_FIXED (fixed1), username_entry, 56, 88);
    gtk_widget_set_uposition (username_entry, 56, 88);
    gtk_widget_set_usize (username_entry, 280, 24);
    gtk_widget_grab_focus (username_entry);
    GTK_WIDGET_SET_FLAGS (username_entry, GTK_CAN_DEFAULT);
    gtk_widget_grab_default (username_entry);

    password_entry = gtk_entry_new ();
    gtk_object_set_data (GTK_OBJECT (login), "password_entry", password_entry);
    gtk_widget_show (password_entry);
    gtk_fixed_put (GTK_FIXED (fixed1), password_entry, 56, 128);
    gtk_widget_set_uposition (password_entry, 56, 128);
    gtk_widget_set_usize (password_entry, 280, 24);
    GTK_WIDGET_SET_FLAGS (password_entry, GTK_CAN_DEFAULT);
    gtk_entry_set_visibility (GTK_ENTRY (password_entry), FALSE);

    tns_entry = gtk_entry_new ();
    gtk_object_set_data (GTK_OBJECT (login), "tns_entry", tns_entry);
    gtk_widget_show (tns_entry);
    gtk_fixed_put (GTK_FIXED (fixed1), tns_entry, 56, 168);
    gtk_widget_set_uposition (tns_entry, 56, 168);
    gtk_widget_set_usize (tns_entry, 280, 24);
    GTK_WIDGET_SET_FLAGS (tns_entry, GTK_CAN_DEFAULT);

    if(strncmp(obj_ginfo.globallogin,"none",4)!=0){  
	char *user = getusername(obj_ginfo.globallogin);
	char *db = getdb(obj_ginfo.globallogin);
	char *pass = getpassword(obj_ginfo.globallogin);
      
	gtk_entry_set_text(GTK_ENTRY(username_entry),user);
	gtk_entry_set_text(GTK_ENTRY(password_entry),pass);
	gtk_entry_set_text(GTK_ENTRY(tns_entry),db);
	g_free(user);
	g_free(pass);
	g_free(db);
    }

    pixmap1 = create_login_pixmap (LOGINBUTTON_PIXMAP);
    if (pixmap1 == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    gtk_object_set_data (GTK_OBJECT (login), "pixmap1", pixmap1);
    gtk_widget_show (pixmap1);
    gtk_fixed_put (GTK_FIXED (fixed1), pixmap1, 192, 32);
    gtk_widget_set_uposition (pixmap1, 192, 32);
    gtk_widget_set_usize (pixmap1, 21, 21);

    checkbutton1 = gtk_check_button_new_with_label ("Show All Objects");
    gtk_widget_ref (checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (login), "checkbutton1", checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed1), checkbutton1, 56, 190);
    gtk_widget_set_uposition (checkbutton1, 56, 190);
    gtk_widget_set_usize (checkbutton1, 170, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton1), TRUE);

    toolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_object_set_data (GTK_OBJECT (login), "toolbar1", toolbar1);
    gtk_widget_show (toolbar1);
    gtk_fixed_put (GTK_FIXED (fixed1), toolbar1, 56, 235);
    gtk_widget_set_uposition (toolbar1, 56, 235);
    gtk_widget_set_usize (toolbar1, 32, 16);

    tmp_toolbar_icon =  create_login_pixmap (LOGINBUTTON_QUIT);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    login_cancel = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
							    GTK_TOOLBAR_CHILD_BUTTON,
							    NULL,
							    "Cancel",
							    NULL, NULL,
							    tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (login), "login_cancel", login_cancel);
    gtk_widget_show (login_cancel);
    gtk_signal_connect (GTK_OBJECT (login_cancel), "clicked",
			GTK_SIGNAL_FUNC (obj_on_login_cancel_clicked),
			NULL);

    toolbar2 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_object_set_data (GTK_OBJECT (login), "toolbar2", toolbar2);
    gtk_widget_show (toolbar2);
    gtk_fixed_put (GTK_FIXED (fixed1), toolbar2, 288, 235);
    gtk_widget_set_uposition (toolbar2, 288, 235);
    gtk_widget_set_usize (toolbar2, 32, 16);

    tmp_toolbar_icon =  create_login_pixmap (LOGINBUTTON_LOGIN);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    login_ok = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar2),
							GTK_TOOLBAR_CHILD_BUTTON,
							NULL,
							" Login ",
							NULL, NULL,
							tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (login), "login_ok", login_ok);
    gtk_widget_show (login_ok);
    gtk_signal_connect (GTK_OBJECT (login_ok), "clicked",
			GTK_SIGNAL_FUNC (obj_on_login_ok_clicked),
			NULL);

    label1 = gtk_label_new ("Username");
    gtk_object_set_data (GTK_OBJECT (login), "label1", label1);
    gtk_widget_show (label1);
    gtk_fixed_put (GTK_FIXED (fixed1), label1, 56, 72);
    gtk_widget_set_uposition (label1, 56, 72);
    gtk_widget_set_usize (label1, 280, 16);

    label2 = gtk_label_new ("Password");
    gtk_object_set_data (GTK_OBJECT (login), "label2", label2);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed1), label2, 56, 112);
    gtk_widget_set_uposition (label2, 56, 112);
    gtk_widget_set_usize (label2, 280, 16);

    label3 = gtk_label_new ("Tnsname");
    gtk_object_set_data (GTK_OBJECT (login), "label3", label3);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed1), label3, 56, 152);
    gtk_widget_set_uposition (label3, 56, 152);
    gtk_widget_set_usize (label3, 280, 16);

    return login;
}

