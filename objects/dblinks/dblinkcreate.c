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
#include "objects/tables/tablefuncs.h"
#include "dblinkfuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct dblsql_str{
    char	 *dblname;
    int		 ispublic;
    char	 *dbluser;
    char	 *dblpassword;
    char	 *dblservice;
}dblsql_struct;

typedef struct dbl_str{
  GtkWidget *window1;
  GtkWidget *entry1;
  GtkWidget *checkbutton1;
  GtkWidget *entry2;
  GtkWidget *entry3;
  GtkWidget *entry4;
  GtkWidget *text1;
}obj_dbl_struct;

obj_dbl_struct obj_dblwin;
static dblsql_struct dblsql;


static void
build_dbl_sql(void)
{
    GString *query = g_string_new("CREATE ");
  
    if(dblsql.ispublic)
	g_string_append(query, "PUBLIC DATABASE LINK ");
    else
	g_string_append(query, "DATABASE LINK ");
  
    if(dblsql.dblname!=NULL)
	g_string_append(query, dblsql.dblname);
  
    if(dblsql.dbluser!=NULL)
	g_string_sprintfa(query, " CONNECT TO %s ",dblsql.dbluser);

    if(dblsql.dblpassword!=NULL)
	g_string_sprintfa(query, "IDENTIFIED BY %s ",dblsql.dblpassword);
  
    if(dblsql.dblservice!=NULL)
	g_string_sprintfa(query, "USING '%s'",dblsql.dblservice);
  
    clear_text(obj_dblwin.text1);
    gtk_text_insert (GTK_TEXT (obj_dblwin.text1), NULL, NULL,NULL, query->str, query->len);
    
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(obj_dblwin.text1));
	
    g_string_free(query, 1);
}


static void
on_linkname_changed(GtkEditable     *editable,
		    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dblsql.dblname=entry;
    }else{
	dblsql.dblname=NULL;
    }

    build_dbl_sql();
}

static void
on_dblpublic_clicked(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	dblsql.ispublic=1;
    else
	dblsql.ispublic=0;
    
    build_dbl_sql();
}

static void
on_password_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dblsql.dblpassword=entry;
    }else{
	dblsql.dblpassword=NULL;
    }
    
    build_dbl_sql();
}


static void
on_service_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dblsql.dblservice=entry;
    }else{
	dblsql.dblservice=NULL;
    }

    build_dbl_sql();
}


static void
on_asuser_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	dblsql.dbluser=entry;
    }else{
	dblsql.dbluser=NULL;
    }
    
    build_dbl_sql();

}


static void
on_linkcancel_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(obj_dblwin.window1);
} 

static void
on_linkcreate_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (obj_dblwin.text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (obj_dblwin.text1)));

    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("Database Link has been created");
    }
    
    g_free(cp);
    gtk_widget_destroy(obj_dblwin.window1);
}

GtkWidget*
obj_create_dblwin (void)
{
    GtkWidget *vbox1;
    GtkWidget *frame1;
    GtkWidget *fixed2;
    GtkWidget *label1;
    GtkWidget *frame2;
    GtkWidget *fixed1;
    GtkWidget *label3;
    GtkWidget *label4;
    GtkWidget *label2;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *pixmap1;

    obj_dblwin.window1 = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_object_set_data (GTK_OBJECT (obj_dblwin.window1), "obj_dblwin.window1", obj_dblwin.window1);
    gtk_window_set_title (GTK_WINDOW (obj_dblwin.window1), ("Create DB Link"));
    gtk_window_set_position (GTK_WINDOW (obj_dblwin.window1), GTK_WIN_POS_CENTER);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_widget_set_usize (vbox1, 365, 380);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (obj_dblwin.window1), vbox1);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "frame1", frame1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

    fixed2 = gtk_fixed_new ();
    gtk_widget_ref (fixed2);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "fixed2", fixed2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed2);
    gtk_container_add (GTK_CONTAINER (frame1), fixed2);

    pixmap1 = create_misc_pixmap (ADVANTIO_PIXMAP);
    gtk_widget_ref (pixmap1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "pixmap1", pixmap1,
		      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (pixmap1);
    gtk_fixed_put (GTK_FIXED (fixed2), pixmap1, 290, 2);

    obj_dblwin.entry1 = gtk_entry_new ();
    gtk_widget_ref (obj_dblwin.entry1);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "obj_dblwin.entry1", obj_dblwin.entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_dblwin.entry1);
    gtk_fixed_put (GTK_FIXED (fixed2), obj_dblwin.entry1, 90, 8);
    gtk_widget_set_uposition (obj_dblwin.entry1, 90, 8);
    gtk_widget_set_usize (obj_dblwin.entry1, 158, 22);

    label1 = gtk_label_new (("Link Name:"));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_fixed_put (GTK_FIXED (fixed2), label1, 8, 8);
    gtk_widget_set_uposition (label1, 8, 8);
    gtk_widget_set_usize (label1, 80, 24);

    obj_dblwin.checkbutton1 = gtk_check_button_new_with_label (("Public"));
    gtk_widget_ref (obj_dblwin.checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "obj_dblwin.checkbutton1", obj_dblwin.checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_dblwin.checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed2), obj_dblwin.checkbutton1, 152, 32);
    gtk_widget_set_uposition (obj_dblwin.checkbutton1, 152, 32);
    gtk_widget_set_usize (obj_dblwin.checkbutton1, 99, 24);

    frame2 = gtk_frame_new (NULL);
    gtk_widget_ref (frame2);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "frame2", frame2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame2);
    gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (frame2), fixed1);

    obj_dblwin.entry2 = gtk_entry_new ();
    gtk_widget_ref (obj_dblwin.entry2);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "obj_dblwin.entry2", obj_dblwin.entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_dblwin.entry2);
    gtk_fixed_put (GTK_FIXED (fixed1), obj_dblwin.entry2, 160, 72);
    gtk_widget_set_uposition (obj_dblwin.entry2, 160, 72);
    gtk_widget_set_usize (obj_dblwin.entry2, 158, 22);

    obj_dblwin.entry3 = gtk_entry_new ();
    gtk_widget_ref (obj_dblwin.entry3);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "obj_dblwin.entry3", obj_dblwin.entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_dblwin.entry3);
    gtk_fixed_put (GTK_FIXED (fixed1), obj_dblwin.entry3, 160, 8);
    gtk_widget_set_uposition (obj_dblwin.entry3, 160, 8);
    gtk_widget_set_usize (obj_dblwin.entry3, 158, 22);

    obj_dblwin.entry4 = gtk_entry_new ();
    gtk_widget_ref (obj_dblwin.entry4);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "obj_dblwin.entry4", obj_dblwin.entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_dblwin.entry4);
    gtk_fixed_put (GTK_FIXED (fixed1), obj_dblwin.entry4, 160, 40);
    gtk_widget_set_uposition (obj_dblwin.entry4, 160, 40);
    gtk_widget_set_usize (obj_dblwin.entry4, 158, 22);

    label4 = gtk_label_new (("Password:"));
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_fixed_put (GTK_FIXED (fixed1), label4, 48, 40);
    gtk_widget_set_uposition (label4, 48, 40);
    gtk_widget_set_usize (label4, 104, 24);

    label3 = gtk_label_new (("Connect As User:"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed1), label3, 48, 8);
    gtk_widget_set_uposition (label3, 48, 8);
    gtk_widget_set_usize (label3, 104, 24);

    label2 = gtk_label_new (("Connect Using Service:"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed1), label2, 16, 72);
    gtk_widget_set_uposition (label2, 16, 72);
    gtk_widget_set_usize (label2, 138, 24);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_dblwin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    obj_dblwin.text1 = add_sql_window(vbox1);

    gtk_signal_connect (GTK_OBJECT (obj_dblwin.entry1), "changed",
			GTK_SIGNAL_FUNC (on_linkname_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_dblwin.entry2), "changed",
			GTK_SIGNAL_FUNC (on_service_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_dblwin.entry3), "changed",
			GTK_SIGNAL_FUNC (on_asuser_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_dblwin.entry4), "changed",
			GTK_SIGNAL_FUNC (on_password_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_linkcancel_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_linkcreate_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_dblwin.checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_dblpublic_clicked),
			NULL);
		      
    return obj_dblwin.window1;
}

