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
#include "typefuncs.h"

typedef struct typesql_str{
    char	 *vname;
    char	 *vschema;
    char	 *vtext;
}typesql_struct;

static typesql_struct typesql;

typedef struct type_str{
  GtkWidget *window1;
  GtkWidget *entry1;
  GtkWidget *combo1;
  GtkWidget *combo_entry1;
  GtkWidget *text2;
  GtkWidget *label1;
  GtkWidget *text1;
}obj_type_struct;

static obj_type_struct obj_typewin;

static void 
build_type_sql(void)
{
    GString *query = g_string_new("CREATE TYPE ");
  
    if(typesql.vschema!=NULL)
	g_string_sprintfa(query, "%s.",typesql.vschema);
    
    if(typesql.vname!=NULL)
	g_string_append(query, typesql.vname);
    
    g_string_append(query, " AS ");
    
    if(typesql.vtext!=NULL)
	g_string_append(query, typesql.vtext);
    
    clear_text(obj_typewin.text1);
    gtk_text_insert (GTK_TEXT (obj_typewin.text1), NULL, NULL,NULL, query->str, query->len);
    
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(obj_typewin.text1));
    
    g_string_free(query, 1);
}

static void
on_type_vname_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	typesql.vname=entry;
    }else{
	typesql.vname=NULL;
    }
    
    build_type_sql();
}


static void
on_type_vschema_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	typesql.vschema=entry;
    }else{
	typesql.vschema=NULL;
    }

    build_type_sql();
}


static void
on_type_typetext_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *cp;
    
    cp = gtk_editable_get_chars(GTK_EDITABLE (editable),(gint) 0,
				(gint) gtk_text_get_length (GTK_TEXT (editable)));
    
    if(strlen(cp)>0){
	typesql.vtext=cp;
    }else{
	typesql.vtext=NULL;
    }
    
    build_type_sql();
    typesql.vtext = 0;
    g_free(cp);
}


static void
on_type_typecancel_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(obj_typewin.window1);
}


static void
on_type_typeok_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (obj_typewin.text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (obj_typewin.text1)));
    
    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("type has been created");
    }
    
    gtk_widget_destroy(obj_typewin.window1);
    g_free(cp);
}

GtkWidget*
obj_create_typewin (void)
{
    GtkWidget	 *vbox1;
    GtkWidget	 *fixed1;
    GtkWidget	 *label2;
    GtkWidget	 *label3;
    GtkWidget	 *scrolledwindow2;
    GtkWidget	 *hbuttonbox1;
    GtkWidget	 *button1;
    GtkWidget	 *button2;
    GtkWidget    *pixmap1;

    obj_typewin.window1 = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_object_set_data (GTK_OBJECT (obj_typewin.window1), "obj_typewin.window1", obj_typewin.window1);
    gtk_window_set_title (GTK_WINDOW (obj_typewin.window1), ("Create Type"));
    gtk_window_set_position (GTK_WINDOW (obj_typewin.window1), GTK_WIN_POS_CENTER);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_widget_set_usize (vbox1, 414, 484);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (obj_typewin.window1), vbox1);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_box_pack_start (GTK_BOX (vbox1), fixed1, TRUE, TRUE, 0);

    pixmap1 = create_misc_pixmap (ADVANTIO_PIXMAP);
    gtk_widget_ref (pixmap1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "pixmap1", pixmap1,
		      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (pixmap1);
    gtk_fixed_put (GTK_FIXED (fixed1), pixmap1, 325, 12);

    obj_typewin.entry1 = gtk_entry_new ();
    gtk_widget_ref (obj_typewin.entry1);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "obj_typewin.entry1", obj_typewin.entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_typewin.entry1);
    gtk_fixed_put (GTK_FIXED (fixed1), obj_typewin.entry1, 100, 24);
    gtk_widget_set_uposition (obj_typewin.entry1, 100, 24);
    gtk_widget_set_usize (obj_typewin.entry1, 158, 22);

    label2 = gtk_label_new (("Type Name:"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed1), label2, 25, 24);
    gtk_widget_set_uposition (label2, 25, 24);
    /* gtk_widget_set_usize (label2, 72, 24); */

    label3 = gtk_label_new (("Schema:"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed1), label3, 25, 56);
    gtk_widget_set_uposition (label3, 25, 56);
    /* gtk_widget_set_usize (label3, 72, 24); */

    obj_typewin.combo1 = gtk_combo_new ();
    gtk_widget_ref (obj_typewin.combo1);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "obj_typewin.combo1", obj_typewin.combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_typewin.combo1);
    gtk_fixed_put (GTK_FIXED (fixed1), obj_typewin.combo1, 100, 56);
    gtk_widget_set_uposition (obj_typewin.combo1, 100, 56);
    gtk_widget_set_usize (obj_typewin.combo1, 175, 22);

    obj_typewin.combo_entry1 = GTK_COMBO (obj_typewin.combo1)->entry;
    gtk_widget_ref (obj_typewin.combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "obj_typewin.combo_entry1", obj_typewin.combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_typewin.combo_entry1);

    scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow2);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "scrolledwindow2", scrolledwindow2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow2);
    gtk_widget_set_usize (scrolledwindow2, 414, 260);
    gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow2, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    obj_typewin.text2 = gtk_text_new (NULL, NULL);
    gtk_widget_ref (obj_typewin.text2);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "obj_typewin.text2", obj_typewin.text2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_typewin.text2);
    gtk_text_set_word_wrap(GTK_TEXT (obj_typewin.text2),TRUE);
    gtk_container_add (GTK_CONTAINER (scrolledwindow2), obj_typewin.text2);
    gtk_text_set_editable (GTK_TEXT (obj_typewin.text2), TRUE);

    obj_typewin.label1 = gtk_label_new (("   "));
    gtk_widget_ref (obj_typewin.label1);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "obj_typewin.label1", obj_typewin.label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_typewin.label1);
    gtk_box_pack_start (GTK_BOX (vbox1), obj_typewin.label1, FALSE, FALSE, 0);
    gtk_label_set_justify (GTK_LABEL (obj_typewin.label1), GTK_JUSTIFY_LEFT);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_typewin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    obj_typewin.text1 = add_sql_window(vbox1);

    gtk_signal_connect (GTK_OBJECT (obj_typewin.entry1), "changed",
			GTK_SIGNAL_FUNC (on_type_vname_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_typewin.combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_type_vschema_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_typewin.text2), "changed",
			GTK_SIGNAL_FUNC (on_type_typetext_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_type_typecancel_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_type_typeok_clicked),
			NULL);

    generic_populate_schemas(obj_typewin.combo1,obj_typewin.combo_entry1, obj_cinfo.login->dbid);

    return obj_typewin.window1;
}

