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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "include/objectman.h"
#include "sequencefuncs.h"

GtkWidget *seqwin_entry1;
GtkWidget *seqwin_entry3;
GtkWidget *seqwin_entry4;
GtkWidget *seqwin_entry5;
GtkWidget *seqwin_entry6;
GtkWidget *seqwin_combo1;
GtkWidget *seqwin_text1;
GtkWidget *seqwin_combo_entry1;

typedef struct seqsql_str{
    char *sname;
    char *sschema;
    int   ascending;
    char  *min;
    char  *max;
    char  *initial;
    char  *increment;
    int   cache;
    int   order;
    int   cycle;
}seqsql_struct;

seqsql_struct seqsql;

void 
builda_seq_sql()
{
    GString *query = g_string_new("ALTER SEQUENCE ");
    
    if(seqsql.sschema!=NULL)
	g_string_sprintfa(query, "%s.",seqsql.sschema);
    
    if(seqsql.sname!=NULL)
	g_string_append(query, seqsql.sname);
    
    g_string_append(query, " INCREMENT BY ");
  
    if(seqsql.increment!=NULL){
	if(seqsql.ascending)
	    g_string_sprintfa(query, "%s ",seqsql.increment);
	else
	    g_string_sprintfa(query, "-%s ",seqsql.increment);
    }else{
	if(seqsql.ascending)
	    g_string_append(query, "1 ");
	else
	    g_string_append(query, "-1 ");
    }
    	
    if(seqsql.initial!=NULL)
	g_string_sprintfa(query, "START WITH %s ",seqsql.initial);
    
    if(seqsql.max!=NULL)
	g_string_sprintfa(query, "MAXVALUE %s ",seqsql.max);
    else
	g_string_append(query, "NOMAXVALUE ");
    
    if(seqsql.min!=NULL)
	g_string_sprintfa(query, "MINVALUE %s ",seqsql.min);
      
    if(seqsql.cache)
	g_string_append(query, "CACHE 20 ");
    else
	g_string_append(query, "NOCACHE ");
      
    if(seqsql.cycle)
	g_string_append(query, "CYCLE ");
    else
	g_string_append(query, "NOCYCLE ");
      
    if(seqsql.order)
	g_string_append(query, "ORDER ");
    else
	g_string_append(query, "NOORDER"); 
     
    clear_text(seqwin_text1);
    gtk_text_insert (GTK_TEXT (seqwin_text1), NULL, NULL,NULL, query->str, query->len);
    g_string_free(query, 1);
}

  
void
on_seqentry1a_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	seqsql.sname=entry;
    }else{
	seqsql.sname=NULL;
    }

    builda_seq_sql();
}


void
on_seqcombo_entry1a_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	seqsql.sschema=entry;
    }else{
	seqsql.sschema=NULL;
    }

    builda_seq_sql();
}


void
on_seqseqwin_radiobutton1a_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	seqsql.ascending=1;
    else
	seqsql.ascending=0;
    
    builda_seq_sql();
    
}


void
on_seqseqwin_radiobutton2a_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	seqsql.ascending=0;
    else
	seqsql.ascending=1;
    
    builda_seq_sql();
}


void
on_seqseqwin_entry3a_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	seqsql.min=entry;
    }else{
	seqsql.min=NULL;
    }

    builda_seq_sql();
}


void
on_seqseqwin_entry4a_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	seqsql.max=entry;
    }else{
	seqsql.max=NULL;
    }

    builda_seq_sql();
}


void
on_seqseqwin_entry5a_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	seqsql.increment=entry;
    }else{
	seqsql.increment=NULL;
    }

    builda_seq_sql();
}


void
on_seqseqwin_entry6a_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0){
	seqsql.initial=entry;
    }else{
	seqsql.initial=NULL;
    }

    builda_seq_sql();
}


void
on_seqseqwin_radiobutton4a_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	seqsql.cache=1;
    else
	seqsql.cache=0;
    
    builda_seq_sql();
}


void
on_seqseqwin_radiobutton5a_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	seqsql.cache=0;
    else
	seqsql.cache=1;
    
    builda_seq_sql();

}


void
on_seqseqwin_checkbutton1a_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	seqsql.cycle=1;
    else
	seqsql.cycle=0;
    
    builda_seq_sql();
}


void
on_seqseqwin_checkbutton2a_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	seqsql.order=1;
    else
	seqsql.order=0;
    
    builda_seq_sql();
}


void
on_seqbutton1a_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (seqwin_text1),(gint) 0,
                 (gint) gtk_text_get_length (GTK_TEXT (seqwin_text1)));

    if (execute_immediate_sql(obje_cinfo.login->dbid, cp)) {
	msgbox("Sequence has been altered");
    }
    g_free(cp);
}


GtkWidget*
obj_dosequenceview (
    char	 *name,
    char	 *owner,
    char	 *min,
    char	 *max,
    char	 *inc,
    char	 *cycle, 
    char	 *order,
    long	 cache)
{
    GtkWidget	 *vbox1;
    GtkWidget	 *fixed3;
    GtkWidget	 *label1;
    GtkWidget	 *label2;
    GSList	 *fixed3_group	= NULL;
    GtkWidget	 *seqwin_radiobutton1;
    GtkWidget	 *seqwin_radiobutton2;
    GtkWidget	 *fixed2;
    GtkWidget	 *frame1;
    GtkWidget	 *fixed4;
    GtkWidget	 *label3;
    GtkWidget	 *label4;
    GtkWidget	 *label5;
    GtkWidget	 *label6;
    GtkWidget	 *fixed1;
    GtkWidget	 *frame2;
    GtkWidget	 *fixed5;
    GtkWidget	 *frame3;
    GtkWidget	 *fixed6;
    GSList	 *fixed6_group	= NULL;
    GtkWidget	 *seqwin_radiobutton4;
    GtkWidget	 *seqwin_radiobutton5;
    GtkWidget	 *seqwin_checkbutton1;
    GtkWidget	 *seqwin_checkbutton2;
    GtkWidget	 *hbuttonbox1;
    GtkWidget	 *button1;
    GtkWidget	 *button2;
    GtkWidget	 *scrolledwindow1;
    GtkWidget    *notelabel1;

    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_ref (obj_mainwin.notebook1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.notebook1", obj_mainwin.notebook1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.notebook1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
    gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);

    notelabel1 = gtk_label_new (("Sequence Info"));
    gtk_widget_ref (notelabel1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "notelabel1", notelabel1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notelabel1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), notelabel1);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    /*gtk_widget_set_usize (vbox1, 403, 398);*/
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), vbox1);

    fixed3 = gtk_fixed_new ();
    gtk_widget_ref (fixed3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "fixed3", fixed3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed3);
    gtk_box_pack_start (GTK_BOX (vbox1), fixed3, FALSE, TRUE, 0);

    seqwin_entry1 = gtk_entry_new ();
    gtk_widget_ref (seqwin_entry1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_entry1", seqwin_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_entry1);
    gtk_fixed_put (GTK_FIXED (fixed3), seqwin_entry1, 120, 8);
    gtk_widget_set_uposition (seqwin_entry1, 120, 8);
    gtk_widget_set_usize (seqwin_entry1, 158, 22);

    label1 = gtk_label_new (("Name:"));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_fixed_put (GTK_FIXED (fixed3), label1, 16, 8);
    gtk_widget_set_uposition (label1, 16, 8);
    /* gtk_widget_set_usize (label1, 82, 16); */

    seqwin_combo1 = gtk_combo_new ();
    gtk_widget_ref (seqwin_combo1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_combo1", seqwin_combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_combo1);
    gtk_fixed_put (GTK_FIXED (fixed3), seqwin_combo1, 120, 32);
    gtk_widget_set_uposition (seqwin_combo1, 120, 32);
    gtk_widget_set_usize (seqwin_combo1, 175, 22);

    seqwin_combo_entry1 = GTK_COMBO (seqwin_combo1)->entry;
    gtk_widget_ref (seqwin_combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_combo_entry1", seqwin_combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_combo_entry1);

    label2 = gtk_label_new (("Owner:"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed3), label2, 16, 40);
    gtk_widget_set_uposition (label2, 16, 40);
    /* gtk_widget_set_usize (label2, 82, 16); */

    seqwin_radiobutton1 = gtk_radio_button_new_with_label (fixed3_group, ("Ascending"));
    fixed3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seqwin_radiobutton1));
    gtk_widget_ref (seqwin_radiobutton1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_radiobutton1", seqwin_radiobutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_radiobutton1);
    gtk_fixed_put (GTK_FIXED (fixed3), seqwin_radiobutton1, 64, 64);
    gtk_widget_set_uposition (seqwin_radiobutton1, 64, 64);
    gtk_widget_set_usize (seqwin_radiobutton1, 93, 24);

    seqwin_radiobutton2 = gtk_radio_button_new_with_label (fixed3_group, ("Descending"));
    fixed3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seqwin_radiobutton2));
    gtk_widget_ref (seqwin_radiobutton2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_radiobutton2", seqwin_radiobutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_radiobutton2);
    gtk_fixed_put (GTK_FIXED (fixed3), seqwin_radiobutton2, 184, 64);
    gtk_widget_set_uposition (seqwin_radiobutton2, 184, 64);
    gtk_widget_set_usize (seqwin_radiobutton2, 93, 24);

    fixed2 = gtk_fixed_new ();
    gtk_widget_ref (fixed2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "fixed2", fixed2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed2);
    gtk_box_pack_start (GTK_BOX (vbox1), fixed2, FALSE, TRUE, 0);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "frame1", frame1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_fixed_put (GTK_FIXED (fixed2), frame1, 48, 0);
    gtk_widget_set_uposition (frame1, 48, 0);
    gtk_widget_set_usize (frame1, 304, 88);

    fixed4 = gtk_fixed_new ();
    gtk_widget_ref (fixed4);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "fixed4", fixed4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed4);
    gtk_container_add (GTK_CONTAINER (frame1), fixed4);

    seqwin_entry3 = gtk_entry_new ();
    gtk_widget_ref (seqwin_entry3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_entry3", seqwin_entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_entry3);
    gtk_fixed_put (GTK_FIXED (fixed4), seqwin_entry3, 56, 8);
    gtk_widget_set_uposition (seqwin_entry3, 56, 8);
    gtk_widget_set_usize (seqwin_entry3, 80, 24);

    seqwin_entry4 = gtk_entry_new ();
    gtk_widget_ref (seqwin_entry4);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_entry4", seqwin_entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_entry4);
    gtk_fixed_put (GTK_FIXED (fixed4), seqwin_entry4, 56, 40);
    gtk_widget_set_uposition (seqwin_entry4, 56, 40);
    gtk_widget_set_usize (seqwin_entry4, 80, 24);

    seqwin_entry5 = gtk_entry_new ();
    gtk_widget_ref (seqwin_entry5);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_entry5", seqwin_entry5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_entry5);
    gtk_fixed_put (GTK_FIXED (fixed4), seqwin_entry5, 208, 8);
    gtk_widget_set_uposition (seqwin_entry5, 208, 8);
    gtk_widget_set_usize (seqwin_entry5, 80, 24);

    seqwin_entry6 = gtk_entry_new ();
    gtk_widget_ref (seqwin_entry6);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_entry6", seqwin_entry6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_entry6);
    gtk_fixed_put (GTK_FIXED (fixed4), seqwin_entry6, 208, 40);
    gtk_widget_set_uposition (seqwin_entry6, 208, 40);
    gtk_widget_set_usize (seqwin_entry6, 80, 24);

    label3 = gtk_label_new (("Min:"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed4), label3, 8, 8);
    gtk_widget_set_uposition (label3, 8, 8);
    /* gtk_widget_set_usize (label3, 34, 16); */

    label4 = gtk_label_new (("Max:"));
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_fixed_put (GTK_FIXED (fixed4), label4, 8, 48);
    gtk_widget_set_uposition (label4, 8, 48);
    /* gtk_widget_set_usize (label4, 34, 16); */

    label5 = gtk_label_new (("Increment:"));
    gtk_widget_ref (label5);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label5", label5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label5);
    gtk_fixed_put (GTK_FIXED (fixed4), label5, 144, 8);
    gtk_widget_set_uposition (label5, 144, 8);
    /* gtk_widget_set_usize (label5, 56, 16); */

    label6 = gtk_label_new (("Initial:"));
    gtk_widget_ref (label6);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "label6", label6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label6);
    gtk_fixed_put (GTK_FIXED (fixed4), label6, 160, 48);
    gtk_widget_set_uposition (label6, 160, 48);
    /* gtk_widget_set_usize (label6, 40, 16); */

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_box_pack_start (GTK_BOX (vbox1), fixed1, FALSE, TRUE, 0);

    frame2 = gtk_frame_new (NULL);
    gtk_widget_ref (frame2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "frame2", frame2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame2);
    gtk_fixed_put (GTK_FIXED (fixed1), frame2, 48, 0);
    gtk_widget_set_uposition (frame2, 48, 0);
    gtk_widget_set_usize (frame2, 304, 88);

    fixed5 = gtk_fixed_new ();
    gtk_widget_ref (fixed5);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "fixed5", fixed5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed5);
    gtk_container_add (GTK_CONTAINER (frame2), fixed5);

    frame3 = gtk_frame_new (NULL);
    gtk_widget_ref (frame3);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "frame3", frame3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame3);
    gtk_fixed_put (GTK_FIXED (fixed5), frame3, 16, 32);
    gtk_widget_set_uposition (frame3, 16, 32);
    gtk_widget_set_usize (frame3, 272, 40);

    fixed6 = gtk_fixed_new ();
    gtk_widget_ref (fixed6);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "fixed6", fixed6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed6);
    gtk_container_add (GTK_CONTAINER (frame3), fixed6);

    seqwin_radiobutton4 = gtk_radio_button_new_with_label (fixed6_group, ("Default Cache"));
    fixed6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seqwin_radiobutton4));
    gtk_widget_ref (seqwin_radiobutton4);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_radiobutton4", seqwin_radiobutton4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_radiobutton4);
    gtk_fixed_put (GTK_FIXED (fixed6), seqwin_radiobutton4, 24, 8);
    gtk_widget_set_uposition (seqwin_radiobutton4, 24, 8);
    gtk_widget_set_usize (seqwin_radiobutton4, 112, 24);

    seqwin_radiobutton5 = gtk_radio_button_new_with_label (fixed6_group, ("No Cache"));
    fixed6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seqwin_radiobutton5));
    gtk_widget_ref (seqwin_radiobutton5);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_radiobutton5", seqwin_radiobutton5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_radiobutton5);
    gtk_fixed_put (GTK_FIXED (fixed6), seqwin_radiobutton5, 144, 8);
    gtk_widget_set_uposition (seqwin_radiobutton5, 144, 8);
    gtk_widget_set_usize (seqwin_radiobutton5, 93, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seqwin_radiobutton5), TRUE);

    seqwin_checkbutton1 = gtk_check_button_new_with_label (("Cycle Values"));
    gtk_widget_ref (seqwin_checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_checkbutton1", seqwin_checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed5), seqwin_checkbutton1, 16, 8);
    gtk_widget_set_uposition (seqwin_checkbutton1, 16, 8);
    gtk_widget_set_usize (seqwin_checkbutton1, 99, 24);

    seqwin_checkbutton2 = gtk_check_button_new_with_label (("Order Values"));
    gtk_widget_ref (seqwin_checkbutton2);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_checkbutton2", seqwin_checkbutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seqwin_checkbutton2);
    gtk_fixed_put (GTK_FIXED (fixed5), seqwin_checkbutton2, 160, 8);
    gtk_widget_set_uposition (seqwin_checkbutton2, 160, 8);
    gtk_widget_set_usize (seqwin_checkbutton2, 99, 24);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);

    button1 = gtk_button_new_with_label (("Alter Sequence"));
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "scrolledwindow1", scrolledwindow1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow1);
    gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    seqwin_text1 = gtk_text_new (NULL, NULL);
    gtk_widget_ref (seqwin_text1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "seqwin_text1", seqwin_text1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_text_set_word_wrap(GTK_TEXT (seqwin_text1),TRUE);
    gtk_widget_show (seqwin_text1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), seqwin_text1);
    gtk_text_set_editable (GTK_TEXT (seqwin_text1), TRUE);

    gtk_signal_connect (GTK_OBJECT (seqwin_entry1), "changed",
			GTK_SIGNAL_FUNC (on_seqentry1a_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_seqcombo_entry1a_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_seqseqwin_radiobutton1a_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_seqseqwin_radiobutton2a_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_entry3), "changed",
			GTK_SIGNAL_FUNC (on_seqseqwin_entry3a_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_entry4), "changed",
			GTK_SIGNAL_FUNC (on_seqseqwin_entry4a_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_entry5), "changed",
			GTK_SIGNAL_FUNC (on_seqseqwin_entry5a_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_entry6), "changed",
			GTK_SIGNAL_FUNC (on_seqseqwin_entry6a_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_radiobutton4), "toggled",
			GTK_SIGNAL_FUNC (on_seqseqwin_radiobutton4a_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_radiobutton5), "toggled",
			GTK_SIGNAL_FUNC (on_seqseqwin_radiobutton5a_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_seqseqwin_checkbutton1a_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (seqwin_checkbutton2), "toggled",
			GTK_SIGNAL_FUNC (on_seqseqwin_checkbutton2a_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_seqbutton1a_clicked),
			NULL);

    seqsql.sname=NULL;
    seqsql.ascending=1;
    seqsql.min=NULL;
    seqsql.max=NULL;
    seqsql.initial=NULL;
    seqsql.increment=NULL;
    seqsql.cache=0;
    seqsql.order=0;
    seqsql.cycle=0;

    gtk_entry_set_text(GTK_ENTRY(seqwin_entry1),name);  
    gtk_entry_set_text(GTK_ENTRY(seqwin_entry3),min);
    gtk_entry_set_text(GTK_ENTRY(seqwin_entry4),max);
    gtk_entry_set_text(GTK_ENTRY(seqwin_entry5),inc);
    gtk_entry_set_text(GTK_ENTRY(seqwin_combo_entry1),owner);
    
    if(cache)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seqwin_radiobutton4), TRUE);
    if(strncmp(cycle,"Y",1)==0)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seqwin_checkbutton1), TRUE);
    if(strncmp(order,"Y",1)==0)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seqwin_checkbutton2), TRUE);
  
    gtk_widget_set_sensitive(seqwin_entry1,0);
    gtk_widget_set_sensitive(seqwin_entry6,0);
    gtk_widget_set_sensitive(seqwin_combo_entry1,0);

    return obj_mainwin.window1;
}

