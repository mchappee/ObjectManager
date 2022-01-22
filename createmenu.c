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
#include "objects/dblinks/dblinkfuncs.h"
#include "objects/indexes/indexfuncs.h"
#include "objects/synonyms/synonymfuncs.h"
#include "objects/tables/tablefuncs.h"
#include "objects/views/viewfuncs.h"
#include "objects/datafiles/datafilefuncs.h"
#include "objects/sessions/sessionfuncs.h"
#include "objects/procedures/procfuncs.h"
#include "objects/functions/functionfuncs.h"
#include "objects/types/typefuncs.h"
#include "objects/packages/packagefuncs.h"
#include "objects/triggers/triggerfuncs.h"
#include "objects/sequences/sequencefuncs.h"

static void
create_object_activate(
    GtkMenuItem	      *menuitem,
    gpointer	      user_data)
{
    CreateDialogFunc  create_dialog_func  = (CreateDialogFunc) user_data;
    GtkWidget	      *create_dialog	  = create_dialog_func();
    
    gtk_widget_show(create_dialog);
}

void 
obj_create_blank_toolbar()
{

  obj_mainwin.dynamictoolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
  gtk_widget_ref (obj_mainwin.dynamictoolbar1);
  gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.dynamictoolbar1", obj_mainwin.dynamictoolbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_toolbar_set_button_relief(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_RELIEF_NONE);
  gtk_widget_show (obj_mainwin.dynamictoolbar1);
  gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.toolbar1), obj_mainwin.dynamictoolbar1, NULL, NULL);
}

void 
obj_create_main_toolbar(GtkWidget *vbox1)
{

    GtkWidget *tmp_toolbar_icon;
    GtkWidget *button1;
    GtkWidget *button4;

    obj_mainwin.toolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_widget_ref (obj_mainwin.toolbar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.toolbar1", obj_mainwin.toolbar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(obj_mainwin.toolbar1), GTK_RELIEF_NONE);
    gtk_widget_show (obj_mainwin.toolbar1);
    gtk_box_pack_start (GTK_BOX (vbox1), obj_mainwin.toolbar1, FALSE, FALSE, 0);

    tmp_toolbar_icon = create_main_pixmap (MAINTOOLBAR_QUIT);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button1 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.toolbar1),
							     GTK_TOOLBAR_CHILD_BUTTON,
							     NULL,
							     ("\nQuit"),
							     "Quit Object Manager", NULL,
							     tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);

    gtk_widget_show (button1);

    tmp_toolbar_icon = create_main_pixmap (MAINTOOLBAR_LOGIN);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button4 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.toolbar1),
							     GTK_TOOLBAR_CHILD_BUTTON,
							     NULL,
							     ("Add\nConnection"),
							     "Add an additional database connection", NULL,
							     tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button4);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button4", button4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button4);

    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.toolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.toolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.toolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.toolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.toolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.toolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.toolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.toolbar1));

    obj_create_blank_toolbar();

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (obj_on_button1_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button4), "clicked",
			GTK_SIGNAL_FUNC (obj_on_addconnect_clicked),
			NULL);
}


void obj_create_menu(GtkWidget *menubar1)
{
    GtkWidget	     *File;
    GtkWidget	     *File_menu;
    GtkWidget	     *Quit;
    GtkWidget	     *Options;
    GtkWidget	     *Options_menu;
    GtkWidget	     *Save_Connection;
    GtkWidget	     *Forget_Connection;
    GtkWidget	     *separator2;
    GtkWidget	     *preferences;
    GtkWidget	     *About;
    GtkWidget	     *About_menu;
    GtkWidget	     *About_Object_Manager;
    GtkWidget	     *About_OraLinux;
    GtkWidget	     *text1;
    GtkWidget	     *icons1;
    GtkWidget	     *text_and_icons1;
    GtkWidget	     *Toolbar;
    GtkWidget	     *toolbar1_menu;
    GtkWidget	     *CreateObject;
    GtkWidget	     *CreateObject_menu;
    GSList	     *__group  = NULL;
    GtkAccelGroup    *accel_group;
    int		     i;
    int              nitems;
    int              tbstate;
    DBObject	     *objects;


    File = gtk_menu_item_new_with_label ("File");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "File", File);
    gtk_widget_show (File);
    gtk_container_add (GTK_CONTAINER (menubar1), File);

    File_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "File_menu", File_menu);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (File), File_menu);

    Quit = gtk_menu_item_new_with_label ("Quit   ");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Quit", Quit);
    gtk_widget_show (Quit);
    gtk_container_add (GTK_CONTAINER (File_menu), Quit);
    gtk_signal_connect (GTK_OBJECT (Quit), "activate",
			GTK_SIGNAL_FUNC (obj_on_Quit_activate),
			NULL);

    Options = gtk_menu_item_new_with_label ("Options");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Options", Options);
    gtk_widget_show (Options);
    gtk_container_add (GTK_CONTAINER (menubar1), Options);

    Options_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Options_menu", Options_menu);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (Options), Options_menu);

    Save_Connection = gtk_menu_item_new_with_label ("Save Connection");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Save_Connection", Save_Connection);
    gtk_widget_show (Save_Connection);
    gtk_container_add (GTK_CONTAINER (Options_menu), Save_Connection);
    gtk_signal_connect (GTK_OBJECT (Save_Connection), "activate",
			GTK_SIGNAL_FUNC (obj_on_Save_Connection_activate),
			NULL);
    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (obj_mainwin.window1), accel_group);
    gtk_widget_add_accelerator (Save_Connection, "activate", accel_group,
				GDK_s, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

    Forget_Connection = gtk_menu_item_new_with_label ("Forget Saved Connection");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Forget_Connection", Forget_Connection);
    gtk_widget_show (Forget_Connection);
    gtk_container_add (GTK_CONTAINER (Options_menu), Forget_Connection);
    gtk_signal_connect (GTK_OBJECT (Forget_Connection), "activate",
			GTK_SIGNAL_FUNC (obj_on_Forget_Connection_activate),
			NULL);
    gtk_widget_add_accelerator (Forget_Connection, "activate", accel_group,
				GDK_f, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);


    Toolbar = gtk_menu_item_new_with_label ("Toolbar");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "Toolbar", Toolbar);
    gtk_widget_show (Toolbar);
    gtk_container_add (GTK_CONTAINER (Options_menu), Toolbar);

    toolbar1_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "toolbar1_menu", toolbar1_menu);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (Toolbar), toolbar1_menu);

    text1 = gtk_radio_menu_item_new_with_label (__group, "Text");
    __group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (text1));
    gtk_widget_ref (text1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "text1", text1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (text1);
    gtk_container_add (GTK_CONTAINER (toolbar1_menu), text1);

    icons1 = gtk_radio_menu_item_new_with_label (__group, "Icons");
    __group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (icons1));
    gtk_widget_ref (icons1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "icons1", icons1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (icons1);
    gtk_container_add (GTK_CONTAINER (toolbar1_menu), icons1);

    text_and_icons1 = gtk_radio_menu_item_new_with_label (__group, "Text and Icons");
    __group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (text_and_icons1));
    gtk_widget_ref (text_and_icons1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "text_and_icons1", text_and_icons1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (text_and_icons1);
    gtk_container_add (GTK_CONTAINER (toolbar1_menu), text_and_icons1);

    CreateObject = gtk_menu_item_new_with_label ("Create Object");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "CreateObject", CreateObject);
    gtk_widget_show (CreateObject);
    gtk_container_add (GTK_CONTAINER (menubar1), CreateObject);

    CreateObject_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "CreateObject_menu", CreateObject_menu);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (CreateObject), CreateObject_menu);

    get_object_list(&objects, &nitems);
    for (i = 0; i < nitems; i++) {
	GtkWidget *menuitem = gtk_menu_item_new_with_label (objects[i].labeltext);
	gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), objects[i].labeltext, menuitem);
	if (!objects[i].create_dialog_func)
	    gtk_widget_set_sensitive(menuitem, FALSE);
	gtk_widget_show (menuitem);
	gtk_container_add (GTK_CONTAINER (CreateObject_menu), menuitem);
	gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			    GTK_SIGNAL_FUNC (create_object_activate),
			    (gpointer) objects[i].create_dialog_func);
    }
  
    
    gtk_signal_connect (GTK_OBJECT (text1), "activate",
			GTK_SIGNAL_FUNC (obj_on_text1_activate),
			NULL);
    gtk_signal_connect (GTK_OBJECT (icons1), "activate",
			GTK_SIGNAL_FUNC (obj_on_icons1_activate),
			NULL);
    gtk_signal_connect (GTK_OBJECT (text_and_icons1), "activate",
			GTK_SIGNAL_FUNC (obj_on_text_and_icons1_activate),
			NULL);

    tbstate=prefs_get_int_var(PREF_TBSTATE);
	if(tbstate == 1){
	    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.toolbar1), GTK_TOOLBAR_TEXT);
	    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_TEXT);
	    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (text1), TRUE);
	}
	if(tbstate == 2){
	    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.toolbar1), GTK_TOOLBAR_ICONS);
	    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_ICONS);
	    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (icons1), TRUE);
	}
	if(tbstate == 3){
	    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.toolbar1), GTK_TOOLBAR_BOTH);
	    gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_BOTH);
	    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (text_and_icons1), TRUE);
	}

    separator2 = gtk_menu_item_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "separator2", separator2);
    gtk_widget_show (separator2);
    gtk_container_add (GTK_CONTAINER (Options_menu), separator2);

    preferences = gtk_menu_item_new_with_label (_("Preferences"));
    gtk_widget_ref (preferences);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "preferences", preferences,
                            (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (preferences);
    gtk_container_add (GTK_CONTAINER (Options_menu), preferences);
    gtk_signal_connect (GTK_OBJECT (preferences), "activate",
                      GTK_SIGNAL_FUNC (on_preferences_activate),
                      NULL);

    About = gtk_menu_item_new_with_label ("About");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "About", About);
    gtk_widget_show (About);
    gtk_container_add (GTK_CONTAINER (menubar1), About);
    gtk_menu_item_right_justify (GTK_MENU_ITEM (About));

    About_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "About_menu", About_menu);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (About), About_menu);

    About_Object_Manager = gtk_menu_item_new_with_label ("About Object Manager");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "About_Object_Manager", About_Object_Manager);
    gtk_widget_show (About_Object_Manager);
    gtk_container_add (GTK_CONTAINER (About_menu), About_Object_Manager);
    gtk_signal_connect (GTK_OBJECT (About_Object_Manager), "activate",
			GTK_SIGNAL_FUNC (obj_on_About_Object_Manager_activate),
			NULL);
    gtk_widget_add_accelerator (About_Object_Manager, "activate", accel_group,
				GDK_a, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

    About_OraLinux = gtk_menu_item_new_with_label ("About OraSoft");
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "About_OraLinux", About_OraLinux);
    gtk_widget_show (About_OraLinux);
    gtk_container_add (GTK_CONTAINER (About_menu), About_OraLinux);
    gtk_signal_connect (GTK_OBJECT (About_OraLinux), "activate",
			GTK_SIGNAL_FUNC (obj_on_About_OraLinux_activate),
			NULL);
    gtk_widget_add_accelerator (About_OraLinux, "activate", accel_group,
				GDK_o, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
				
    
    if( obj_ginfo.dynamicexport && obj_ginfo.total_dlinfo){
        GtkWidget    *modules;
	
        modules = gtk_menu_item_new_with_label ("About Export Modules");
        gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "modules", modules);
        gtk_widget_show (modules);
        gtk_container_add (GTK_CONTAINER (About_menu), modules);
        gtk_signal_connect (GTK_OBJECT (modules), "activate",
 			    GTK_SIGNAL_FUNC (on_About_modules_activate),
			    NULL);
    }

}

