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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/objectman.h"
#include "objects/tables/tablefuncs.h"
#include "datafilefuncs.h"

static void build_generic_datafile_stuff(GtkWidget *dfwindow2);

typedef struct datafilesql_str{
    char	 *filename;
    char	 *tablespace;
    int		 online;
    char	 *filesize;
    int		 filesize_k;
    int		 reuse;
    int		 autoextend;
    char	 *extend;
    int		 extend_k;
    int		 maxunlimited;
    char	 *maxextent;
    int		 maxextent_k;
    int		 alter;
    int		 externalcall;
}datafilesql_struct;

static datafilesql_struct dfsql;

GtkWidget *text2;
GtkWidget *dfwindow2;
GtkWidget *radiobutton6;
GtkWidget *radiobutton5;
GtkWidget *radiobutton10;
GtkWidget *radiobutton9;
GtkWidget *radiobutton8;
GtkWidget *entry3;
GtkWidget *entry4;
GtkWidget *radiobutton7;
GtkWidget *combo1;
GtkWidget *alter_entry3;
GtkWidget *alter_entry4;
GtkWidget *alter_entry5;
GtkWidget *alter_entry6;

static char *pcommand;


static void
build_datafile_sql(void)
{
    GString *query = g_string_new("ALTER TABLESPACE ");
    GString *dfonly = g_string_new(" ");

	if(dfsql.tablespace!=NULL)
	    g_string_append(query, dfsql.tablespace);
    
	g_string_append(query, " ADD DATAFILE ");
   
    if(dfsql.filename!=NULL){
	g_string_sprintfa(query, "'%s' ",dfsql.filename);
	g_string_sprintfa(dfonly, "'%s' ",dfsql.filename);
    }
    
    if(dfsql.filesize!=NULL){
	if(dfsql.filesize_k==1){
	    g_string_sprintfa(query, "SIZE %sK ",dfsql.filesize);
	    g_string_sprintfa(dfonly, "SIZE %sK ",dfsql.filesize);
	}else{
	    g_string_sprintfa(query, "SIZE %sM ",dfsql.filesize);
	    g_string_sprintfa(dfonly, "SIZE %sM ",dfsql.filesize);
	}
    }
    
    if(dfsql.extend!=NULL && dfsql.autoextend!=0){
	if(dfsql.extend_k==1){
	    g_string_sprintfa(query, " AUTOEXTEND ON NEXT %sK",dfsql.extend);
	    g_string_sprintfa(dfonly, " AUTOEXTEND ON NEXT %sK",dfsql.extend);
	}else{
	    g_string_sprintfa(query, " AUTOEXTEND ON NEXT %sM",dfsql.extend);
	    g_string_sprintfa(dfonly, " AUTOEXTEND ON NEXT %sM",dfsql.extend);
	}
    }
    
    if(dfsql.maxunlimited==0 && dfsql.autoextend!=0){
	if(dfsql.maxextent!=NULL && dfsql.maxextent_k!=0){
	    g_string_sprintfa(query, " MAXSIZE %sK",dfsql.maxextent);
	    g_string_sprintfa(dfonly, " MAXSIZE %sK",dfsql.maxextent);
	}
	if(dfsql.maxextent!=NULL && dfsql.maxextent_k!=1){
	    g_string_sprintfa(query, " MAXSIZE %sM",dfsql.maxextent);
	    g_string_sprintfa(dfonly, " MAXSIZE %sM",dfsql.maxextent);
	}
    }

    clear_text(text2);
    if(dfsql.externalcall)
      gtk_text_insert (GTK_TEXT (text2), NULL, NULL,NULL, dfonly->str, dfonly->len);
    else
      gtk_text_insert (GTK_TEXT (text2), NULL, NULL,NULL, query->str, query->len);

    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(text2));

    g_string_free(dfonly, 1);
    g_string_free(query, 1);
}

static void
on_dfentry1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dfsql.filename=entry;
    }else{
	dfsql.filename=NULL;
    }

    build_datafile_sql();
}


static void
on_dfcombo_entry1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dfsql.tablespace=entry;
    }else{
	dfsql.tablespace=NULL;
    }

    build_datafile_sql();
}


static void
on_dfentry2_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dfsql.filesize=entry;
    }else{
	dfsql.filesize=NULL;
    }

    build_datafile_sql();
}


static void
on_dfradiobutton3_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    dfsql.filesize_k=1;
    build_datafile_sql();
}


static void
on_dfradiobutton4_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    dfsql.filesize_k=0;
    build_datafile_sql();
}


static void
on_dfcheckbutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0)
	dfsql.reuse=0;
    else
	dfsql.reuse=1;
    
    build_datafile_sql();
}


static void
on_dfradiobutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    dfsql.online=1;
    build_datafile_sql();
}


static void
on_dfradiobutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    dfsql.online=0;
    build_datafile_sql();
}


static void
on_dfentry3_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dfsql.extend=entry;
    }else{
	dfsql.extend=NULL;
    }

    build_datafile_sql();
}


static void
on_dfentry4_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dfsql.maxextent=entry;
    }else{
	dfsql.maxextent=NULL;
    }

    build_datafile_sql();
}


static void
on_dfradiobutton6_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    dfsql.extend_k=0;
    build_datafile_sql();
}


static void
on_dfradiobutton5_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    dfsql.extend_k=1;
    build_datafile_sql();
}


static void
on_dfradiobutton8_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0)
	dfsql.maxunlimited=1;
    build_datafile_sql();
}


static void
on_dfradiobutton7_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0)
	dfsql.maxunlimited=0;
    build_datafile_sql();
}


static void
on_dfradiobutton10_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    dfsql.maxextent_k=0;
    build_datafile_sql();
}


static void
on_dfradiobutton9_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    dfsql.maxextent_k=1;
    build_datafile_sql();
}


static void
on_dfcheckbutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	gtk_widget_set_sensitive(radiobutton6,0);
	gtk_widget_set_sensitive(radiobutton5,0);
	gtk_widget_set_sensitive(radiobutton10,0);
	gtk_widget_set_sensitive(radiobutton9,0);
	gtk_widget_set_sensitive(radiobutton8,0);
	gtk_widget_set_sensitive(entry3,0);
	gtk_widget_set_sensitive(entry4,0);
	gtk_widget_set_sensitive(radiobutton7,0);
	dfsql.autoextend=0;
    }else{
	gtk_widget_set_sensitive(radiobutton6,1);
	gtk_widget_set_sensitive(radiobutton5,1);
	gtk_widget_set_sensitive(radiobutton10,1);
	gtk_widget_set_sensitive(radiobutton9,1);
	gtk_widget_set_sensitive(radiobutton8,1);
	gtk_widget_set_sensitive(entry3,1);
	gtk_widget_set_sensitive(entry4,1);
	gtk_widget_set_sensitive(radiobutton7,1);
	dfsql.autoextend=1;
    }
    
    build_datafile_sql();
}


static void
on_dfbutton3_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (text2),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (text2)));
    
    if(dfsql.externalcall){
        char *response = (char *)gtk_object_get_data (GTK_OBJECT (user_data), "response");
        pcommand=g_strdup(cp);

	if(strlen(pcommand)<10)
	  *response = 0;
	else
          *response = 1;
	  
        gtk_widget_destroy(GTK_WIDGET(user_data));
	g_free(cp);
        gtk_main_quit();
    }else{
      if (execute_immediate_sql(obj_cinfo.login->dbid,cp)) {
	if(!dfsql.alter)
	    msgbox("Datafile Created");
	else
	    msgbox("Datafile Altered");
      }
      g_free(cp);
    }
}


static void
on_dfbutton4_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
   if(dfsql.externalcall){
     char *response = (char *)gtk_object_get_data (GTK_OBJECT (user_data), "response");
     *response = 0;
     gtk_widget_destroy(GTK_WIDGET(user_data));
     gtk_main_quit();
   }else{
     gtk_widget_destroy(dfwindow2);
   }
}


GtkWidget*
create_datafilewin (void)
{
    dfwindow2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (dfwindow2), "dfwindow2", dfwindow2);
    gtk_window_set_title (GTK_WINDOW (dfwindow2), ("Create Datafile"));
    gtk_window_set_position (GTK_WINDOW (dfwindow2), GTK_WIN_POS_CENTER);

    dfsql.externalcall=0;

    build_generic_datafile_stuff(dfwindow2);

    return dfwindow2;
}

GtkWidget*
create_external_datafilewin (int *response)
{

    dfwindow2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (dfwindow2), "dfwindow2", dfwindow2);
    gtk_window_set_title (GTK_WINDOW (dfwindow2), ("Create Datafile"));
    gtk_window_set_position (GTK_WINDOW (dfwindow2), GTK_WIN_POS_CENTER);
    gtk_object_set_data (GTK_OBJECT (dfwindow2), "response", response);

    dfsql.externalcall=1;

    build_generic_datafile_stuff(dfwindow2);

    return dfwindow2;
}


static void
build_generic_datafile_stuff(
    GtkWidget *dfwindow2)
{
    GtkWidget	 *vbox2;
    GtkWidget	 *notebook2;
    GtkWidget	 *fixed7;
    GtkWidget	 *entry1;
    GtkWidget	 *label7;
    GtkWidget	 *label8;
    GtkWidget	 *combo_entry1;
    GtkWidget	 *frame4;
    GtkWidget	 *fixed9;
    GtkWidget	 *entry2;
    GtkWidget	 *label9;
    GSList	 *g2_group  = NULL;
    GtkWidget	 *radiobutton3;
    GtkWidget	 *radiobutton4;
    GtkWidget	 *checkbutton1;
    GtkWidget	 *frame3;
    GtkWidget	 *fixed8;
    GSList	 *g1_group  = NULL;
    GtkWidget	 *radiobutton1;
    GtkWidget	 *radiobutton2;
    GtkWidget	 *label6;
    GtkWidget	 *fixed10;
    GtkWidget	 *frame5;
    GtkWidget	 *fixed11;
    GtkWidget	 *label10;
    GtkWidget	 *frame6;
    GtkWidget	 *fixed12;
    GSList	 *g4_group  = NULL;
    GSList	 *g5_group  = NULL;
    GSList	 *g3_group  = NULL;
    GtkWidget	 *checkbutton2;
    GtkWidget	 *hbuttonbox2;
    GtkWidget	 *button3;
    GtkWidget	 *button4;
    GtkWidget    *pixmap1;

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox2);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "vbox2", vbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (vbox2, 373, 440);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (dfwindow2), vbox2);

    notebook2 = gtk_notebook_new ();
    gtk_widget_ref (notebook2);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "notebook2", notebook2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notebook2);
    gtk_box_pack_start (GTK_BOX (vbox2), notebook2, TRUE, TRUE, 0);

    fixed7 = gtk_fixed_new ();
    gtk_widget_ref (fixed7);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "fixed7", fixed7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed7);
    gtk_container_add (GTK_CONTAINER (notebook2), fixed7);

    entry1 = gtk_entry_new ();
    gtk_widget_ref (entry1);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "entry1", entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry1);
    gtk_fixed_put (GTK_FIXED (fixed7), entry1, 85, 8);
    gtk_widget_set_uposition (entry1, 85, 8);
    gtk_widget_set_usize (entry1, 248, 24);

    label7 = gtk_label_new (("File Name:"));
    gtk_widget_ref (label7);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "label7", label7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label7);
    gtk_fixed_put (GTK_FIXED (fixed7), label7, 3, 8);
    gtk_widget_set_uposition (label7, 3, 8);
    gtk_widget_set_usize (label7, 80, 24);

    pixmap1 = create_misc_pixmap (ADVANTIO_PIXMAP);
    gtk_widget_ref (pixmap1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "pixmap1", pixmap1,
    			  (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (pixmap1);
    gtk_fixed_put (GTK_FIXED (fixed7), pixmap1, 255, 42);

    label8 = gtk_label_new (("Tablespace"));
    gtk_widget_ref (label8);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "label8", label8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label8);
    gtk_fixed_put (GTK_FIXED (fixed7), label8, 3, 46);
    gtk_widget_set_uposition (label8, 3, 46);
    gtk_widget_set_usize (label8, 80, 24);

    combo1 = gtk_combo_new ();
    gtk_widget_ref (combo1);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "combo1", combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo1);
    gtk_fixed_put (GTK_FIXED (fixed7), combo1, 100, 46);
    gtk_widget_set_uposition (combo1, 100, 46);
    gtk_widget_set_usize (combo1, 135, 22);

    combo_entry1 = GTK_COMBO (combo1)->entry;
    gtk_widget_ref (combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "combo_entry1", combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry1);

    if(dfsql.externalcall)
        gtk_widget_set_sensitive(combo1,0);

    frame4 = gtk_frame_new (NULL);
    gtk_widget_ref (frame4);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "frame4", frame4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame4);
    gtk_fixed_put (GTK_FIXED (fixed7), frame4, 32, 190);
    gtk_widget_set_uposition (frame4, 32, 190);
    gtk_widget_set_usize (frame4, 312, 80);

    fixed9 = gtk_fixed_new ();
    gtk_widget_ref (fixed9);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "fixed9", fixed9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed9);
    gtk_container_add (GTK_CONTAINER (frame4), fixed9);

    entry2 = gtk_entry_new ();
    gtk_widget_ref (entry2);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "entry2", entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry2);
    gtk_fixed_put (GTK_FIXED (fixed9), entry2, 72, 16);
    gtk_widget_set_uposition (entry2, 72, 16);
    gtk_widget_set_usize (entry2, 158, 22);

    label9 = gtk_label_new (("Size:"));
    gtk_widget_ref (label9);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "label9", label9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label9);
    gtk_fixed_put (GTK_FIXED (fixed9), label9, 8, 16);
    gtk_widget_set_uposition (label9, 8, 16);
    gtk_widget_set_usize (label9, 64, 24);

    radiobutton3 = gtk_radio_button_new_with_label (g2_group, ("K"));
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
    gtk_widget_ref (radiobutton3);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton3", radiobutton3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton3);
    gtk_fixed_put (GTK_FIXED (fixed9), radiobutton3, 232, 16);
    gtk_widget_set_uposition (radiobutton3, 232, 16);
    gtk_widget_set_usize (radiobutton3, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton3), TRUE);

    radiobutton4 = gtk_radio_button_new_with_label (g2_group, ("M"));
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton4));
    gtk_widget_ref (radiobutton4);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton4", radiobutton4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton4);
    gtk_fixed_put (GTK_FIXED (fixed9), radiobutton4, 264, 16);
    gtk_widget_set_uposition (radiobutton4, 264, 16);
    gtk_widget_set_usize (radiobutton4, 32, 24);

    checkbutton1 = gtk_check_button_new_with_label (("Reuse"));
    gtk_widget_ref (checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "checkbutton1", checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed9), checkbutton1, 72, 40);
    gtk_widget_set_uposition (checkbutton1, 72, 40);
    gtk_widget_set_usize (checkbutton1, 99, 24);

    frame3 = gtk_frame_new (NULL);
    gtk_widget_ref (frame3);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "frame3", frame3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame3);
    gtk_fixed_put (GTK_FIXED (fixed7), frame3, 32, 130);
    gtk_widget_set_uposition (frame3, 32, 130);
    gtk_widget_set_usize (frame3, 312, 48);

    fixed8 = gtk_fixed_new ();
    gtk_widget_ref (fixed8);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "fixed8", fixed8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed8);
    gtk_container_add (GTK_CONTAINER (frame3), fixed8);

    radiobutton1 = gtk_radio_button_new_with_label (g1_group, ("Online"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
    gtk_widget_ref (radiobutton1);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton1", radiobutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton1);
    gtk_fixed_put (GTK_FIXED (fixed8), radiobutton1, 40, 8);
    gtk_widget_set_uposition (radiobutton1, 40, 8);
    gtk_widget_set_usize (radiobutton1, 93, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton1), TRUE);
    gtk_widget_set_sensitive(radiobutton1,0);

    radiobutton2 = gtk_radio_button_new_with_label (g1_group, ("Offline"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
    gtk_widget_ref (radiobutton2);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton2", radiobutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton2);
    gtk_fixed_put (GTK_FIXED (fixed8), radiobutton2, 160, 8);
    gtk_widget_set_uposition (radiobutton2, 160, 8);
    gtk_widget_set_usize (radiobutton2, 93, 24);
    gtk_widget_set_sensitive(radiobutton2,0);

    label6 = gtk_label_new (("General"));
    gtk_widget_ref (label6);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "label6", label6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label6);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 0), label6);

    fixed10 = gtk_fixed_new ();
    gtk_widget_ref (fixed10);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "fixed10", fixed10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed10);
    gtk_container_add (GTK_CONTAINER (notebook2), fixed10);

    frame5 = gtk_frame_new (NULL);
    gtk_widget_ref (frame5);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "frame5", frame5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame5);
    gtk_fixed_put (GTK_FIXED (fixed10), frame5, 16, 48);
    gtk_widget_set_uposition (frame5, 16, 48);
    gtk_widget_set_usize (frame5, 336, 208);

    fixed11 = gtk_fixed_new ();
    gtk_widget_ref (fixed11);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "fixed11", fixed11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed11);
    gtk_container_add (GTK_CONTAINER (frame5), fixed11);

    entry3 = gtk_entry_new ();
    gtk_widget_ref (entry3);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "entry3", entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry3);
    gtk_fixed_put (GTK_FIXED (fixed11), entry3, 104, 24);
    gtk_widget_set_uposition (entry3, 104, 24);
    gtk_widget_set_usize (entry3, 126, 24);
    gtk_widget_set_sensitive(entry3,0);

    label10 = gtk_label_new (("Increment:"));
    gtk_widget_ref (label10);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "label10", label10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label10);
    gtk_fixed_put (GTK_FIXED (fixed11), label10, 24, 24);
    gtk_widget_set_uposition (label10, 24, 24);
    gtk_widget_set_usize (label10, 72, 24);

    frame6 = gtk_frame_new (("Max Extent"));
    gtk_widget_ref (frame6);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "frame6", frame6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame6);
    gtk_fixed_put (GTK_FIXED (fixed11), frame6, 24, 72);
    gtk_widget_set_uposition (frame6, 24, 72);
    gtk_widget_set_usize (frame6, 280, 96);

    fixed12 = gtk_fixed_new ();
    gtk_widget_ref (fixed12);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "fixed12", fixed12,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed12);
    gtk_container_add (GTK_CONTAINER (frame6), fixed12);

    entry4 = gtk_entry_new ();
    gtk_widget_ref (entry4);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "entry4", entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry4);
    gtk_fixed_put (GTK_FIXED (fixed12), entry4, 88, 40);
    gtk_widget_set_uposition (entry4, 88, 40);
    gtk_widget_set_usize (entry4, 80, 24);
    gtk_widget_set_sensitive(entry4,0);

    radiobutton7 = gtk_radio_button_new_with_label (g4_group, ("Unlimited"));
    g4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton7));
    gtk_widget_ref (radiobutton7);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton7", radiobutton7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton7);
    gtk_fixed_put (GTK_FIXED (fixed12), radiobutton7, 16, 8);
    gtk_widget_set_uposition (radiobutton7, 16, 8);
    gtk_widget_set_usize (radiobutton7, 93, 24);
    gtk_widget_set_sensitive(radiobutton7,0);

    radiobutton8 = gtk_radio_button_new_with_label (g4_group, ("Fixed:"));
    g4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton8));
    gtk_widget_ref (radiobutton8);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton8", radiobutton8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton8);
    gtk_fixed_put (GTK_FIXED (fixed12), radiobutton8, 16, 40);
    gtk_widget_set_uposition (radiobutton8, 16, 40);
    gtk_widget_set_usize (radiobutton8, 72, 24);
    gtk_widget_set_sensitive(radiobutton8,0);

    radiobutton9 = gtk_radio_button_new_with_label (g5_group, ("K"));
    g5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton9));
    gtk_widget_ref (radiobutton9);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton9", radiobutton9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton9);
    gtk_fixed_put (GTK_FIXED (fixed12), radiobutton9, 176, 40);
    gtk_widget_set_uposition (radiobutton9, 176, 40);
    gtk_widget_set_usize (radiobutton9, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton9), TRUE);
    gtk_widget_set_sensitive(radiobutton9,0);

    radiobutton10 = gtk_radio_button_new_with_label (g5_group, ("M"));
    g5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton10));
    gtk_widget_ref (radiobutton10);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton10", radiobutton10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton10);
    gtk_fixed_put (GTK_FIXED (fixed12), radiobutton10, 208, 40);
    gtk_widget_set_uposition (radiobutton10, 208, 40);
    gtk_widget_set_usize (radiobutton10, 32, 24);
    gtk_widget_set_sensitive(radiobutton10,0);

    radiobutton5 = gtk_radio_button_new_with_label (g3_group, ("K"));
    g3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton5));
    gtk_widget_ref (radiobutton5);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton5", radiobutton5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton5);
    gtk_fixed_put (GTK_FIXED (fixed11), radiobutton5, 240, 24);
    gtk_widget_set_uposition (radiobutton5, 240, 24);
    gtk_widget_set_usize (radiobutton5, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton5), TRUE);
    gtk_widget_set_sensitive(radiobutton5,0);

    radiobutton6 = gtk_radio_button_new_with_label (g3_group, ("M"));
    g3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton6));
    gtk_widget_ref (radiobutton6);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "radiobutton6", radiobutton6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton6);
    gtk_fixed_put (GTK_FIXED (fixed11), radiobutton6, 272, 24);
    gtk_widget_set_uposition (radiobutton6, 272, 24);
    gtk_widget_set_usize (radiobutton6, 32, 24);
    gtk_widget_set_sensitive(radiobutton6,0);

    checkbutton2 = gtk_check_button_new_with_label (("Auto Extend"));
    gtk_widget_ref (checkbutton2);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "checkbutton2", checkbutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton2);
    gtk_fixed_put (GTK_FIXED (fixed10), checkbutton2, 16, 24);
    gtk_widget_set_uposition (checkbutton2, 16, 24);
    gtk_widget_set_usize (checkbutton2, 99, 24);

    label7 = gtk_label_new (("Auto Extend"));
    gtk_widget_ref (label7);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "label7", label7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label7);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 1), label7);

    hbuttonbox2 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox2);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "hbuttonbox2", hbuttonbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox2);
    gtk_box_pack_start (GTK_BOX (vbox2), hbuttonbox2, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_SPREAD);

    button3 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button3);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "button3", button3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button3);
    gtk_container_add (GTK_CONTAINER (hbuttonbox2), button3);
    GTK_WIDGET_SET_FLAGS (button3, GTK_CAN_DEFAULT);

    button4 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button4);
    gtk_object_set_data_full (GTK_OBJECT (dfwindow2), "button4", button4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button4);
    gtk_container_add (GTK_CONTAINER (hbuttonbox2), button4);
    GTK_WIDGET_SET_FLAGS (button4, GTK_CAN_DEFAULT);

    text2 = add_sql_window(vbox2);

    dfsql.online=1;
    dfsql.reuse=0;
    dfsql.autoextend=0;
    dfsql.extend_k=1;
    dfsql.filesize_k=1;
    dfsql.maxunlimited=1;
    dfsql.maxextent_k=1;
    dfsql.filename=NULL;
    dfsql.tablespace=NULL;
    dfsql.filesize=NULL;
    dfsql.extend=NULL;;
    dfsql.maxextent=NULL;
    dfsql.alter=0;

    gtk_signal_connect (GTK_OBJECT (entry1), "changed",
			GTK_SIGNAL_FUNC (on_dfentry1_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_dfcombo_entry1_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (entry2), "changed",
			GTK_SIGNAL_FUNC (on_dfentry2_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton3_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton4), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton4_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_dfcheckbutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (entry3), "changed",
			GTK_SIGNAL_FUNC (on_dfentry3_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (entry4), "changed",
			GTK_SIGNAL_FUNC (on_dfentry4_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton7), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton7_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton8), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton8_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton9), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton9_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton10), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton10_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton5), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton5_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton6), "toggled",
			GTK_SIGNAL_FUNC (on_dfradiobutton6_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton2), "toggled",
			GTK_SIGNAL_FUNC (on_dfcheckbutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button3), "clicked",
			GTK_SIGNAL_FUNC (on_dfbutton3_clicked),
			(gpointer)dfwindow2);
			
    gtk_signal_connect (GTK_OBJECT (button4), "clicked",
			GTK_SIGNAL_FUNC (on_dfbutton4_clicked),
			(gpointer)dfwindow2);

    if(!dfsql.externalcall)
        generic_populate_tablespace(combo1,combo_entry1, obj_cinfo.login->dbid);

}

static void
on_dfbutton1_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *entry,*entry2,*sql;

    if(dfsql.alter==1){
	entry=gtk_entry_get_text(GTK_ENTRY(alter_entry3));
	if(strlen(entry)>0){
	    sql=g_malloc(strlen(dfsql.filename) + strlen(entry) + 40);
	    sprintf(sql,"alter database rename file '%s' to '%s'",dfsql.filename,entry);
	    execute_immediate_sql(obj_cinfo.login->dbid,sql);
	    dfsql.filename=g_malloc(strlen(entry)+1);
	    strcpy(dfsql.filename,entry);
	}else{
	    msgbox("You have to enter a value!");
	}
    }

    if(dfsql.alter==2){
	entry=gtk_entry_get_text(GTK_ENTRY(alter_entry4));
	if(strlen(entry)>0){
	    sql=g_malloc(strlen(dfsql.filename) + strlen(entry) + 40);
	    sprintf(sql,"alter database datafile '%s' resize %sK",dfsql.filename,entry);
	    execute_immediate_sql(obj_cinfo.login->dbid,sql);    
	}else{
	    msgbox("You have to enter a value!");
	}
    }
  
    if(dfsql.alter==3){
	entry=gtk_entry_get_text(GTK_ENTRY(alter_entry5));
	entry2=gtk_entry_get_text(GTK_ENTRY(alter_entry6));
	if(strlen(entry)>0){
	    if(strlen(entry2)>0){
		sql=g_malloc(strlen(dfsql.filename) + strlen(entry) + strlen(entry2) + 80);
		sprintf(sql,"alter database datafile '%s' autoextend on next %sK maxsize %sK",dfsql.filename,entry,entry2);
		execute_immediate_sql(obj_cinfo.login->dbid,sql);
	    }else{
		sql=g_malloc(strlen(dfsql.filename) + strlen(entry) + 80);
		sprintf(sql,"alter database datafile '%s' autoextend on next %sK",dfsql.filename,entry);
		execute_immediate_sql(obj_cinfo.login->dbid,sql);
	    }
	}
    }
    msgbox("Datafile Altered");          
}

static void
on_altdf_radiobutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    gtk_widget_set_sensitive(alter_entry3,1);
    gtk_widget_set_sensitive(alter_entry4,0);
    gtk_widget_set_sensitive(alter_entry5,0);
    gtk_widget_set_sensitive(alter_entry5,0);
    dfsql.alter=1;
}


static void
on_altdf_radiobutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    gtk_widget_set_sensitive(alter_entry3,0);
    gtk_widget_set_sensitive(alter_entry4,1);
    gtk_widget_set_sensitive(alter_entry5,0);
    gtk_widget_set_sensitive(alter_entry6,0);
    dfsql.alter=2;
}

static void
on_altdf_radiobutton3_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    gtk_widget_set_sensitive(alter_entry3,0);
    gtk_widget_set_sensitive(alter_entry4,0);
    gtk_widget_set_sensitive(alter_entry5,1);
    gtk_widget_set_sensitive(alter_entry6,1);
    dfsql.alter=3;
}


GtkWidget*
alter_datafilewin(
    char	 *filename,
    char	 *owner,
    char	 *bytes,
    char	 *max,
    char	 *increment)
{
    GtkWidget	 *window2;
    GtkWidget	 *vbox2;
    GtkWidget	 *fixed4;
    GtkWidget	 *frame4;
    GtkWidget	 *fixed5;
    GtkWidget	 *frame6;
    GtkWidget	 *fixed7;
    GtkWidget	 *label2;
    GtkWidget	 *label3;
    GtkWidget	 *frame5;
    GtkWidget	 *fixed6;
    GtkWidget	 *button1;
    GSList	 *g1_group  = NULL;
    GtkWidget	 *radiobutton1;
    GtkWidget	 *radiobutton2;
    GtkWidget	 *radiobutton3;
    GtkWidget	 *label10;
    GtkWidget	 *label20;

    window2=obj_mainwin.window1;
    dfsql.alter=1;
  
    dfsql.filename=g_malloc(strlen(filename)+1);
    strcpy(dfsql.filename,filename);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "vbox2", vbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (vbox2, 373, 440);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), vbox2);

    fixed4 = gtk_fixed_new ();
    gtk_widget_ref (fixed4);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed4", fixed4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed4);
    gtk_container_add (GTK_CONTAINER (vbox2), fixed4);

    frame4 = gtk_frame_new (NULL);
    gtk_widget_ref (frame4);
    gtk_object_set_data_full (GTK_OBJECT (window2), "frame4", frame4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame4);
    gtk_fixed_put (GTK_FIXED (fixed4), frame4, 40, 32);
    gtk_widget_set_uposition (frame4, 40, 32);
    gtk_widget_set_usize (frame4, 312, 56);

    fixed5 = gtk_fixed_new ();
    gtk_widget_ref (fixed5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed5", fixed5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed5);
    gtk_container_add (GTK_CONTAINER (frame4), fixed5);

    alter_entry3 = gtk_entry_new ();
    gtk_widget_ref (alter_entry3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "alter_entry3", alter_entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (alter_entry3);
    gtk_fixed_put (GTK_FIXED (fixed5), alter_entry3, 8, 16);
    gtk_widget_set_uposition (alter_entry3, 8, 16);
    gtk_widget_set_usize (alter_entry3, 296, 24);

    frame6 = gtk_frame_new (NULL);
    gtk_widget_ref (frame6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "frame6", frame6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame6);
    gtk_fixed_put (GTK_FIXED (fixed4), frame6, 40, 176);
    gtk_widget_set_uposition (frame6, 40, 176);
    gtk_widget_set_usize (frame6, 312, 96);

    fixed7 = gtk_fixed_new ();
    gtk_widget_ref (fixed7);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed7", fixed7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed7);
    gtk_container_add (GTK_CONTAINER (frame6), fixed7);

    alter_entry5 = gtk_entry_new ();
    gtk_widget_ref (alter_entry5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "alter_entry5", alter_entry5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (alter_entry5);
    gtk_fixed_put (GTK_FIXED (fixed7), alter_entry5, 80, 16);
    gtk_widget_set_uposition (alter_entry5, 80, 16);
    gtk_widget_set_usize (alter_entry5, 224, 24);

    alter_entry6 = gtk_entry_new ();
    gtk_widget_ref (alter_entry6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "alter_entry6", alter_entry6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (alter_entry6);
    gtk_fixed_put (GTK_FIXED (fixed7), alter_entry6, 80, 56);
    gtk_widget_set_uposition (alter_entry6, 80, 56);
    gtk_widget_set_usize (alter_entry6, 224, 24);

    label2 = gtk_label_new (("Increment:"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed7), label2, 0, 16);
    gtk_widget_set_uposition (label2, 0, 16);
    gtk_widget_set_usize (label2, 80, 24);

    label3 = gtk_label_new (("Max Size:"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed7), label3, 0, 56);
    gtk_widget_set_uposition (label3, 0, 56);
    gtk_widget_set_usize (label3, 80, 24);

    frame5 = gtk_frame_new (NULL);
    gtk_widget_ref (frame5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "frame5", frame5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame5);
    gtk_fixed_put (GTK_FIXED (fixed4), frame5, 40, 112);
    gtk_widget_set_uposition (frame5, 40, 112);
    gtk_widget_set_usize (frame5, 312, 40);

    fixed6 = gtk_fixed_new ();
    gtk_widget_ref (fixed6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed6", fixed6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed6);
    gtk_container_add (GTK_CONTAINER (frame5), fixed6);

    alter_entry4 = gtk_entry_new ();
    gtk_widget_ref (alter_entry4);
    gtk_object_set_data_full (GTK_OBJECT (window2), "alter_entry4", alter_entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (alter_entry4);
    gtk_fixed_put (GTK_FIXED (fixed6), alter_entry4, 8, 8);
    gtk_widget_set_uposition (alter_entry4, 8, 8);
    gtk_widget_set_usize (alter_entry4, 296, 24);

    button1 = gtk_button_new_with_label (("Alter"));
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_fixed_put (GTK_FIXED (fixed4), button1, 176, 280);
    gtk_widget_set_uposition (button1, 176, 280);
    gtk_widget_set_usize (button1, 47, 22);

    radiobutton1 = gtk_radio_button_new_with_label (g1_group, ("Change File Name:"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
    gtk_widget_ref (radiobutton1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton1", radiobutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton1);
    gtk_fixed_put (GTK_FIXED (fixed4), radiobutton1, 40, 8);
    gtk_widget_set_uposition (radiobutton1, 40, 8);
    gtk_widget_set_usize (radiobutton1, 296, 24);

    radiobutton2 = gtk_radio_button_new_with_label (g1_group, ("Change File Size (in kilobytes):"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
    gtk_widget_ref (radiobutton2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton2", radiobutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton2);
    gtk_fixed_put (GTK_FIXED (fixed4), radiobutton2, 40, 88);
    gtk_widget_set_uposition (radiobutton2, 40, 88);
    gtk_widget_set_usize (radiobutton2, 296, 24);

    radiobutton3 = gtk_radio_button_new_with_label (g1_group, ("Change Storage Options (in kilobytes):"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
    gtk_widget_ref (radiobutton3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton3", radiobutton3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton3);
    gtk_fixed_put (GTK_FIXED (fixed4), radiobutton3, 40, 152);
    gtk_widget_set_uposition (radiobutton3, 40, 152);
    gtk_widget_set_usize (radiobutton3, 296, 24);

    label10 = gtk_label_new (("Info"));
    gtk_widget_ref (label10);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label10", label10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label10);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), label10);
			      
    label20 = gtk_label_new (("Edit"));
    gtk_widget_ref (label20);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label20", label20,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label20);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 1), label20);

    gtk_widget_set_sensitive(alter_entry4,0);
    gtk_widget_set_sensitive(alter_entry5,0);
    gtk_widget_set_sensitive(alter_entry6,0);

    gtk_entry_set_text(GTK_ENTRY(alter_entry3),filename);
    gtk_entry_set_text(GTK_ENTRY(alter_entry4),bytes);
    gtk_entry_set_text(GTK_ENTRY(alter_entry6),max);
    gtk_entry_set_text(GTK_ENTRY(alter_entry5),increment);

    gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_altdf_radiobutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_altdf_radiobutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
			GTK_SIGNAL_FUNC (on_altdf_radiobutton3_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_dfbutton1_clicked),
			NULL);

    return window2;
}

char *
getdatafile()
{
    GtkWidget   *dialog;
    int         response=0;

    dialog = create_external_datafilewin(&response);
    gtk_widget_show(dialog);
    gtk_main();

    if(response)
	return pcommand;
    else
	return NULL;
}

