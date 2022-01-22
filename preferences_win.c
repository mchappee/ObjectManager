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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "include/objectman.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

  GtkWidget *entry4;
  GtkWidget *entry5;
  GtkWidget *entry6;
  GtkWidget *button4;
  GtkWidget *togglebutton7;
  GtkWidget *togglebutton8;
  GtkWidget *togglebutton9;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;

static void
on_pref_file_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
    char    *filename;
    
    filename = filewin("Select Logging File");
    if(filename){
        gtk_entry_set_text(GTK_ENTRY(entry6), filename);
	g_free(filename);
    }

}


static void
on_pref_logging_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1){
        gtk_label_set_text(GTK_LABEL(label1),"ON");
	gtk_widget_set_sensitive(button4, TRUE);
	gtk_widget_set_sensitive(entry6, TRUE);
    }else{
        gtk_label_set_text(GTK_LABEL(label1),"OFF");
	gtk_widget_set_sensitive(button4, FALSE);
	gtk_widget_set_sensitive(entry6, FALSE);
    }
}


static void
on_pref_highlighting_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1){
        gtk_label_set_text(GTK_LABEL(label2),"ON");
    }else{
        gtk_label_set_text(GTK_LABEL(label2),"OFF");
    }
}


static void
on_pref_modular_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1){
        gtk_label_set_text(GTK_LABEL(label3),"ON");
    }else{
        gtk_label_set_text(GTK_LABEL(label3),"OFF");
    }
}


static void
on_pref_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
    char    *filename;
    char    *rows;
    char    *printcall;
    
    if (GTK_TOGGLE_BUTTON (togglebutton7)->active==1){
        filename = gtk_entry_get_text(GTK_ENTRY(entry6));
	if(strlen(filename) > 0){
            prefs_set_char_var(PREF_LOGGING, filename);
	    obj_ginfo.logsql = g_strdup(filename);
	}else{
	    prefs_unset_char_var(PREF_LOGGING);
	    obj_ginfo.logsql = NULL;
	}
    }else{
        prefs_unset_char_var(PREF_LOGGING);
	obj_ginfo.logsql = NULL;
    }
    
    if (GTK_TOGGLE_BUTTON (togglebutton8)->active==1){
        prefs_set_int_var(PREF_SYNTAX, 1);
	obj_ginfo.highlighting = 1;
    }else{
        prefs_set_int_var(PREF_SYNTAX, 0);
	obj_ginfo.highlighting = 0;
    }
    
    if (GTK_TOGGLE_BUTTON (togglebutton9)->active==1){
        prefs_set_int_var(PREF_MODULAR, 1);
	obj_ginfo.dynamicexport = 1;
    }else{
        prefs_set_int_var(PREF_MODULAR, 0);
	obj_ginfo.dynamicexport = 0;
    }

    rows = gtk_entry_get_text(GTK_ENTRY(entry4));
    if(strlen(rows) > 0){
        obj_ginfo.rows = atoi(rows);
	prefs_set_char_var(PREF_ROWS, rows);
    }else{
        obj_ginfo.rows = 1;
	prefs_unset_char_var(PREF_ROWS);
    }

    printcall = gtk_entry_get_text(GTK_ENTRY(entry5));
    if(strlen(printcall) > 0){
        obj_ginfo.printcall = g_strdup(printcall);
	prefs_set_char_var(PREF_PRINTCALL, printcall);
    }else{
        obj_ginfo.printcall = "lpr";
	prefs_set_char_var(PREF_PRINTCALL, "lpr");
    }

    prefs_save_values();
    gtk_widget_destroy(user_data);

}


static void
on_pref_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(user_data);
}

GtkWidget*
create_preferences_window (void)
{
  GtkWidget *window4;
  GtkWidget *vbox7;
  GtkWidget *table1;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkWidget *label13;
  GtkWidget *label14;
  GtkWidget *label15;
  GtkWidget *hbuttonbox2;
  GtkWidget *button5;
  GtkWidget *button6;
  char      *logging;
  int       highlight;
  int       modular;
  char      char_rows[9];

  window4 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window4), "window4", window4);
  gtk_window_set_title (GTK_WINDOW (window4), ("Preferences"));
  gtk_window_set_position (GTK_WINDOW (window4), GTK_WIN_POS_CENTER);

  vbox7 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox7);
  gtk_object_set_data_full (GTK_OBJECT (window4), "vbox7", vbox7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox7);
  gtk_container_add (GTK_CONTAINER (window4), vbox7);

  table1 = gtk_table_new (5, 4, FALSE);
  gtk_widget_ref (table1);
  gtk_object_set_data_full (GTK_OBJECT (window4), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox7), table1, TRUE, TRUE, 0);

  entry4 = gtk_entry_new ();
  gtk_widget_ref (entry4);
  gtk_object_set_data_full (GTK_OBJECT (window4), "entry4", entry4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry4);
  gtk_table_attach (GTK_TABLE (table1), entry4, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry5 = gtk_entry_new ();
  gtk_widget_ref (entry5);
  gtk_object_set_data_full (GTK_OBJECT (window4), "entry5", entry5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry5);
  gtk_table_attach (GTK_TABLE (table1), entry5, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry6 = gtk_entry_new ();
  gtk_widget_ref (entry6);
  gtk_object_set_data_full (GTK_OBJECT (window4), "entry6", entry6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry6);
  gtk_table_attach (GTK_TABLE (table1), entry6, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  button4 = gtk_button_new_with_label (("..."));
  gtk_widget_ref (button4);
  gtk_object_set_data_full (GTK_OBJECT (window4), "button4", button4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button4);
  gtk_table_attach (GTK_TABLE (table1), button4, 3, 4, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton7 = gtk_toggle_button_new();
  gtk_widget_ref (togglebutton7);
  gtk_object_set_data_full (GTK_OBJECT (window4), "togglebutton7", togglebutton7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton7);
  gtk_table_attach (GTK_TABLE (table1), togglebutton7, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton8 = gtk_toggle_button_new();
  gtk_widget_ref (togglebutton8);
  gtk_object_set_data_full (GTK_OBJECT (window4), "togglebutton8", togglebutton8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton8);
  gtk_table_attach (GTK_TABLE (table1), togglebutton8, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton9 = gtk_toggle_button_new();
  gtk_widget_ref (togglebutton9);
  gtk_object_set_data_full (GTK_OBJECT (window4), "togglebutton9", togglebutton9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton9);
  gtk_table_attach (GTK_TABLE (table1), togglebutton9, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label1 = gtk_label_new (("yada"));
  label2 = gtk_label_new (("yada"));
  label3 = gtk_label_new (("yada"));
  gtk_container_add (GTK_CONTAINER (togglebutton7), label1);
  gtk_container_add (GTK_CONTAINER (togglebutton8), label2);
  gtk_container_add (GTK_CONTAINER (togglebutton9), label3);
  gtk_widget_show(label1);
  gtk_widget_show(label2);
  gtk_widget_show(label3);  


  label11 = gtk_label_new (("SQL Logging:"));
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (window4), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table1), label11, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);

  label12 = gtk_label_new (("Syntax Highlighting:      "));
  gtk_widget_ref (label12);
  gtk_object_set_data_full (GTK_OBJECT (window4), "label12", label12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table1), label12, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);

  label13 = gtk_label_new (("Modular Export:"));
  gtk_widget_ref (label13);
  gtk_object_set_data_full (GTK_OBJECT (window4), "label13", label13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table1), label13, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);

  label14 = gtk_label_new (("# Rows Returned:"));
  gtk_widget_ref (label14);
  gtk_object_set_data_full (GTK_OBJECT (window4), "label14", label14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label14);
  gtk_table_attach (GTK_TABLE (table1), label14, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);

  label15 = gtk_label_new (("Print Call:"));
  gtk_widget_ref (label15);
  gtk_object_set_data_full (GTK_OBJECT (window4), "label15", label15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label15);
  gtk_table_attach (GTK_TABLE (table1), label15, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox2);
  gtk_object_set_data_full (GTK_OBJECT (window4), "hbuttonbox2", hbuttonbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (vbox7), hbuttonbox2, TRUE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_SPREAD);

  button5 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"OK");
  gtk_widget_ref (button5);
  gtk_object_set_data_full (GTK_OBJECT (window4), "button5", button5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button5);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button5);
  GTK_WIDGET_SET_FLAGS (button5, GTK_CAN_DEFAULT);

  button6 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
  gtk_widget_ref (button6);
  gtk_object_set_data_full (GTK_OBJECT (window4), "button6", button6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button6);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button6);
  GTK_WIDGET_SET_FLAGS (button6, GTK_CAN_DEFAULT);


  /* Lets find us some options */

  logging = prefs_get_char_var(PREF_LOGGING);
  if(logging){
      obj_ginfo.logsql = g_strdup(logging);
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (togglebutton7), TRUE);
      gtk_entry_set_text(GTK_ENTRY(entry6), logging);
      gtk_label_set_text(GTK_LABEL(label1),"ON");
  }else{
      gtk_label_set_text(GTK_LABEL(label1),"OFF");
      gtk_widget_set_sensitive(button4, FALSE);
      gtk_widget_set_sensitive(entry6, FALSE);
  }

  highlight = prefs_get_int_var(PREF_SYNTAX);
  if(highlight){
      gtk_label_set_text(GTK_LABEL(label2),"ON");
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (togglebutton8), TRUE);
      obj_ginfo.highlighting = 1;
  }else
      gtk_label_set_text(GTK_LABEL(label2),"OFF");

  modular = prefs_get_int_var(PREF_MODULAR);
  if(modular){
      gtk_label_set_text(GTK_LABEL(label3),"ON");
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (togglebutton9), TRUE);
      obj_ginfo.dynamicexport = 1;
  }else
      gtk_label_set_text(GTK_LABEL(label3),"OFF");

  obj_ginfo.rows = prefs_get_int_var(PREF_ROWS);
  sprintf(char_rows, "%d", obj_ginfo.rows);
  if(obj_ginfo.rows)
      gtk_entry_set_text(GTK_ENTRY(entry4), char_rows);

  obj_ginfo.printcall = g_strdup(prefs_get_char_var(PREF_PRINTCALL));
  if(obj_ginfo.printcall)
      gtk_entry_set_text(GTK_ENTRY(entry5), obj_ginfo.printcall);


  gtk_signal_connect (GTK_OBJECT (button4), "clicked",
                      GTK_SIGNAL_FUNC (on_pref_file_clicked),
                      entry4);
  gtk_signal_connect (GTK_OBJECT (togglebutton7), "toggled",
                      GTK_SIGNAL_FUNC (on_pref_logging_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (togglebutton8), "toggled",
                      GTK_SIGNAL_FUNC (on_pref_highlighting_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (togglebutton9), "toggled",
                      GTK_SIGNAL_FUNC (on_pref_modular_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button5), "clicked",
                      GTK_SIGNAL_FUNC (on_pref_ok_clicked),
                      window4);
  gtk_signal_connect (GTK_OBJECT (button6), "clicked",
                      GTK_SIGNAL_FUNC (on_pref_cancel_clicked),
                      window4);

  return window4;
}

