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
#include "synonymfuncs.h"

typedef struct syn_str{
  GtkWidget *window1;
  GtkWidget *entry1;
  GtkWidget *combo1;
  GtkWidget *combo2;
  GtkWidget *combo3;
  GtkWidget *combo4;
  GtkWidget *combo_entry1;
  GtkWidget *combo_entry3;
  GtkWidget *combo_entry4;
  GtkWidget *combo_entry2;
  GtkWidget *text1;
}obj_syn_struct;

typedef struct synsql_str{
    char	 *sname;
    char	 *sschema;
    char	 *tname;
    char	 *tschema;
    char	 *dblink;
    int		 toggle;
    GList	 *schema_items;
    int		 ispublic;
}synsql_struct;

void syn_loadobjtype(int);

obj_syn_struct obj_synwin;
synsql_struct synsql;

void 
build_syn_sql()
{
    GString *query = g_string_new("CREATE ");
  
    if(synsql.ispublic)
	g_string_append(query, "PUBLIC SYNONYM ");
    else
	g_string_append(query, "SYNONYM ");
  
    if(synsql.sschema!=NULL && synsql.ispublic == 0)
	g_string_sprintfa(query, "%s.",synsql.sschema);
    
    if(synsql.sname!=NULL)
	g_string_append(query, synsql.sname);
    
    g_string_append(query, " FOR ");
  
    if(synsql.tschema!=NULL)
	g_string_sprintfa(query, "%s.",synsql.tschema);
    
    if(synsql.tname!=NULL)
	g_string_append(query, synsql.tname);
    
    if(synsql.dblink!=NULL)
	g_string_sprintfa(query, "@%s",synsql.dblink);
  
    clear_text(obj_synwin.text1);
    gtk_text_insert (GTK_TEXT (obj_synwin.text1), NULL, NULL,NULL, query->str, query->len);
    
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(obj_synwin.text1));
    
    g_string_free(query, 1);
}


void
on_sname_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	synsql.sname=entry;
    }else{
	synsql.sname=NULL;
    }

    build_syn_sql();
}


void
on_sschema_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	synsql.sschema=entry;
    }else{
	synsql.sschema=NULL;
    }

    build_syn_sql();
}

static void
free_glist_string_item(
    gpointer list_data,
    gpointer user_data)
{
    g_free(list_data);
}

void
on_otype_changed(
    GtkEditable	     *editable,
    gpointer	     user_data)
{
    char	     *entry;
    char	     *owner;
    GList	     *combo1_items  = NULL;
    char             *query;
    static char      qtemplate[] = 
	"SELECT object_name "
	"FROM   all_objects "
	"WHERE  object_type = '%s'"
	"       AND owner = '%s'";
    
    entry=gtk_entry_get_text(GTK_ENTRY(obj_synwin.combo_entry2));
    if (synsql.toggle && synsql.tschema!=NULL) {
 
	synsql.dblink=NULL;
 
	owner=synsql.tschema;

	query = g_malloc(sizeof(qtemplate) + strlen(entry) + strlen(owner) + 1);
	sprintf(query, qtemplate, entry, owner);
	combo1_items = glist_from_query(query, obj_cinfo.login->dbid, 0, TRUE, 0);
	g_free(query);
      
	if (combo1_items) {
	    gtk_combo_set_popdown_strings (GTK_COMBO (obj_synwin.combo4), combo1_items);
	    g_list_foreach (combo1_items, free_glist_string_item, 0);
	    g_list_free (combo1_items);
	} else {
	    gtk_list_clear_items (GTK_LIST (GTK_COMBO (obj_synwin.combo4)->list), 0, -1);
	}
    } else {
	if(strlen(entry)>0){
	    synsql.dblink=entry;
	}else{
	    synsql.dblink=NULL;
	}
	build_syn_sql();
    }

    return;
}

void
on_tschema_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	synsql.tschema=entry;
    }else{
	synsql.tschema=NULL;
    }
    
    on_otype_changed(NULL,NULL);
    build_syn_sql();
}


void
on_tobject_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	synsql.tname=entry;
    }else{
	synsql.tname=NULL;
    }
    
    build_syn_sql();
}


void
on_cdb_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1){
	synsql.toggle=1;
	syn_loadobjtype(1);
    }
    
    build_syn_sql();
    
}


void
on_rdb_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1){
	synsql.toggle=0;
	syn_loadobjtype(0);
    }
    
    build_syn_sql();
}


void
on_syncancel_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(obj_synwin.window1);
}


void
on_synok_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (obj_synwin.text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (obj_synwin.text1)));
    
    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("Synonym has been created");
    }
    gtk_widget_destroy(obj_synwin.window1);
    g_free(cp);
}

void
on_syn_checkbutton1_clicked(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	synsql.ispublic=1;
    else
	synsql.ispublic=0;
    
  build_syn_sql();
}


void
syn_loadobjtype(int a)
{
    char           *datatypes[7] = {"FUNCTION", "PACKAGE", "PROCEDURE", "SEQUENCE", "SYNONYM", "TABLE", "VIEW" } ;
    GList          *combo1_items = NULL;
    GList          *tmpglist = NULL;
    GList          *glist = NULL;
    int            i;
    char           *cdbid;
    char           *qtemplate = "SELECT db_link FROM all_db_links";

    if(a){
	for (i = 0; i < sizeof(datatypes)/sizeof(datatypes[0]); i++)
	    glist = g_list_append(glist, datatypes[i]);
  
	gtk_combo_set_popdown_strings( GTK_COMBO(obj_synwin.combo2), glist);
  
	if(obj_cinfo.currenttype != OMDB_Synonym){
	    gtk_combo_set_popdown_strings (GTK_COMBO (obj_synwin.combo3), synsql.schema_items);
	}else{
	    gtk_combo_set_popdown_strings (GTK_COMBO (obj_synwin.combo3), synsql.schema_items);
	    gtk_entry_set_text(GTK_ENTRY(obj_synwin.combo_entry3),obj_cinfo.login->owner);
	}
   
	synsql.dblink=NULL;

    }else{

	cdbid=obj_cinfo.login->dbid;

	combo1_items = glist_from_query(qtemplate, obj_cinfo.login->dbid, 0, TRUE, 0);
      
	if (combo1_items) {
	    gtk_combo_set_popdown_strings (GTK_COMBO (obj_synwin.combo2), combo1_items);
	    g_list_foreach (combo1_items, free_glist_string_item, 0);
	    g_list_free (combo1_items);
	} else {
	    gtk_list_clear_items (GTK_LIST (GTK_COMBO (obj_synwin.combo2)->list), 0, -1);
	}

	tmpglist = g_list_append(tmpglist, "");
	gtk_combo_set_popdown_strings( GTK_COMBO(obj_synwin.combo3), tmpglist) ;
	gtk_combo_set_popdown_strings( GTK_COMBO(obj_synwin.combo4), tmpglist) ;
  
    }

}

GtkWidget*
obj_create_synwin ()
{
    GtkWidget	 *vbox1;
    GtkWidget	 *fixed1;
    GtkWidget	 *frame1;
    GtkWidget	 *fixed2;
    GtkWidget	 *label2;
    GtkWidget	 *label1;
    GtkWidget	 *frame3;
    GtkWidget	 *fixed4;
    GtkWidget	 *label4;
    GtkWidget	 *label5;
    GtkWidget	 *frame2;
    GtkWidget	 *fixed3;
    GtkWidget	 *label3;
    GSList	 *g1_group  = NULL;
    GtkWidget	 *radiobutton1;
    GtkWidget	 *radiobutton2;
    GtkWidget	 *hbuttonbox1;
    GtkWidget	 *button1;
    GtkWidget	 *button2;
    GtkWidget	 *checkbutton1;
    GtkWidget    *pixmap1;

    obj_synwin.window1 = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_object_set_data (GTK_OBJECT (obj_synwin.window1), "obj_synwin.window1", obj_synwin.window1);
    gtk_window_set_title (GTK_WINDOW (obj_synwin.window1), ("Create Synonym"));
    gtk_window_set_position (GTK_WINDOW (obj_synwin.window1), GTK_WIN_POS_CENTER);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_widget_set_usize (vbox1, 399, 406);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (obj_synwin.window1), vbox1);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_box_pack_start (GTK_BOX (vbox1), fixed1, TRUE, TRUE, 0);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "frame1", frame1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_fixed_put (GTK_FIXED (fixed1), frame1, 16, 16);
    gtk_widget_set_uposition (frame1, 16, 16);
    gtk_widget_set_usize (frame1, 368, 100);

    fixed2 = gtk_fixed_new ();
    gtk_widget_ref (fixed2);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "fixed2", fixed2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed2);
    gtk_container_add (GTK_CONTAINER (frame1), fixed2);

    obj_synwin.entry1 = gtk_entry_new ();
    gtk_widget_ref (obj_synwin.entry1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.entry1", obj_synwin.entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.entry1);
    gtk_fixed_put (GTK_FIXED (fixed2), obj_synwin.entry1, 80, 8);
    gtk_widget_set_uposition (obj_synwin.entry1, 80, 8);
    gtk_widget_set_usize (obj_synwin.entry1, 158, 22);

    obj_synwin.combo1 = gtk_combo_new ();
    gtk_widget_ref (obj_synwin.combo1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.combo1", obj_synwin.combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.combo1);
    gtk_fixed_put (GTK_FIXED (fixed2), obj_synwin.combo1, 80, 40);
    gtk_widget_set_uposition (obj_synwin.combo1, 80, 40);
    gtk_widget_set_usize (obj_synwin.combo1, 175, 22);

    obj_synwin.combo_entry1 = GTK_COMBO (obj_synwin.combo1)->entry;
    gtk_widget_ref (obj_synwin.combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.combo_entry1", obj_synwin.combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.combo_entry1);

    pixmap1 = create_misc_pixmap (ADVANTIO_PIXMAP);
    gtk_widget_ref (pixmap1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "pixmap1", pixmap1,
		      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (pixmap1);
    gtk_fixed_put (GTK_FIXED (fixed2), pixmap1, 270, 8);

    checkbutton1 = gtk_check_button_new_with_label (("PUBLIC"));
    gtk_widget_ref (checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "checkbutton1", checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed2), checkbutton1, 120, 70);
    gtk_widget_set_uposition (checkbutton1, 120, 70);
    gtk_widget_set_usize (checkbutton1, 88, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton1), TRUE);

    label2 = gtk_label_new (("Schema:"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed2), label2, 8, 40);
    gtk_widget_set_uposition (label2, 8, 40);
    /* gtk_widget_set_usize (label2, 112, 24); */

    label1 = gtk_label_new (("Name:"));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_fixed_put (GTK_FIXED (fixed2), label1, 8, 8);
    gtk_widget_set_uposition (label1, 8, 8);
    /* gtk_widget_set_usize (label1, 112, 24); */

    frame3 = gtk_frame_new (NULL);
    gtk_widget_ref (frame3);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "frame3", frame3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame3);
    gtk_fixed_put (GTK_FIXED (fixed1), frame3, 16, 216);
    gtk_widget_set_uposition (frame3, 16, 216);
    gtk_widget_set_usize (frame3, 368, 80);

    fixed4 = gtk_fixed_new ();
    gtk_widget_ref (fixed4);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "fixed4", fixed4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed4);
    gtk_container_add (GTK_CONTAINER (frame3), fixed4);

    obj_synwin.combo3 = gtk_combo_new ();
    gtk_widget_ref (obj_synwin.combo3);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.combo3", obj_synwin.combo3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.combo3);
    gtk_fixed_put (GTK_FIXED (fixed4), obj_synwin.combo3, 120, 16);
    gtk_widget_set_uposition (obj_synwin.combo3, 120, 16);
    gtk_widget_set_usize (obj_synwin.combo3, 175, 22);

    obj_synwin.combo_entry3 = GTK_COMBO (obj_synwin.combo3)->entry;
    gtk_widget_ref (obj_synwin.combo_entry3);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.combo_entry3", obj_synwin.combo_entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.combo_entry3);

    obj_synwin.combo4 = gtk_combo_new ();
    gtk_widget_ref (obj_synwin.combo4);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.combo4", obj_synwin.combo4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.combo4);
    gtk_fixed_put (GTK_FIXED (fixed4), obj_synwin.combo4, 120, 48);
    gtk_widget_set_uposition (obj_synwin.combo4, 120, 48);
    gtk_widget_set_usize (obj_synwin.combo4, 175, 22);

    obj_synwin.combo_entry4 = GTK_COMBO (obj_synwin.combo4)->entry;
    gtk_widget_ref (obj_synwin.combo_entry4);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.combo_entry4", obj_synwin.combo_entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.combo_entry4);

    label4 = gtk_label_new (("Target Schema:"));
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_fixed_put (GTK_FIXED (fixed4), label4, 8, 16);
    gtk_widget_set_uposition (label4, 8, 16);
    /* gtk_widget_set_usize (label4, 98, 24); */

    label5 = gtk_label_new (("Target Object:"));
    gtk_widget_ref (label5);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "label5", label5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label5);
    gtk_fixed_put (GTK_FIXED (fixed4), label5, 8, 48);
    gtk_widget_set_uposition (label5, 8, 48);
    /* gtk_widget_set_usize (label5, 96, 24); */

    frame2 = gtk_frame_new (NULL);
    gtk_widget_ref (frame2);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "frame2", frame2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame2);
    gtk_fixed_put (GTK_FIXED (fixed1), frame2, 16, 136);
    gtk_widget_set_uposition (frame2, 16, 136);
    gtk_widget_set_usize (frame2, 368, 72);

    fixed3 = gtk_fixed_new ();
    gtk_widget_ref (fixed3);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "fixed3", fixed3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed3);
    gtk_container_add (GTK_CONTAINER (frame2), fixed3);

    obj_synwin.combo2 = gtk_combo_new ();
    gtk_widget_ref (obj_synwin.combo2);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.combo2", obj_synwin.combo2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.combo2);
    gtk_fixed_put (GTK_FIXED (fixed3), obj_synwin.combo2, 120, 40);
    gtk_widget_set_uposition (obj_synwin.combo2, 120, 40);
    gtk_widget_set_usize (obj_synwin.combo2, 175, 22);

    obj_synwin.combo_entry2 = GTK_COMBO (obj_synwin.combo2)->entry;
    gtk_widget_ref (obj_synwin.combo_entry2);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "obj_synwin.combo_entry2", obj_synwin.combo_entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_synwin.combo_entry2);

    label3 = gtk_label_new (("Object Type:"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed3), label3, 40, 40);
    gtk_widget_set_uposition (label3, 40, 40);
    /* gtk_widget_set_usize (label3, 74, 24); */

    radiobutton1 = gtk_radio_button_new_with_label (g1_group, ("Current Database"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
    gtk_widget_ref (radiobutton1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "radiobutton1", radiobutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton1);
    gtk_fixed_put (GTK_FIXED (fixed3), radiobutton1, 48, 8);
    gtk_widget_set_uposition (radiobutton1, 48, 8);
    gtk_widget_set_usize (radiobutton1, 128, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton1), TRUE);

    radiobutton2 = gtk_radio_button_new_with_label (g1_group, ("DB Linked"));
    g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
    gtk_widget_ref (radiobutton2);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "radiobutton2", radiobutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton2);
    gtk_fixed_put (GTK_FIXED (fixed3), radiobutton2, 208, 8);
    gtk_widget_set_uposition (radiobutton2, 208, 8);
    gtk_widget_set_usize (radiobutton2, 93, 24);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_synwin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    obj_synwin.text1 = add_sql_window(vbox1);

    gtk_signal_connect (GTK_OBJECT (obj_synwin.entry1), "changed",
			GTK_SIGNAL_FUNC (on_sname_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_synwin.combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_sschema_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_synwin.combo_entry3), "changed",
			GTK_SIGNAL_FUNC (on_tschema_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_synwin.combo_entry4), "changed",
			GTK_SIGNAL_FUNC (on_tobject_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_synwin.combo_entry2), "changed",
			GTK_SIGNAL_FUNC (on_otype_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_cdb_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_rdb_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_syncancel_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_synok_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_syn_checkbutton1_clicked),
			NULL);

    synsql.sname=NULL;
    synsql.sschema=NULL;
    synsql.tname=NULL;
    synsql.tschema=NULL;
    synsql.dblink=NULL;
    synsql.toggle=1;
    synsql.schema_items=NULL;
    synsql.ispublic=1;

    /* Dupe call */
    generic_populate_schemas(obj_synwin.combo1, obj_synwin.combo_entry1, obj_cinfo.login->dbid);
    generic_populate_schemas(obj_synwin.combo3, obj_synwin.combo_entry3, obj_cinfo.login->dbid);
    
    syn_loadobjtype(1);

    return obj_synwin.window1;
}

