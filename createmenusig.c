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

void
obj_on_Quit_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    obj_destroy(NULL,user_data);

}

void
obj_on_Save_Connection_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    
    if(strncmp(obj_ginfo.globallogin,"none",4)!=0)
	prefs_set_char_var("login",obj_ginfo.globallogin);
}


void
obj_on_Forget_Connection_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    
    if(strncmp(obj_ginfo.globallogin,"none",4)!=0)
	prefs_unset_char_var("login");
}


void
obj_on_text1_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.toolbar1), GTK_TOOLBAR_TEXT);
    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_TEXT);
    prefs_set_char_var("tbstate","1");
}


void
obj_on_icons1_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.toolbar1), GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_ICONS);
    prefs_set_char_var("tbstate","2");
}


void
obj_on_text_and_icons1_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.toolbar1), GTK_TOOLBAR_BOTH);
    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_BOTH);
    prefs_set_char_var("tbstate","3");
}


void
obj_on_Discovery_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    pid_t pid;
    char *buf[3];
    struct stat sb;

    if (stat(obj_disc[(int)user_data].path, &sb) == -1){
	fprintf(stderr,"\nFile %s is not an executable file",obj_disc[(int)user_data].path);
	return;
    }

    pid = fork();

    if (pid == 0) {

	buf[0]=obj_disc[(int)user_data].path;
	buf[1]=obj_ginfo.globallogin;
	buf[2]=NULL;

	execv(obj_disc[(int)user_data].path, buf);
	perror("exec error");
	exit(-1);
    }
    if (pid < 0) {
	perror("fork error");
	exit(-1);
    }
}

void
on_preferences_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    gtk_widget_show(create_preferences_window());
}


void
obj_on_About_Object_Manager_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    char * abouttext = "\nAbout Object Manager:\n\nVersion 2.0 "
	"\n\nObject Manager is an Oracle object browser and\n" 
	"manager for Linux.\n\n"
	"It will, of course, work on all major Unix platforms.\n\n"
        "Object Manager is being actively developed by\n"
	"the Orasoft development team.";

    msgbox_x(abouttext,478,300,0,NULL);
}


void
obj_on_About_OraLinux_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    char *abouttext = "About OraSoft:\n\n"
	"OraSoft is an ongoing project to bring enterprise\n"
	"quality database software to the Linux platform.\n"
	"If you want to help with development,\n"
	"documentation or artwork\n"
	"please send me mail at matthew@orasoft.org.";

    msgbox_x(abouttext,478,300,0,NULL);
}

void
on_About_modules_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{

    int    i;
    GtkCList    *clist;
    char        *clist_vals[3];
    
    clist = GTK_CLIST(gtk_clist_new(3));
    
    clist_vals[0] = "Author";
    clist_vals[1] = "Description";
    clist_vals[2] = "File";
    gtk_clist_append(clist, clist_vals);
    
    clist_vals[0] = " ";
    clist_vals[1] = " ";
    clist_vals[2] = " ";
    gtk_clist_append(clist, clist_vals);
    
    for (i = 0; i < obj_ginfo.total_dlinfo; i++) {
	clist_vals[0] = dl_info[i].dl_author;
	clist_vals[1] = dl_info[i].dl_description;
	clist_vals[2] = dl_info[i].dl_filepath;
	
	gtk_clist_append(clist, clist_vals);
    }
    
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 200);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 400);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 200);
    
    msgbox_x(NULL, 800, 300, 0, clist);

}

