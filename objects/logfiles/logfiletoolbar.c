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
#include "logfilefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

GtkWidget *text1;
char      *logname;
char      *logthread;
char      *loggroup;

static void
on_rename_logfile_changed              (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char    *renamelogquery;
    char    *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
        renamelogquery = g_malloc(strlen(logname) + 100);
        sprintf(renamelogquery, "ALTER DATABASE RENAME FILE \n'%s' \nTO \n'%s'",logname, entry);
        clear_text(user_data);
        gtk_text_insert (GTK_TEXT (user_data), NULL, NULL,NULL, renamelogquery, strlen(renamelogquery));
    }
}


static void
on_rename_logfile_execute_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (user_data),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (user_data)));

    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("Log File has been renamed");
    }
    
    g_free(cp);
}


static void
on_rename_logfile_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(user_data);
}


static GtkWidget*
rename_logfile (
    GtkButton	 *button,
    gpointer	 user_data)
{
  GtkWidget *window1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *entry1;
  GtkWidget *label2;
  GtkWidget *entry2;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *scrolledwindow1;
  GtkWidget *text1;

  gtk_clist_get_text(GTK_CLIST(user_data), 9, 1, &logname);

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
  gtk_window_set_title (GTK_WINDOW (window1), _("Rename Logfile:"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_widget_set_usize(vbox1, 427, 198);
  gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  label1 = gtk_label_new (_("Rename Logfile From:"));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);

  entry1 = gtk_entry_new ();
  gtk_widget_ref (entry1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "entry1", entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry1);
  gtk_box_pack_start (GTK_BOX (vbox1), entry1, FALSE, FALSE, 0);
  gtk_entry_set_text(GTK_ENTRY(entry1), logname);
  gtk_entry_set_editable (GTK_ENTRY (entry1), FALSE);

  label2 = gtk_label_new (_("To:"));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (vbox1), label2, FALSE, FALSE, 0);

  entry2 = gtk_entry_new ();
  gtk_widget_ref (entry2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "entry2", entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry2);
  gtk_box_pack_start (GTK_BOX (vbox1), entry2, FALSE, FALSE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  button1 = gtk_button_new_with_label (_("Execute"));
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  button2 = gtk_button_new_with_label (_("Cancel"));
  gtk_widget_ref (button2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);

  text1 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "text1", text1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), text1);

  gtk_signal_connect (GTK_OBJECT (entry2), "changed",
                      GTK_SIGNAL_FUNC (on_rename_logfile_changed),
                      text1);
  gtk_signal_connect (GTK_OBJECT (button1), "clicked",
                      GTK_SIGNAL_FUNC (on_rename_logfile_execute_clicked),
                      text1);
  gtk_signal_connect (GTK_OBJECT (button2), "clicked",
                      GTK_SIGNAL_FUNC (on_rename_logfile_cancel_clicked),
                      window1);

  gtk_widget_show(window1);
  return window1;
}


static void
on_logfile_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    char    *droplogquery;
    
    droplogquery = g_malloc(strlen(logname) + 50);
    sprintf(droplogquery, "ALTER DATABASE DROP LOGFILE MEMBER \n'%s'",logname);
    clear_text(user_data);
    gtk_text_insert (GTK_TEXT (user_data), NULL, NULL,NULL, droplogquery, strlen(droplogquery));

}


static void
on_group_toggled                       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    char    *dropgroupquery;
    
    dropgroupquery = g_malloc(strlen(logname) + 50);
    sprintf(dropgroupquery, "ALTER DATABASE DROP LOGFILE GROUP %s", loggroup);
    clear_text(user_data);
    gtk_text_insert (GTK_TEXT (user_data), NULL, NULL,NULL, dropgroupquery, strlen(dropgroupquery));
}


static void
on_drop_log_execute_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (user_data),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (user_data)));

    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("Log File has been dropped");
    }
    
    g_free(cp);
}


static void
on_drop_log_cancel_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(user_data);
}

static GtkWidget*
create_drop_window (
    GtkButton	 *button,
    gpointer	 user_data)
{
  GtkWidget *window1;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GSList *hbox1_group = NULL;
  GtkWidget *radiobutton1;
  GtkWidget *radiobutton3;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *scrolledwindow1;

  gtk_clist_get_text(GTK_CLIST(user_data), 0, 1, &loggroup);
  gtk_clist_get_text(GTK_CLIST(user_data), 9, 1, &logname);

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
  gtk_window_set_title (GTK_WINDOW (window1), _("Drop Log Window"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_widget_set_usize(vbox1, 474, 166);
  gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, TRUE, 0);

  radiobutton1 = gtk_radio_button_new_with_label (hbox1_group, _("Drop This Log File"));
  hbox1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
  gtk_widget_ref (radiobutton1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton1", radiobutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (hbox1), radiobutton1, TRUE, FALSE, 0);

  radiobutton3 = gtk_radio_button_new_with_label (hbox1_group, _("Drop Entire Group"));
  hbox1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
  gtk_widget_ref (radiobutton3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton3", radiobutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton3);
  gtk_box_pack_start (GTK_BOX (hbox1), radiobutton3, TRUE, FALSE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  button1 = gtk_button_new_with_label (_("Execute"));
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  button2 = gtk_button_new_with_label (_("Cancel"));
  gtk_widget_ref (button2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);

  text1 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "text1", text1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), text1);

  gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
                      GTK_SIGNAL_FUNC (on_logfile_toggled),
                      text1);
  gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
                      GTK_SIGNAL_FUNC (on_group_toggled),
                      text1);
  gtk_signal_connect (GTK_OBJECT (button1), "clicked",
                      GTK_SIGNAL_FUNC (on_drop_log_execute_clicked),
                      text1);
  gtk_signal_connect (GTK_OBJECT (button2), "clicked",
                      GTK_SIGNAL_FUNC (on_drop_log_cancel_clicked),
                      window1);

  on_logfile_toggled(GTK_TOGGLE_BUTTON(radiobutton1), text1);
  gtk_widget_show(window1);
  return window1;
}


void 
obj_create_logfile_toolbar(
    GtkCList *clist)
{
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *tmp_toolbar_icon;
    int       tbstate;
    
    obj_mainwin.dynamictoolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_widget_ref (obj_mainwin.dynamictoolbar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.dynamictoolbar1", obj_mainwin.dynamictoolbar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_RELIEF_NONE);
    gtk_widget_show (obj_mainwin.dynamictoolbar1);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.toolbar1), obj_mainwin.dynamictoolbar1, NULL, NULL);
    
    tmp_toolbar_icon = create_logfile_pixmap (LOGFILETOOLBAR_DROP);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button1 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Drop Options"),
					  "Got to the Drop options screen", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    
    tmp_toolbar_icon = create_table_pixmap (TABLETOOLBAR_RENAME);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button2 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Rename Logfile"),
					  "Rename the currently selected logfile", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    
    tbstate=prefs_get_int_var(PREF_TBSTATE);
    if(tbstate == 1)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_TEXT);

    if(tbstate == 2)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_ICONS);

    if(tbstate == 3)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_BOTH);
    
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (create_drop_window),
			clist);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (rename_logfile),
			clist);
    
    
}



