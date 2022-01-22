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
#include "triggerfuncs.h"

typedef struct triggersql_str{
    char	 *tname;
    char	 *tschema;
    char	 *oname;
    char	 *oschema;
    int		 replace;
    char	 *tbody;
    int		 whenflag;
    int		 insertflag;
    int		 deleteflag;
    int		 updateflag;
    int		 referenceflag;
    char	 *oldas;
    char	 *newas;
    char	 *condition;
    int		 colflag;
}triggersql_struct;

static triggersql_struct trigsql;

GtkWidget *trigger_window1;
GtkWidget *trigger_text1;
GtkWidget *trigger_clist1;
GtkWidget *trigger_combo3;

static void 
build_trigger_sql(void)
{
    GString *query = g_string_new("CREATE ");
    int     a=0;
    char    *update_col;
    gulong  flags = 0;

    if(trigsql.replace)
	g_string_append(query, "OR REPLACE ");
      
    g_string_append(query, "TRIGGER ");

    if(trigsql.tschema!=NULL)
	g_string_sprintfa(query, "%s.",trigsql.tschema);

    if(trigsql.tname!=NULL)
	g_string_append(query, trigsql.tname);

    if(trigsql.whenflag)
	g_string_append(query, " AFTER ");
    else
	g_string_append(query, " BEFORE ");

    a = (trigsql.insertflag + trigsql.deleteflag + trigsql.updateflag);

    switch(a){
    case 15:{
	g_string_append(query, "INSERT ON ");
        break;
    }
    case 20:{
	g_string_append(query, "DELETE ON ");
        break;
    }
    case 25:{
	g_string_append(query, "UPDATE OF ");
        break;
    }
    /* ins and del */
    case 35:{
	g_string_append(query, "INSERT OR DELETE ON ");
        break;
    }
    /* ins and up */
    case 40:{
	g_string_append(query, "INSERT OR UPDATE OF ");
        break;
    }
    /* del and up */
    case 45:{
	g_string_append(query, "DELETE OR UPDATE OF ");
        break;
    }
    /* all */
    case 60:{
	g_string_append(query, "INSERT OR DELETE OR UPDATE OF ");
        break;
    }
    }

    if(trigsql.updateflag){
	GList* sel = GTK_CLIST(trigger_clist1)->selection;
      
	a=0;
	while ( sel != NULL ) {
	    int row_index = GPOINTER_TO_INT ( sel->data );
	    gulong* pulong = gtk_clist_get_row_data (GTK_CLIST(trigger_clist1), row_index );
	    gtk_clist_get_text(GTK_CLIST(trigger_clist1), row_index, 0, &update_col);
	    flags |= *pulong;
	    if(a)
		g_string_append(query, ",");
	    g_string_append(query, update_col);
	    a++;
	    sel = sel->next;
	}
      
	gtk_widget_set_sensitive(trigger_clist1,1);
	g_string_append(query, " ON ");
    }else
	gtk_widget_set_sensitive(trigger_clist1,0);

    if(trigsql.oschema!=NULL)
	g_string_sprintfa(query, "%s.",trigsql.oschema);
      
    if(trigsql.oname!=NULL)
	g_string_append(query, trigsql.oname);

    if(trigsql.referenceflag){
	g_string_sprintfa(query, " REFERENCING OLD AS %s NEW AS %s FOR EACH ROW ",trigsql.oldas,trigsql.newas);
	if(trigsql.condition!=NULL)
	    g_string_sprintfa(query, "WHEN (%s)",trigsql.condition);
    }
    
    if(trigsql.tbody!=NULL)
        g_string_sprintfa(query, " %s",trigsql.tbody);
    
    clear_text(trigger_text1);
    gtk_text_insert (GTK_TEXT (trigger_text1), NULL, NULL,NULL, query->str, query->len);
    
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(trigger_text1));
    
    g_string_free(query, 1);
}

static void
on_trigger_tname_entry1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	trigsql.tname=entry;
    }else{
	trigsql.tname=NULL;
    }

    build_trigger_sql();

}

static void
on_trigger_oldas_entry1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	trigsql.oldas=entry;
    }else{
	trigsql.oldas="OLD";
    }

    build_trigger_sql();

}

static void
on_trigger_newas_entry1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	trigsql.newas=entry;
    }else{
	trigsql.newas="NEW";
    }

    build_trigger_sql();

}

static void
on_trigger_tschema_combo_entry1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	trigsql.tschema=entry;
    }else{
	trigsql.tschema=NULL;
    }

    build_trigger_sql();
}


static void
on_trigger_oname_combo_entry2_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    char    *query;
    char    *qtemplate = "SELECT column_name FROM all_tab_columns "
                         "WHERE owner = '%s' and table_name = '%s' "
			 "order by column_name";
			 
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	trigsql.oname=entry;
        query = g_malloc(strlen(qtemplate) + strlen(trigsql.oschema) + strlen(trigsql.oname) + 1);
        sprintf(query, qtemplate, trigsql.oschema, trigsql.oname);
	gtk_clist_clear (GTK_CLIST(trigger_clist1));
        set_clist_from_query(GTK_CLIST(trigger_clist1), query,obj_cinfo.login->dbid, 0, NULL);
	g_free(query);
    }else{
	trigsql.oname=NULL;
    }

    build_trigger_sql();
}


static void
on_trigger_oschema_combo_entry3_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	trigsql.oschema=entry;
	generic_populate_tables(trigger_combo3, entry, obj_cinfo.login->dbid);
    }else{
	trigsql.oschema=NULL;
    }

    build_trigger_sql();
}


static void
on_trigger_ontable_radiobutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
  build_trigger_sql();
}


static void
on_trigger_body_text2_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *cp;
 
    cp = gtk_editable_get_chars(GTK_EDITABLE (editable),(gint) 0,
				(gint) gtk_text_get_length (GTK_TEXT (editable)));
		 
  
    if(strlen(cp)>0){
        g_free(trigsql.tbody);
	trigsql.tbody=g_malloc(strlen(cp)+1);
	strcpy(trigsql.tbody,cp);
    }else{
	trigsql.tbody=NULL;
    }
    
    build_trigger_sql();
    g_free(cp);

}


static void
on_trigger_onviewradiobutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
  build_trigger_sql();
}


static void
on_trigger_enabled_checkbutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	
    }else{
        
    }
    
    build_trigger_sql();
  
}


static void
on_trigger_replace_checkbutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	trigsql.replace=0;
    }else{
        trigsql.replace=1;
    }
    
    build_trigger_sql();
}


static void
on_trigger_insert_checkbutton3_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	trigsql.insertflag=0;
    }else{
        trigsql.insertflag=15;
    }
    
    build_trigger_sql();
}


static void
on_trigger_delete_checkbutton4_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	trigsql.deleteflag=0;
    }else{
        trigsql.deleteflag=20;
    }
    
    build_trigger_sql();
}


static void
on_trigger_update_checkbutton5_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	trigsql.updateflag=0;
    }else{
        trigsql.updateflag=25;
    }
    
    build_trigger_sql();
}


static void
on_trigger_columnlist_select_row(
    GtkCList        *clist,
    gint             row,
    gint             column,
    GdkEvent        *event,
    gpointer         user_data)
{
    build_trigger_sql();
}


static void
on_trigger_before_radiobutton3_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    trigsql.whenflag=0;
    build_trigger_sql();
}


static void
on_trigger_after_radiobutton4_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    trigsql.whenflag=1;
    build_trigger_sql();
}


static void
on_trigger_ref_checkbutton6_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	trigsql.referenceflag=0;
    }else{
        trigsql.referenceflag=1;
    }
    
    build_trigger_sql();
}


static void
on_trigger_condition_text3_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *cp;
 
    cp = gtk_editable_get_chars(GTK_EDITABLE (editable),(gint) 0,
				(gint) gtk_text_get_length (GTK_TEXT (editable)));
		 
  
    if(strlen(cp)>0){
        g_free(trigsql.condition);
	trigsql.condition=g_malloc(strlen(cp)+1);
	strcpy(trigsql.condition,cp);
    }else{
	trigsql.condition=NULL;
    }
    
    build_trigger_sql();
    g_free(cp);

}

static void
on_trigger_cancel_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    g_free(trigsql.tbody);
    g_free(trigsql.condition);
    gtk_widget_destroy(trigger_window1);
}

static void
on_trigger_execute_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (trigger_text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (trigger_text1)));
    
    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("The trigger has been created");
    }
    
    g_free(cp);
    on_trigger_cancel_clicked(NULL,NULL);
}

GtkWidget*
create_trigger_window1 (void)
{
    GtkWidget *vbox1;
    GtkWidget *notebook1;
    GtkWidget *fixed1;
    GtkWidget *entry1;
    GtkWidget *combo1;
    GtkWidget *combo_entry1;
    GtkWidget *frame1;
    GtkWidget *fixed3;
    GtkWidget *combo2;
    GtkWidget *combo_entry2;
    GtkWidget *combo_entry3;
    GtkWidget *label6;
    GtkWidget *label7;
    GtkWidget *label3;
    GtkWidget *label4;
    GtkWidget *label5;
    GSList *g1_group = NULL;
    GtkWidget *radiobutton1;
    GtkWidget *scrolledwindow2;
    GtkWidget *text2;
    GtkWidget *radiobutton2;
    GtkWidget *checkbutton2;
    GtkWidget *checkbutton1;
    GtkWidget *label1;
    GtkWidget *fixed2;
    GtkWidget *frame2;
    GtkWidget *fixed4;
    GtkWidget *label8;
    GtkWidget *checkbutton3;
    GtkWidget *checkbutton4;
    GtkWidget *checkbutton5;
    GtkWidget *scrolledwindow3;
    GtkWidget *label9;
    GtkWidget *label10;
    GSList *g2_group = NULL;
    GtkWidget *radiobutton3;
    GtkWidget *radiobutton4;
    GtkWidget *checkbutton6;
    GtkWidget *frame3;
    GtkWidget *fixed5;
    GtkWidget *entry2;
    GtkWidget *entry3;
    GtkWidget *label11;
    GtkWidget *label12;
    GtkWidget *frame4;
    GtkWidget *scrolledwindow4;
    GtkWidget *text3;
    GtkWidget *label13;
    GtkWidget *label2;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *pixmap1;

    trigger_window1 = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_object_set_data (GTK_OBJECT (trigger_window1), "trigger_window1", trigger_window1);
    gtk_window_set_title (GTK_WINDOW (trigger_window1), "Create Trigger");
    gtk_window_set_position (GTK_WINDOW (trigger_window1), GTK_WIN_POS_CENTER);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (trigger_window1), vbox1);
    gtk_widget_set_usize (vbox1, 402, 561);

    notebook1 = gtk_notebook_new ();
    gtk_widget_ref (notebook1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "notebook1", notebook1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notebook1);
    gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (notebook1), fixed1);

    pixmap1 = create_misc_pixmap (ADVANTIO_PIXMAP);
    gtk_widget_ref (pixmap1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "pixmap1", pixmap1,
		      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (pixmap1);
    gtk_fixed_put (GTK_FIXED (fixed1), pixmap1, 300, 8);

    entry1 = gtk_entry_new ();
    gtk_widget_ref (entry1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "entry1", entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry1);
    gtk_fixed_put (GTK_FIXED (fixed1), entry1, 100, 8);
    gtk_widget_set_uposition (entry1, 100, 8);
    gtk_widget_set_usize (entry1, 158, 22);

    combo1 = gtk_combo_new ();
    gtk_widget_ref (combo1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "combo1", combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo1);
    gtk_fixed_put (GTK_FIXED (fixed1), combo1, 100, 32);
    gtk_widget_set_uposition (combo1, 100, 32);
    gtk_widget_set_usize (combo1, 175, 22);

    combo_entry1 = GTK_COMBO (combo1)->entry;
    gtk_widget_ref (combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "combo_entry1", combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry1);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "frame1", frame1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_fixed_put (GTK_FIXED (fixed1), frame1, 48, 88);
    gtk_widget_set_uposition (frame1, 48, 88);
    gtk_widget_set_usize (frame1, 304, 80);

    fixed3 = gtk_fixed_new ();
    gtk_widget_ref (fixed3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "fixed3", fixed3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed3);
    gtk_container_add (GTK_CONTAINER (frame1), fixed3);

    combo2 = gtk_combo_new ();
    gtk_widget_ref (combo2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "combo2", combo2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo2);
    gtk_fixed_put (GTK_FIXED (fixed3), combo2, 112, 8);
    gtk_widget_set_uposition (combo2, 112, 8);
    gtk_widget_set_usize (combo2, 175, 22);

    combo_entry2 = GTK_COMBO (combo2)->entry;
    gtk_widget_ref (combo_entry2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "combo_entry2", combo_entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry2);

    trigger_combo3 = gtk_combo_new ();
    gtk_widget_ref (trigger_combo3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "trigger_combo3", trigger_combo3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (trigger_combo3);
    gtk_fixed_put (GTK_FIXED (fixed3), trigger_combo3, 112, 40);
    gtk_widget_set_uposition (trigger_combo3, 112, 40);
    gtk_widget_set_usize (trigger_combo3, 175, 22);

    combo_entry3 = GTK_COMBO (trigger_combo3)->entry;
    gtk_widget_ref (combo_entry3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "combo_entry3", combo_entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry3);

    label6 = gtk_label_new ("Schema:");
    gtk_widget_ref (label6);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label6", label6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label6);
    gtk_fixed_put (GTK_FIXED (fixed3), label6, 8, 8);
    gtk_widget_set_uposition (label6, 8, 8);
    /* gtk_widget_set_usize (label6, 104, 24); */

    label7 = gtk_label_new ("Name:");
    gtk_widget_ref (label7);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label7", label7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label7);
    gtk_fixed_put (GTK_FIXED (fixed3), label7, 8, 40);
    gtk_widget_set_uposition (label7, 8, 40);
    /* gtk_widget_set_usize (label7, 104, 24); */

    label3 = gtk_label_new ("Trigger Name:");
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed1), label3, 16, 8);
    gtk_widget_set_uposition (label3, 16, 8);
    /* gtk_widget_set_usize (label3, 120, 24); */

    label4 = gtk_label_new ("Schema:");
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_fixed_put (GTK_FIXED (fixed1), label4, 16, 32);
    gtk_widget_set_uposition (label4, 16, 32);
    /* gtk_widget_set_usize (label4, 120, 24); */

    label5 = gtk_label_new ("Object:");
    gtk_widget_ref (label5);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label5", label5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label5);
    gtk_fixed_put (GTK_FIXED (fixed1), label5, 48, 64);
    gtk_widget_set_uposition (label5, 48, 64);
    /* gtk_widget_set_usize (label5, 72, 24); */

    radiobutton1 = gtk_radio_button_new_with_label (g1_group, "Table");
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
    gtk_widget_ref (radiobutton1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "radiobutton1", radiobutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton1);
    gtk_fixed_put (GTK_FIXED (fixed1), radiobutton1, 136, 64);
    gtk_widget_set_uposition (radiobutton1, 136, 64);
    gtk_widget_set_usize (radiobutton1, 80, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton1), TRUE);

    scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "scrolledwindow2", scrolledwindow2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow2);
    gtk_fixed_put (GTK_FIXED (fixed1), scrolledwindow2, 48, 200);
    gtk_widget_set_uposition (scrolledwindow2, 48, 200);
    gtk_widget_set_usize (scrolledwindow2, 312, 144);

    text2 = gtk_text_new (NULL, NULL);
    gtk_widget_ref (text2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "text2", text2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (text2);
    gtk_container_add (GTK_CONTAINER (scrolledwindow2), text2);
    gtk_text_set_editable (GTK_TEXT (text2), TRUE);

    radiobutton2 = gtk_radio_button_new_with_label (g1_group, "View");
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
    gtk_widget_ref (radiobutton2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "radiobutton2", radiobutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton2);
    gtk_fixed_put (GTK_FIXED (fixed1), radiobutton2, 232, 64);
    gtk_widget_set_uposition (radiobutton2, 232, 64);
    gtk_widget_set_usize (radiobutton2, 80, 24);

    checkbutton2 = gtk_check_button_new_with_label ("Enabled");
    gtk_widget_ref (checkbutton2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "checkbutton2", checkbutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton2);
    gtk_fixed_put (GTK_FIXED (fixed1), checkbutton2, 192, 176);
    gtk_widget_set_uposition (checkbutton2, 192, 176);
    gtk_widget_set_usize (checkbutton2, 144, 24);

    checkbutton1 = gtk_check_button_new_with_label ("Replace if exists");
    gtk_widget_ref (checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "checkbutton1", checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed1), checkbutton1, 48, 176);
    gtk_widget_set_uposition (checkbutton1, 48, 176);
    gtk_widget_set_usize (checkbutton1, 144, 24);

    label1 = gtk_label_new ("General");
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

    fixed2 = gtk_fixed_new ();
    gtk_widget_ref (fixed2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "fixed2", fixed2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed2);
    gtk_container_add (GTK_CONTAINER (notebook1), fixed2);

    frame2 = gtk_frame_new (NULL);
    gtk_widget_ref (frame2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "frame2", frame2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame2);
    gtk_fixed_put (GTK_FIXED (fixed2), frame2, 32, 8);
    gtk_widget_set_uposition (frame2, 32, 8);
    gtk_widget_set_usize (frame2, 336, 168);

    fixed4 = gtk_fixed_new ();
    gtk_widget_ref (fixed4);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "fixed4", fixed4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed4);
    gtk_container_add (GTK_CONTAINER (frame2), fixed4);

    label8 = gtk_label_new ("Fire Trigger:");
    gtk_widget_ref (label8);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label8", label8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label8);
    gtk_fixed_put (GTK_FIXED (fixed4), label8, 16, 8);
    gtk_widget_set_uposition (label8, 16, 8);
    /* gtk_widget_set_usize (label8, 96, 24); */

    checkbutton3 = gtk_check_button_new_with_label ("Insert:");
    gtk_widget_ref (checkbutton3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "checkbutton3", checkbutton3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton3);
    gtk_fixed_put (GTK_FIXED (fixed4), checkbutton3, 16, 32);
    gtk_widget_set_uposition (checkbutton3, 16, 32);
    gtk_widget_set_usize (checkbutton3, 80, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton3), TRUE);

    checkbutton4 = gtk_check_button_new_with_label ("Delete");
    gtk_widget_ref (checkbutton4);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "checkbutton4", checkbutton4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton4);
    gtk_fixed_put (GTK_FIXED (fixed4), checkbutton4, 16, 80);
    gtk_widget_set_uposition (checkbutton4, 16, 80);
    gtk_widget_set_usize (checkbutton4, 80, 24);

    checkbutton5 = gtk_check_button_new_with_label ("Update...");
    gtk_widget_ref (checkbutton5);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "checkbutton5", checkbutton5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton5);
    gtk_fixed_put (GTK_FIXED (fixed4), checkbutton5, 16, 128);
    gtk_widget_set_uposition (checkbutton5, 16, 128);
    gtk_widget_set_usize (checkbutton5, 80, 24);

    scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "scrolledwindow3", scrolledwindow3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow3);
    gtk_fixed_put (GTK_FIXED (fixed4), scrolledwindow3, 104, 40);
    gtk_widget_set_uposition (scrolledwindow3, 104, 40);
    gtk_widget_set_usize (scrolledwindow3, 208, 112);

    trigger_clist1 = gtk_clist_new (2);
    gtk_widget_ref (trigger_clist1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "trigger_clist1", trigger_clist1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (trigger_clist1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow3), trigger_clist1);
    gtk_clist_set_column_width (GTK_CLIST (trigger_clist1), 0, 183);
    gtk_clist_set_column_width (GTK_CLIST (trigger_clist1), 1, 80);
    gtk_clist_set_selection_mode (GTK_CLIST (trigger_clist1), GTK_SELECTION_MULTIPLE);
    gtk_clist_column_titles_show (GTK_CLIST (trigger_clist1));
    gtk_widget_set_sensitive(trigger_clist1,0);

    label9 = gtk_label_new ("Col Name");
    gtk_widget_ref (label9);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label9", label9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label9);
    gtk_clist_set_column_widget (GTK_CLIST (trigger_clist1), 0, label9);

    label10 = gtk_label_new ("");
    gtk_widget_ref (label10);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label10", label10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label10);
    gtk_clist_set_column_widget (GTK_CLIST (trigger_clist1), 1, label10);

    radiobutton3 = gtk_radio_button_new_with_label (g2_group, "Before");
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
    gtk_widget_ref (radiobutton3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "radiobutton3", radiobutton3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton3);
    gtk_fixed_put (GTK_FIXED (fixed4), radiobutton3, 120, 8);
    gtk_widget_set_uposition (radiobutton3, 120, 8);
    gtk_widget_set_usize (radiobutton3, 93, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton3), TRUE);

    radiobutton4 = gtk_radio_button_new_with_label (g2_group, "After");
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton4));
    gtk_widget_ref (radiobutton4);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "radiobutton4", radiobutton4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton4);
    gtk_fixed_put (GTK_FIXED (fixed4), radiobutton4, 224, 8);
    gtk_widget_set_uposition (radiobutton4, 224, 8);
    gtk_widget_set_usize (radiobutton4, 93, 24);

    checkbutton6 = gtk_check_button_new_with_label ("Referencing:");
    gtk_widget_ref (checkbutton6);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "checkbutton6", checkbutton6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton6);
    gtk_fixed_put (GTK_FIXED (fixed2), checkbutton6, 32, 176);
    gtk_widget_set_uposition (checkbutton6, 32, 176);
    gtk_widget_set_usize (checkbutton6, 128, 24);

    frame3 = gtk_frame_new (NULL);
    gtk_widget_ref (frame3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "frame3", frame3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame3);
    gtk_fixed_put (GTK_FIXED (fixed2), frame3, 32, 200);
    gtk_widget_set_uposition (frame3, 32, 200);
    gtk_widget_set_usize (frame3, 336, 152);

    fixed5 = gtk_fixed_new ();
    gtk_widget_ref (fixed5);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "fixed5", fixed5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed5);
    gtk_container_add (GTK_CONTAINER (frame3), fixed5);

    entry2 = gtk_entry_new ();
    gtk_widget_ref (entry2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "entry2", entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry2);
    gtk_fixed_put (GTK_FIXED (fixed5), entry2, 96, 8);
    gtk_widget_set_uposition (entry2, 96, 8);
    gtk_widget_set_usize (entry2, 64, 24);

    entry3 = gtk_entry_new ();
    gtk_widget_ref (entry3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "entry3", entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry3);
    gtk_fixed_put (GTK_FIXED (fixed5), entry3, 248, 8);
    gtk_widget_set_uposition (entry3, 248, 8);
    gtk_widget_set_usize (entry3, 64, 24);

    label11 = gtk_label_new ("Old as:");
    gtk_widget_ref (label11);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label11", label11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label11);
    gtk_fixed_put (GTK_FIXED (fixed5), label11, 16, 8);
    gtk_widget_set_uposition (label11, 16, 8);
    /* gtk_widget_set_usize (label11, 80, 24); */

    label12 = gtk_label_new ("New as:");
    gtk_widget_ref (label12);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label12", label12,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label12);
    gtk_fixed_put (GTK_FIXED (fixed5), label12, 176, 8);
    gtk_widget_set_uposition (label12, 176, 8);
    /* gtk_widget_set_usize (label12, 56, 24); */

    frame4 = gtk_frame_new (NULL);
    gtk_widget_ref (frame4);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "frame4", frame4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame4);
    gtk_fixed_put (GTK_FIXED (fixed5), frame4, 8, 56);
    gtk_widget_set_uposition (frame4, 8, 56);
    gtk_widget_set_usize (frame4, 312, 88);

    scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow4);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "scrolledwindow4", scrolledwindow4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow4);
    gtk_container_add (GTK_CONTAINER (frame4), scrolledwindow4);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    text3 = gtk_text_new (NULL, NULL);
    gtk_widget_ref (text3);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "text3", text3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (text3);
    gtk_container_add (GTK_CONTAINER (scrolledwindow4), text3);
    gtk_text_set_editable (GTK_TEXT (text3), TRUE);

    label13 = gtk_label_new ("Conditions:");
    gtk_widget_ref (label13);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label13", label13,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label13);
    gtk_fixed_put (GTK_FIXED (fixed5), label13, 8, 40);
    gtk_widget_set_uposition (label13, 8, 40);
    gtk_widget_set_usize (label13, 120, 16);

    label2 = gtk_label_new ("Timing");
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (trigger_window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    trigger_text1 = add_sql_window(vbox1);

    gtk_signal_connect (GTK_OBJECT (entry1), "changed",
			GTK_SIGNAL_FUNC (on_trigger_tname_entry1_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (entry2), "changed",
			GTK_SIGNAL_FUNC (on_trigger_oldas_entry1_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (entry3), "changed",
			GTK_SIGNAL_FUNC (on_trigger_newas_entry1_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_trigger_tschema_combo_entry1_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry3), "changed",
			GTK_SIGNAL_FUNC (on_trigger_oname_combo_entry2_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry2), "changed",
			GTK_SIGNAL_FUNC (on_trigger_oschema_combo_entry3_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_ontable_radiobutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (text2), "changed",
			GTK_SIGNAL_FUNC (on_trigger_body_text2_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_onviewradiobutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton2), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_enabled_checkbutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_replace_checkbutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton3), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_insert_checkbutton3_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton4), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_delete_checkbutton4_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton5), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_update_checkbutton5_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (trigger_clist1), "select_row",
			GTK_SIGNAL_FUNC (on_trigger_columnlist_select_row),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_before_radiobutton3_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton4), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_after_radiobutton4_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton6), "toggled",
			GTK_SIGNAL_FUNC (on_trigger_ref_checkbutton6_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (text3), "changed",
			GTK_SIGNAL_FUNC (on_trigger_condition_text3_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_trigger_execute_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_trigger_cancel_clicked),
			NULL);

    trigsql.tname=NULL;
    trigsql.tschema=NULL;
    trigsql.oname=NULL;
    trigsql.oschema=NULL;
    trigsql.replace=0;
    trigsql.tbody=NULL;
    trigsql.whenflag=0;
    trigsql.insertflag=15;
    trigsql.deleteflag=0;
    trigsql.updateflag=0;
    trigsql.referenceflag=0;
    trigsql.oldas="OLD";
    trigsql.newas="NEW";
    trigsql.condition=NULL;
    trigsql.colflag=0;

    /* Dupe call */
    generic_populate_schemas(combo1,combo_entry1, obj_cinfo.login->dbid);
    generic_populate_schemas(combo2,combo_entry2, obj_cinfo.login->dbid);

    return trigger_window1;
}

