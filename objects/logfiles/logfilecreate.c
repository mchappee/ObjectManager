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

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "include/objectman.h"
#include "objects/tables/tablefuncs.h"
#include "logfilefuncs.h"

GtkWidget *text1;

typedef struct logfile_str{
    char	 *name;
    char         *combogroup;
    char         *textgroup;
    char         *size;
    int          size_letter;
    int          use_existing;
}logfile_struct;

logfile_struct logfile;

static void 
build_logfile_sql()
{
    GString *query = g_string_new("ALTER DATABASE ADD LOGFILE ");

    if(!logfile.use_existing){
        if(logfile.textgroup)
	    g_string_sprintfa(query, "GROUP %s ", logfile.textgroup);
    }else
        g_string_append(query, "MEMBER ");
    
    if(!logfile.use_existing)
        g_string_append(query, "(");
    
    if(logfile.name)
        g_string_sprintfa(query, "'%s'", logfile.name);
	
    if(!logfile.use_existing)
        g_string_append(query, ") ");
    
    if(!logfile.use_existing){
        if(logfile.size){
            if(logfile.size_letter)
	        g_string_sprintfa(query, "SIZE %s K", logfile.size);
	    else
	        g_string_sprintfa(query, "SIZE %s M", logfile.size);
        }
    }

    if(logfile.use_existing)
        g_string_sprintfa(query, " TO GROUP %s ", logfile.combogroup);

    clear_text(text1);
    gtk_text_insert (GTK_TEXT (text1), NULL, NULL,NULL, query->str, query->len);

    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(text1));

    g_string_free(query, 1);
}


static void
on_logfilename_changed                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	logfile.name = entry;
    }else{
	logfile.name = NULL;
    }
    
    build_logfile_sql();
}


static void
on_existing_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	logfile.combogroup = entry;
    }else{
	logfile.combogroup = NULL;
    }
    
    build_logfile_sql();
}


static void
on_newgroup_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	logfile.textgroup = entry;
    }else{
	logfile.textgroup = NULL;
    }

    build_logfile_sql();
}


static void
on_existing_toggled                    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	logfile.use_existing = 1;
    
    build_logfile_sql();
}


static void
on_newgroup_toggled                    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	logfile.use_existing = 0;
    
    build_logfile_sql();
}


static void
on_logsize_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	logfile.size = entry;
    }else{
	logfile.size = NULL;
    }
    
    build_logfile_sql();
}


static void
on_logfile_M_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	logfile.size_letter = 0;
    
    build_logfile_sql();
}


static void
on_logfile_K_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	logfile.size_letter = 1;
    
    build_logfile_sql();
}


static void
on_logfile_execute_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (text1)));

    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("Log File has been created");
    }
    
    g_free(cp);
}


static void
on_logfile_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(user_data);
}


GtkWidget*
create_logfile_window1 (void)
{
  GtkWidget *window1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *entry1;
  GtkWidget *frame1;
  GtkWidget *fixed1;
  GtkWidget *combo1;
  GtkWidget *combo_entry1;
  GtkWidget *entry3;
  GSList *_2_group = NULL;
  GtkWidget *radiobutton3;
  GtkWidget *radiobutton4;
  GtkWidget *hbox1;
  GtkWidget *label2;
  GtkWidget *entry2;
  GSList *_1_group = NULL;
  GtkWidget *radiobutton1;
  GtkWidget *radiobutton2;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkWidget *button2;
  GList     *glist;
  char      *logfilequery = "SELECT distinct group# "
                            "FROM v$logfile";

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
  gtk_window_set_title (GTK_WINDOW (window1), _("Create Logfile"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_widget_set_usize(vbox1, 400, 300);
  gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  label1 = gtk_label_new (_("Log File Name:"));
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

  frame1 = gtk_frame_new (_("Group Info"));
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "frame1", frame1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  fixed1 = gtk_fixed_new ();
  gtk_widget_ref (fixed1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "fixed1", fixed1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fixed1);
  gtk_container_add (GTK_CONTAINER (frame1), fixed1);

  combo1 = gtk_combo_new ();
  gtk_widget_ref (combo1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "combo1", combo1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo1);
  gtk_fixed_put (GTK_FIXED (fixed1), combo1, 120, 8);
  gtk_widget_set_uposition (combo1, 120, 8);
  gtk_widget_set_usize (combo1, 175, 22);

  combo_entry1 = GTK_COMBO (combo1)->entry;
  gtk_widget_ref (combo_entry1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry1", combo_entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry1);

  entry3 = gtk_entry_new ();
  gtk_widget_ref (entry3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "entry3", entry3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry3);
  gtk_fixed_put (GTK_FIXED (fixed1), entry3, 120, 32);
  gtk_widget_set_uposition (entry3, 120, 32);
  gtk_widget_set_usize (entry3, 158, 22);

  radiobutton3 = gtk_radio_button_new_with_label (_2_group, _("Existing:"));
  _2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
  gtk_widget_ref (radiobutton3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton3", radiobutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton3);
  gtk_fixed_put (GTK_FIXED (fixed1), radiobutton3, 16, 8);
  gtk_widget_set_uposition (radiobutton3, 16, 8);
  gtk_widget_set_usize (radiobutton3, 93, 24);

  radiobutton4 = gtk_radio_button_new_with_label (_2_group, _("New:"));
  _2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton4));
  gtk_widget_ref (radiobutton4);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton4", radiobutton4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton4);
  gtk_fixed_put (GTK_FIXED (fixed1), radiobutton4, 16, 32);
  gtk_widget_set_uposition (radiobutton4, 16, 32);
  gtk_widget_set_usize (radiobutton4, 93, 24);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton4), TRUE);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, TRUE, 0);

  label2 = gtk_label_new (_("Size:"));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox1), label2, TRUE, FALSE, 0);

  entry2 = gtk_entry_new ();
  gtk_widget_ref (entry2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "entry2", entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry2);
  gtk_box_pack_start (GTK_BOX (hbox1), entry2, TRUE, TRUE, 0);

  radiobutton1 = gtk_radio_button_new_with_label (_1_group, _("M"));
  _1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
  gtk_widget_ref (radiobutton1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton1", radiobutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (hbox1), radiobutton1, FALSE, FALSE, 0);

  radiobutton2 = gtk_radio_button_new_with_label (_1_group, _("K"));
  _1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
  gtk_widget_ref (radiobutton2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton2", radiobutton2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton2);
  gtk_box_pack_start (GTK_BOX (hbox1), radiobutton2, FALSE, FALSE, 0);

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

  text1 = add_sql_window(vbox1);

  gtk_signal_connect (GTK_OBJECT (entry1), "changed",
                      GTK_SIGNAL_FUNC (on_logfilename_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (combo_entry1), "changed",
                      GTK_SIGNAL_FUNC (on_existing_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entry3), "changed",
                      GTK_SIGNAL_FUNC (on_newgroup_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
                      GTK_SIGNAL_FUNC (on_existing_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (radiobutton4), "toggled",
                      GTK_SIGNAL_FUNC (on_newgroup_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entry2), "changed",
                      GTK_SIGNAL_FUNC (on_logsize_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
                      GTK_SIGNAL_FUNC (on_logfile_M_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
                      GTK_SIGNAL_FUNC (on_logfile_K_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button1), "clicked",
                      GTK_SIGNAL_FUNC (on_logfile_execute_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button2), "clicked",
                      GTK_SIGNAL_FUNC (on_logfile_cancel_clicked),
                      window1);

  glist = glist_from_query(logfilequery, obj_cinfo.login->dbid, 0, FALSE, NULL);

  if (glist)
      gtk_combo_set_popdown_strings (GTK_COMBO (combo1), glist);

  logfile.size_letter = 0;
  logfile.use_existing = 0;
  
  build_logfile_sql();

  return window1;
}


