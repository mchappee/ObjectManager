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
#include "rollbackfuncs.h"
#include "objects/tables/tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

GtkWidget *create_ts_entry2;
GtkWidget *create_ts_clist1;
GtkWidget *create_ts_entry3;
GtkWidget *create_ts_entry4;
GtkWidget *create_ts_entry5;
GtkWidget *create_ts_entry6;
GtkWidget *create_ts_entry7;
GtkWidget *create_ts_entry8;
GtkWidget *create_ts_checkbutton1;
GtkWidget *create_ts_text2;
GtkWidget *create_ts_frame3;
GtkWidget *create_ts_mainwin;

typedef struct rollbacksql_str{
    char		*rollback_name;
    char		*tablespace_name;
    int			online;
    int			override;
    char		*initialsize;
    int			initialsize_k;
    char		*nextsize;
    int			nextsize_k;
    char		*minsize;
    int			minsize_k;
    char		*increase;
    char		*minnumber;
    int			unlimited;
    char		*maxnumber;
    int                 ispublic;
}rollbacksql_struct;

static rollbacksql_struct rollbacksql;

static void 
build_rollback_sql(void)
{
    GString *query = g_string_new("CREATE ");
    
    if(rollbacksql.ispublic)
        g_string_append(query,"PUBLIC ROLLBACK SEGMENT ");
    else
        g_string_append(query,"ROLLBACK SEGMENT ");
    
    if(rollbacksql.rollback_name!=NULL)
	g_string_sprintfa(query, "%s ",rollbacksql.rollback_name);

    if(rollbacksql.tablespace_name!=NULL)
	g_string_sprintfa(query, "TABLESPACE %s ",rollbacksql.tablespace_name);

    if(rollbacksql.override){
	g_string_append(query,"STORAGE (");
	if(rollbacksql.initialsize!=NULL){
	    if(rollbacksql.initialsize_k)
		g_string_sprintfa(query, " INITIAL %sK ",rollbacksql.initialsize);
	    else
		g_string_sprintfa(query, " INITIAL %sM ",rollbacksql.initialsize);
	}
    
	if(rollbacksql.nextsize!=NULL){
	    if(rollbacksql.nextsize_k)
		g_string_sprintfa(query, "NEXT %sK ",rollbacksql.nextsize);
	    else
		g_string_sprintfa(query, "NEXT %sM ",rollbacksql.nextsize);
	}
    
        if(rollbacksql.minsize!=NULL){
	    if(rollbacksql.minsize_k)
		g_string_sprintfa(query, "OPTIMAL %sK ",rollbacksql.minsize);
	    else
		g_string_sprintfa(query, "OPTIMAL %sM ",rollbacksql.minsize);
	}
	  
	if(rollbacksql.minnumber!=NULL)
	    g_string_sprintfa(query, "MINEXTENTS %s ",rollbacksql.minnumber);    
    
	if(rollbacksql.unlimited)
	    g_string_append(query, "MAXEXTENTS UNLIMITED ");
	else{
	    if(rollbacksql.maxnumber!=NULL)
		g_string_sprintfa(query, "MAXEXTENTS %s ",rollbacksql.maxnumber);
	}
    
	if(rollbacksql.increase!=NULL)
	    g_string_sprintfa(query, "PCTINCREASE %s ",rollbacksql.increase);
    
	g_string_append(query,") ");
    
    }
    
    clear_text(create_ts_text2);
    gtk_text_insert (GTK_TEXT (create_ts_text2), NULL, NULL,NULL, query->str, query->len);
    
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(create_ts_text2));
	
    g_string_free(query, 1);
}

static void
on_tsname_entry2_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	rollbacksql.rollback_name=entry;
    }else{
	rollbacksql.rollback_name=NULL;
    }

    build_rollback_sql();

}

static void
on_tsoffline_radiobutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.online=0;
    build_rollback_sql();
}


static void
on_tsonline_radiobutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.online=1;
    build_rollback_sql();
}

static void
on_tsinitial_entry3_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	rollbacksql.initialsize=entry;
    }else{
	rollbacksql.initialsize=NULL;
    }

    build_rollback_sql();

}


static void
on_tsnext_entry4_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	rollbacksql.nextsize=entry;
    }else{
	rollbacksql.nextsize=NULL;
    }

    build_rollback_sql();

}


static void
on_tsmin_entry5_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	rollbacksql.minsize=entry;
    }else{
	rollbacksql.minsize=NULL;
    }
    
    build_rollback_sql();

}


static void
on_tsinc_entry6_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	rollbacksql.increase=entry;
    }else{
	rollbacksql.increase=NULL;
    }

    build_rollback_sql();

}


static void
on_tsmax_entry7_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	rollbacksql.minnumber=entry;
    }else{
	rollbacksql.minnumber=NULL;
    }

    build_rollback_sql();

}


static void
on_tsinitialk_radiobutton5_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.initialsize_k=1;
    build_rollback_sql();
}


static void
on_tsinitialm_radiobutton6_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.initialsize_k=0;
    build_rollback_sql();
}


static void
on_tsminm_radiobutton10_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.minsize_k=0;
    build_rollback_sql();
}


static void
on_tsmink_radiobutton9_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.minsize_k=1;
    build_rollback_sql();
}


static void
on_tsnextk_radiobutton7_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.nextsize_k=1;
    build_rollback_sql();
}


static void
on_tsnextm_radiobutton8_toggled(
    GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    rollbacksql.nextsize_k=0;
    build_rollback_sql();
}


static void
on_tsfixedval_entry8_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	rollbacksql.maxnumber=entry;
    }else{
	rollbacksql.maxnumber=NULL;
    }

    build_rollback_sql();
}


static void
on_tsunlimit_radiobutton11_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.unlimited=1;
    build_rollback_sql();
}


static void
on_tsfixed_radiobutton12_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    rollbacksql.unlimited=0;
    build_rollback_sql();
}

static void
on_rbscombo_entry1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	rollbacksql.tablespace_name=entry;
    }else{
	rollbacksql.tablespace_name=NULL;
    }

    build_rollback_sql();
}


static void
on_tsoverride_checkbutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	rollbacksql.override=0;
	gtk_widget_set_sensitive(create_ts_frame3,0);
    }else{
        gtk_widget_set_sensitive(create_ts_frame3,1);
	rollbacksql.override=1;
    }
    
    build_rollback_sql();
}

static void
on_rollback_checkbutton1_clicked(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	rollbacksql.ispublic=1;
    else
	rollbacksql.ispublic=0;
    
  build_rollback_sql();
}

static void
on_tscreate_button3_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char    onlinetemplate[]="alter rollback segment %s online";
    char    *query;
    char    *cp = gtk_editable_get_chars(GTK_EDITABLE (create_ts_text2),(gint) 0,
	   			      (gint) gtk_text_get_length (GTK_TEXT (create_ts_text2)));
    
    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
        if(rollbacksql.online){
	    query = g_malloc(strlen(onlinetemplate) + strlen(rollbacksql.rollback_name) + 1);
	    sprintf(query, onlinetemplate, rollbacksql.rollback_name);
	    execute_immediate_sql(obj_cinfo.login->dbid, query);
	    g_free(query);
	}
	
	msgbox("Rollback Segment has been created");
	
    }
    
    g_free(cp);
    gtk_widget_destroy(create_ts_mainwin);
}


static void
on_tscancel_button4_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(create_ts_mainwin);
}


GtkWidget*
create_rollbackwin (void)
{
    GtkWidget	 *window2;
    GtkWidget	 *vbox3;
    GtkWidget	 *notebook2;
    GtkWidget	 *fixed5;
    GtkWidget	 *label6;
    GtkWidget	 *frame1;
    GtkWidget	 *fixed6;
    GSList	 *g1_group  = NULL;
    GtkWidget	 *radiobutton2;
    GtkWidget	 *radiobutton1;
    GtkWidget	 *label4;
    GtkWidget	 *fixed8;
    GtkWidget	 *fixed9;
    GtkWidget	 *label10;
    GtkWidget	 *label11;
    GtkWidget	 *label12;
    GtkWidget	 *label13;
    GtkWidget	 *label14;
    GSList	 *g3_group  = NULL;
    GtkWidget	 *radiobutton5;
    GtkWidget	 *radiobutton6;
    GSList	 *g5_group  = NULL;
    GtkWidget	 *radiobutton10;
    GtkWidget	 *radiobutton9;
    GSList	 *g4_group  = NULL;
    GtkWidget	 *radiobutton7;
    GtkWidget	 *radiobutton8;
    GtkWidget	 *frame4;
    GtkWidget	 *fixed10;
    GSList	 *g6_group  = NULL;
    GtkWidget	 *radiobutton11;
    GtkWidget	 *radiobutton12;
    GtkWidget	 *label5;
    GtkWidget	 *hbuttonbox2;
    GtkWidget	 *button3;
    GtkWidget	 *button4;
    GtkWidget    *checkbutton1;
    GtkWidget	 *combo_entry1;
    GtkWidget	 *combo1;

    window2 = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_object_set_data (GTK_OBJECT (window2), "window2", window2);
    gtk_window_set_title (GTK_WINDOW (window2), ("Create Tablespace"));
    gtk_window_set_position (GTK_WINDOW (window2), GTK_WIN_POS_CENTER);
    create_ts_mainwin=window2;

    vbox3 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "vbox3", vbox3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (vbox3, 355, 500);
    gtk_widget_show (vbox3);
    gtk_container_add (GTK_CONTAINER (window2), vbox3);

    notebook2 = gtk_notebook_new ();
    gtk_widget_ref (notebook2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "notebook2", notebook2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notebook2);
    gtk_box_pack_start (GTK_BOX (vbox3), notebook2, TRUE, TRUE, 0);

    fixed5 = gtk_fixed_new ();
    gtk_widget_ref (fixed5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed5", fixed5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed5);
    gtk_container_add (GTK_CONTAINER (notebook2), fixed5);

    create_ts_entry2 = gtk_entry_new ();
    gtk_widget_ref (create_ts_entry2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_entry2", create_ts_entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_entry2);
    gtk_fixed_put (GTK_FIXED (fixed5), create_ts_entry2, 128, 8);
    gtk_widget_set_uposition (create_ts_entry2, 128, 8);
    gtk_widget_set_usize (create_ts_entry2, 184, 24);

    label6 = gtk_label_new (("Rollback Name:"));
    gtk_widget_ref (label6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label6", label6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label6);
    gtk_fixed_put (GTK_FIXED (fixed5), label6, 16, 8);
    gtk_widget_set_uposition (label6, 16, 8);
    /* gtk_widget_set_usize (label6, 112, 24); */

    combo1 = gtk_combo_new ();
    gtk_widget_ref (combo1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "combo1", combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo1);
    gtk_fixed_put (GTK_FIXED (fixed5), combo1, 128, 40);
    gtk_widget_set_uposition (combo1, 128, 40);
    gtk_widget_set_usize (combo1, 135, 22);

    combo_entry1 = GTK_COMBO (combo1)->entry;
    gtk_widget_ref (combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "combo_entry1", combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry1);

    checkbutton1 = gtk_check_button_new_with_label (("PUBLIC"));
    gtk_widget_ref (checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "checkbutton1", checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed5), checkbutton1, 128, 80);
    gtk_widget_set_uposition (checkbutton1, 128, 80);
    gtk_widget_set_usize (checkbutton1, 88, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton1), TRUE);

    frame1 = gtk_frame_new (("Status"));
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "frame1", frame1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_fixed_put (GTK_FIXED (fixed5), frame1, 16, 120);
    gtk_widget_set_uposition (frame1, 16, 120);
    gtk_widget_set_usize (frame1, 296, 48);

    fixed6 = gtk_fixed_new ();
    gtk_widget_ref (fixed6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed6", fixed6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed6);
    gtk_container_add (GTK_CONTAINER (frame1), fixed6);

    radiobutton2 = gtk_radio_button_new_with_label (g1_group, ("Offline"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
    gtk_widget_ref (radiobutton2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton2", radiobutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton2);
    gtk_fixed_put (GTK_FIXED (fixed6), radiobutton2, 192, 3);
    gtk_widget_set_uposition (radiobutton2, 192, 3);
    gtk_widget_set_usize (radiobutton2, 93, 24);

    radiobutton1 = gtk_radio_button_new_with_label (g1_group, ("Online"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
    gtk_widget_ref (radiobutton1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton1", radiobutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton1);
    gtk_fixed_put (GTK_FIXED (fixed6), radiobutton1, 24, 3);
    gtk_widget_set_uposition (radiobutton1, 24, 3);
    gtk_widget_set_usize (radiobutton1, 93, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton1), TRUE);

    label4 = gtk_label_new (("General"));
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 0), label4);

    fixed8 = gtk_fixed_new ();
    gtk_widget_ref (fixed8);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed8", fixed8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed8);
    gtk_container_add (GTK_CONTAINER (notebook2), fixed8);

    create_ts_frame3 = gtk_frame_new (NULL);
    gtk_widget_ref (create_ts_frame3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_frame3", create_ts_frame3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_frame3);
    gtk_fixed_put (GTK_FIXED (fixed8), create_ts_frame3, 8, 24);
    gtk_widget_set_uposition (create_ts_frame3, 8, 24);
    gtk_widget_set_usize (create_ts_frame3, 312, 240);

    fixed9 = gtk_fixed_new ();
    gtk_widget_ref (fixed9);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed9", fixed9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed9);
    gtk_container_add (GTK_CONTAINER (create_ts_frame3), fixed9);

    create_ts_entry3 = gtk_entry_new ();
    gtk_widget_ref (create_ts_entry3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_entry3", create_ts_entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_entry3);
    gtk_fixed_put (GTK_FIXED (fixed9), create_ts_entry3, 88, 8);
    gtk_widget_set_uposition (create_ts_entry3, 88, 8);
    gtk_widget_set_usize (create_ts_entry3, 144, 24);

    create_ts_entry4 = gtk_entry_new ();
    gtk_widget_ref (create_ts_entry4);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_entry4", create_ts_entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_entry4);
    gtk_fixed_put (GTK_FIXED (fixed9), create_ts_entry4, 88, 32);
    gtk_widget_set_uposition (create_ts_entry4, 88, 32);
    gtk_widget_set_usize (create_ts_entry4, 144, 24);

    create_ts_entry5 = gtk_entry_new ();
    gtk_widget_ref (create_ts_entry5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_entry5", create_ts_entry5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_entry5);
    gtk_fixed_put (GTK_FIXED (fixed9), create_ts_entry5, 88, 56);
    gtk_widget_set_uposition (create_ts_entry5, 88, 56);
    gtk_widget_set_usize (create_ts_entry5, 144, 24);

    create_ts_entry6 = gtk_entry_new ();
    gtk_widget_ref (create_ts_entry6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_entry6", create_ts_entry6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_entry6);
    gtk_fixed_put (GTK_FIXED (fixed9), create_ts_entry6, 88, 80);
    gtk_widget_set_uposition (create_ts_entry6, 88, 80);
    gtk_widget_set_usize (create_ts_entry6, 144, 24);

    gtk_widget_set_sensitive(create_ts_entry6,0);

    create_ts_entry7 = gtk_entry_new ();
    gtk_widget_ref (create_ts_entry7);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_entry7", create_ts_entry7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_entry7);
    gtk_fixed_put (GTK_FIXED (fixed9), create_ts_entry7, 88, 104);
    gtk_widget_set_uposition (create_ts_entry7, 88, 104);
    gtk_widget_set_usize (create_ts_entry7, 144, 24);

    label10 = gtk_label_new (("Initial Size:"));
    gtk_widget_ref (label10);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label10", label10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label10);
    gtk_fixed_put (GTK_FIXED (fixed9), label10, 8, 8);
    gtk_widget_set_uposition (label10, 8, 8);
    /* gtk_widget_set_usize (label10, 80, 24); */

    label11 = gtk_label_new (("Next Size:"));
    gtk_widget_ref (label11);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label11", label11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label11);
    gtk_fixed_put (GTK_FIXED (fixed9), label11, 8, 32);
    gtk_widget_set_uposition (label11, 8, 32);
    /* gtk_widget_set_usize (label11, 80, 24); */

    label12 = gtk_label_new (("Optimal Size:"));
    gtk_widget_ref (label12);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label12", label12,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label12);
    gtk_fixed_put (GTK_FIXED (fixed9), label12, 8, 56);
    gtk_widget_set_uposition (label12, 8, 56);
    /* gtk_widget_set_usize (label12, 80, 24); */

    label13 = gtk_label_new (("% Increase:"));
    gtk_widget_ref (label13);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label13", label13,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label13);
    gtk_fixed_put (GTK_FIXED (fixed9), label13, 8, 80);
    gtk_widget_set_uposition (label13, 8, 80);
    /* gtk_widget_set_usize (label13, 80, 24); */

    label14 = gtk_label_new (("Min Number:"));
    gtk_widget_ref (label14);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label14", label14,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label14);
    gtk_fixed_put (GTK_FIXED (fixed9), label14, 8, 104);
    gtk_widget_set_uposition (label14, 8, 104);
    /* gtk_widget_set_usize (label14, 80, 24); */

    gtk_widget_set_sensitive(label13,0);

    radiobutton5 = gtk_radio_button_new_with_label (g3_group, ("K"));
    g3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton5));
    gtk_widget_ref (radiobutton5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton5", radiobutton5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton5);
    gtk_fixed_put (GTK_FIXED (fixed9), radiobutton5, 240, 8);
    gtk_widget_set_uposition (radiobutton5, 240, 8);
    gtk_widget_set_usize (radiobutton5, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton5), TRUE);

    radiobutton6 = gtk_radio_button_new_with_label (g3_group, ("M"));
    g3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton6));
    gtk_widget_ref (radiobutton6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton6", radiobutton6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton6);
    gtk_fixed_put (GTK_FIXED (fixed9), radiobutton6, 272, 8);
    gtk_widget_set_uposition (radiobutton6, 272, 8);
    gtk_widget_set_usize (radiobutton6, 32, 24);

    radiobutton10 = gtk_radio_button_new_with_label (g5_group, ("M"));
    g5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton10));
    gtk_widget_ref (radiobutton10);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton10", radiobutton10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton10);
    gtk_fixed_put (GTK_FIXED (fixed9), radiobutton10, 272, 56);
    gtk_widget_set_uposition (radiobutton10, 272, 56);
    gtk_widget_set_usize (radiobutton10, 32, 24);

    radiobutton9 = gtk_radio_button_new_with_label (g5_group, ("K"));
    g5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton9));
    gtk_widget_ref (radiobutton9);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton9", radiobutton9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton9);
    gtk_fixed_put (GTK_FIXED (fixed9), radiobutton9, 240, 56);
    gtk_widget_set_uposition (radiobutton9, 240, 56);
    gtk_widget_set_usize (radiobutton9, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton9), TRUE);

    radiobutton7 = gtk_radio_button_new_with_label (g4_group, ("K"));
    g4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton7));
    gtk_widget_ref (radiobutton7);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton7", radiobutton7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton7);
    gtk_fixed_put (GTK_FIXED (fixed9), radiobutton7, 240, 32);
    gtk_widget_set_uposition (radiobutton7, 240, 32);
    gtk_widget_set_usize (radiobutton7, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton7), TRUE);

    radiobutton8 = gtk_radio_button_new_with_label (g4_group, ("M"));
    g4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton8));
    gtk_widget_ref (radiobutton8);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton8", radiobutton8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton8);
    gtk_fixed_put (GTK_FIXED (fixed9), radiobutton8, 272, 32);
    gtk_widget_set_uposition (radiobutton8, 272, 32);
    gtk_widget_set_usize (radiobutton8, 32, 24);

    frame4 = gtk_frame_new (("Max Number"));
    gtk_widget_ref (frame4);
    gtk_object_set_data_full (GTK_OBJECT (window2), "frame4", frame4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame4);
    gtk_fixed_put (GTK_FIXED (fixed9), frame4, 24, 144);
    gtk_widget_set_uposition (frame4, 24, 144);
    gtk_widget_set_usize (frame4, 208, 72);

    fixed10 = gtk_fixed_new ();
    gtk_widget_ref (fixed10);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed10", fixed10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed10);
    gtk_container_add (GTK_CONTAINER (frame4), fixed10);

    create_ts_entry8 = gtk_entry_new ();
    gtk_widget_ref (create_ts_entry8);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_entry8", create_ts_entry8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_entry8);
    gtk_fixed_put (GTK_FIXED (fixed10), create_ts_entry8, 112, 24);
    gtk_widget_set_uposition (create_ts_entry8, 112, 24);
    gtk_widget_set_usize (create_ts_entry8, 86, 24);

    radiobutton11 = gtk_radio_button_new_with_label (g6_group, ("Unlimited"));
    g6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton11));
    gtk_widget_ref (radiobutton11);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton11", radiobutton11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton11);
    gtk_fixed_put (GTK_FIXED (fixed10), radiobutton11, 8, 0);
    gtk_widget_set_uposition (radiobutton11, 8, 0);
    gtk_widget_set_usize (radiobutton11, 100, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton11), TRUE);

    radiobutton12 = gtk_radio_button_new_with_label (g6_group, ("Fixed:"));
    g6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton12));
    gtk_widget_ref (radiobutton12);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton12", radiobutton12,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton12);
    gtk_fixed_put (GTK_FIXED (fixed10), radiobutton12, 8, 24);
    gtk_widget_set_uposition (radiobutton12, 8, 24);
    gtk_widget_set_usize (radiobutton12, 94, 24);

    create_ts_checkbutton1 = gtk_check_button_new_with_label (("Override Default"));
    gtk_widget_ref (create_ts_checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_checkbutton1", create_ts_checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed8), create_ts_checkbutton1, 8, 0);
    gtk_widget_set_uposition (create_ts_checkbutton1, 8, 0);
    gtk_widget_set_usize (create_ts_checkbutton1, 120, 24);

    label5 = gtk_label_new (("Extents"));
    gtk_widget_ref (label5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label5", label5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label5);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 1), label5);

    hbuttonbox2 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "hbuttonbox2", hbuttonbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox2);
    gtk_box_pack_start (GTK_BOX (vbox3), hbuttonbox2, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_SPREAD);

    button3 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "button3", button3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button3);
    gtk_container_add (GTK_CONTAINER (hbuttonbox2), button3);
    GTK_WIDGET_SET_FLAGS (button3, GTK_CAN_DEFAULT);

    button4 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button4);
    gtk_object_set_data_full (GTK_OBJECT (window2), "button4", button4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button4);
    gtk_container_add (GTK_CONTAINER (hbuttonbox2), button4);
    GTK_WIDGET_SET_FLAGS (button4, GTK_CAN_DEFAULT);

    create_ts_text2 = add_sql_window(vbox3);

    gtk_signal_connect (GTK_OBJECT (combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_rbscombo_entry1_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (create_ts_entry2), "changed",
			GTK_SIGNAL_FUNC (on_tsname_entry2_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_tsoffline_radiobutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_tsonline_radiobutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (create_ts_entry3), "changed",
			GTK_SIGNAL_FUNC (on_tsinitial_entry3_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (create_ts_entry4), "changed",
			GTK_SIGNAL_FUNC (on_tsnext_entry4_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (create_ts_entry5), "changed",
			GTK_SIGNAL_FUNC (on_tsmin_entry5_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (create_ts_entry6), "changed",
			GTK_SIGNAL_FUNC (on_tsinc_entry6_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (create_ts_entry7), "changed",
			GTK_SIGNAL_FUNC (on_tsmax_entry7_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton5), "toggled",
			GTK_SIGNAL_FUNC (on_tsinitialk_radiobutton5_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton6), "toggled",
			GTK_SIGNAL_FUNC (on_tsinitialm_radiobutton6_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton10), "toggled",
			GTK_SIGNAL_FUNC (on_tsminm_radiobutton10_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton9), "toggled",
			GTK_SIGNAL_FUNC (on_tsmink_radiobutton9_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton7), "toggled",
			GTK_SIGNAL_FUNC (on_tsnextk_radiobutton7_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton8), "toggled",
			GTK_SIGNAL_FUNC (on_tsnextm_radiobutton8_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (create_ts_entry8), "changed",
			GTK_SIGNAL_FUNC (on_tsfixedval_entry8_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton11), "toggled",
			GTK_SIGNAL_FUNC (on_tsunlimit_radiobutton11_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton12), "toggled",
			GTK_SIGNAL_FUNC (on_tsfixed_radiobutton12_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (create_ts_checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_tsoverride_checkbutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button3), "clicked",
			GTK_SIGNAL_FUNC (on_tscreate_button3_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button4), "clicked",
			GTK_SIGNAL_FUNC (on_tscancel_button4_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_rollback_checkbutton1_clicked),
			NULL);

    rollbacksql.tablespace_name  = NULL;
    rollbacksql.rollback_name  = NULL;
    rollbacksql.online           = 1;
    rollbacksql.override         = 0;
    rollbacksql.initialsize      = NULL;
    rollbacksql.initialsize_k    = 1;
    rollbacksql.nextsize         = NULL;
    rollbacksql.nextsize_k       = 1;
    rollbacksql.minsize          = NULL;
    rollbacksql.minsize_k        = 1;
    rollbacksql.increase         = NULL;
    rollbacksql.minnumber        = NULL;
    rollbacksql.unlimited        = 1;
    rollbacksql.maxnumber        = NULL;
    rollbacksql.ispublic         = 1;

    generic_populate_tablespace(combo1,combo_entry1, obj_cinfo.login->dbid);

    gtk_widget_set_sensitive(create_ts_frame3,0);
    build_rollback_sql();
    
    return window2;
}

