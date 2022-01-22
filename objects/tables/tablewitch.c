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
#include "tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct tablesql_str{
    char *name;
    char *schema;
    char *tablespace;
    char *collist;
}tablesql_struct;

GtkWidget *window3;
GtkWidget *window2;
GtkWidget *window1;
GtkWidget *entry1;
GtkWidget *combo_entry1;
GtkWidget *combo_entry2;
GtkWidget *clist2;
GtkWidget *entry2;
GtkWidget *entry3;
GtkWidget *entry4;
GtkWidget *combo_entry3;
GtkWidget *button3;
GtkWidget *text1;
GtkWidget *combo1;
GtkWidget *combo2;
GtkWidget *combo3;
GtkWidget *label13;
GtkWidget *label14;
GtkWidget *label15;
GtkWidget *label11;
GtkWidget *label12;

static tablesql_struct tableinfo;

static int crow=-1;
static int numcol=0;

static void
obj_twredolist(void)
{
    int a=0;
    char *buf[4];
    GString *query = g_string_new("CREATE TABLE ");

    g_string_sprintfa(query, "%s.%s (",tableinfo.schema,tableinfo.name);

    while(a!=numcol) {
	gtk_clist_get_text(GTK_CLIST(clist2), a, 0, &buf[0]);
	gtk_clist_get_text(GTK_CLIST(clist2), a, 1, &buf[1]);
	gtk_clist_get_text(GTK_CLIST(clist2), a, 2, &buf[2]);
	gtk_clist_get_text(GTK_CLIST(clist2), a, 3, &buf[3]);
  
	if(a!=0)
	    g_string_append(query, ",");
    
	g_string_sprintfa(query, "%s %s",buf[0],buf[1]);
  
	if(strncmp(buf[2]," ",1)!=0){
	    g_string_sprintfa(query, "(%s)",buf[2]);
	}
  
	if(strncmp(buf[3]," ",1)!=0){
	    g_string_sprintfa(query, " DEFAULT '%s' ",buf[3]);
	}
  
	a++;
    }

    g_string_sprintfa(query, ") %s TABLESPACE %s",storage,tableinfo.tablespace);

    clear_text(text1);
    gtk_text_insert (GTK_TEXT (text1), NULL, NULL,NULL, query->str,query->len);
    
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(text1));
    
    g_string_free(query, 1);
}

static void
on_twclist2_click_column(
    GtkCList	 *clist,
    gint	 row,
    gint	 column,
    GdkEvent	 *event,
    gpointer	 user_data)
    
{
    char	 *buf[4];

    crow=row;

    gtk_clist_get_text(GTK_CLIST(clist), row, 0, &buf[0]);
    gtk_clist_get_text(GTK_CLIST(clist), row, 1, &buf[1]);
    gtk_clist_get_text(GTK_CLIST(clist), row, 2, &buf[2]);
    gtk_clist_get_text(GTK_CLIST(clist), row, 3, &buf[3]);

    gtk_entry_set_text(GTK_ENTRY(entry2),buf[0]);
    gtk_entry_set_text(GTK_ENTRY(combo_entry3),buf[1]);

    if(strncmp(buf[2]," ",1)!=0)
	gtk_entry_set_text(GTK_ENTRY(entry3),buf[2]);
    else
	gtk_entry_set_text(GTK_ENTRY(entry3),"");

    if(strncmp(buf[2]," ",1)!=0)
	gtk_entry_set_text(GTK_ENTRY(entry4),buf[3]);
    else
	gtk_entry_set_text(GTK_ENTRY(entry4),"");
}

static void
on_twbutton1_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(window1);
}

static void
on_twbutton2_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *datatypes[8] = {
	"varchar2", "number", "date", "char", "long", "raw", "blob", "clob" } ;
    GList	 *glist=NULL;
    int		 i;
  
    tableinfo.name=gtk_entry_get_text(GTK_ENTRY(entry1));
    if(strlen(tableinfo.name)<1)
	return;
    tableinfo.schema=gtk_entry_get_text(GTK_ENTRY(combo_entry1));
    if(strlen(tableinfo.schema)<1)
	return;
    tableinfo.tablespace=gtk_entry_get_text(GTK_ENTRY(combo_entry2));
    if(strlen(tableinfo.tablespace)<1)
	return;

    if(window3!=NULL)
        return;
	
    window3=obj_create_twwin();
    
    for (i = 0; i < sizeof(datatypes)/sizeof(datatypes[0]); i++)
	glist = g_list_append(glist, datatypes[i]);
    
    gtk_combo_set_popdown_strings( GTK_COMBO(combo3), glist) ;
    
    gtk_widget_show(window3);
}

static void
on_twbutton4_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *colinfo[4];

    colinfo[0]=gtk_entry_get_text(GTK_ENTRY(entry2));
    if(strlen(colinfo[0])<1)
	return;
    colinfo[1]=gtk_entry_get_text(GTK_ENTRY(combo_entry3));
    if(strlen(colinfo[1])<1)
	return;
    colinfo[2]=gtk_entry_get_text(GTK_ENTRY(entry3));
    if(strlen(colinfo[2])<1)
	colinfo[2]=" ";
    colinfo[3]=gtk_entry_get_text(GTK_ENTRY(entry4));
    if(strlen(colinfo[3])<1)
	colinfo[3]=" ";

    if(strstr(colinfo[1],"char")!=0){
        if(!strncmp(colinfo[2]," ", 1))
	    return;
    }
    
    gtk_clist_append( (GtkCList *) clist2, colinfo);
    numcol++;
    obj_twredolist();

}

static void
on_twbutton3_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    if(crow != -1){
	gtk_clist_remove((GtkCList *)clist2,crow);
	crow=-1;
	numcol--;
	obj_twredolist();
    }
}

static void
on_twbutton5_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(window3);
}

static void
on_twbutton6_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (text1)));
    
    
    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("Table Created");
	gtk_widget_destroy(window1);
        gtk_widget_destroy(window3);
    }
    
    g_free(cp);
}

static void
on_twbutton7_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(window3);
}

static void
on_twbutton8_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    window2=obj_create_twstoragewin();
    gtk_widget_show(window2);
}

static void
on_storage_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    obj_twredolist();
    gtk_widget_destroy(window2);
}


static void
on_combo_twentry3_changed(
    GtkEditable	 *editable,
    gpointer	 user_data)
{
    char	 *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strncmp(entry,"varchar2",7)==0 || strncmp(entry,"char",4)==0 || strncmp(entry,"number",6)==0){
	if(strncmp(entry,"number",6)==0)
	    gtk_widget_show(label15);
	else
	    gtk_widget_hide(label15);
	gtk_widget_show(label11);
	gtk_widget_show(label12);
	gtk_widget_show(entry3);
	gtk_widget_show(entry4);
    }else{
	gtk_widget_hide(label15);
	gtk_widget_hide(label11);
	gtk_widget_hide(label12);
	gtk_entry_set_text(GTK_ENTRY(entry3),"");
	gtk_widget_hide(entry3);
	gtk_entry_set_text(GTK_ENTRY(entry4),"");
	gtk_widget_hide(entry4);
    }
}

static void
destroy_win3()
{
    window3=NULL;
}

GtkWidget*
get_twwinmain_widget (void)
{
    GtkWidget *vbox1;
    GtkWidget *pixmap1;
    GtkWidget *fixed1;
    GtkWidget *label2;
    GtkWidget *label3;
    GtkWidget *label4;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    GtkWidget *button2;

    window3 = NULL;
    window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window1), "window1", 
			 window1);
    gtk_window_set_title (GTK_WINDOW (window1), ("Orasoft Table Witch"));
    gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (vbox1, 400, 235);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window1), vbox1);

    pixmap1 = create_misc_pixmap (TABLEWITCH_PIXMAP);
    gtk_widget_ref (pixmap1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "pixmap1", pixmap1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (pixmap1);
    gtk_box_pack_start (GTK_BOX (vbox1), pixmap1, FALSE, FALSE, 0);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_box_pack_start (GTK_BOX (vbox1), fixed1, TRUE, TRUE, 0);

    entry1 = gtk_entry_new ();
    gtk_widget_ref (entry1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry1", entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry1);
    gtk_fixed_put (GTK_FIXED (fixed1), entry1, 120, 32);
    gtk_widget_set_uposition (entry1, 120, 32);
    gtk_widget_set_usize (entry1, 158, 22);

    label2 = gtk_label_new (("Table Name:"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed1), label2, 24, 32);
    gtk_widget_set_uposition (label2, 24, 32);
    /* gtk_widget_set_usize (label2, 96, 24); */

    label3 = gtk_label_new (("Schema:"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed1), label3, 24, 64);
    gtk_widget_set_uposition (label3, 24, 64);
    /* gtk_widget_set_usize (label3, 96, 24); */

    combo1 = gtk_combo_new ();
    gtk_widget_ref (combo1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo1", combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo1);
    gtk_fixed_put (GTK_FIXED (fixed1), combo1, 120, 64);
    gtk_widget_set_uposition (combo1, 120, 64);
    /*gtk_widget_set_usize (combo1, 175, 22);*/

    combo_entry1 = GTK_COMBO (combo1)->entry;
    gtk_widget_ref (combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry1", combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry1);

    label4 = gtk_label_new (("Tablespace:"));
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_fixed_put (GTK_FIXED (fixed1), label4, 24, 96);
    gtk_widget_set_uposition (label4, 24, 96);
    /* gtk_widget_set_usize (label4, 96, 24); */

    combo2 = gtk_combo_new ();
    gtk_widget_ref (combo2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo2", combo2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo2);
    gtk_fixed_put (GTK_FIXED (fixed1), combo2, 120, 96);
    gtk_widget_set_uposition (combo2, 120, 96);
    /*gtk_widget_set_usize (combo2, 175, 22);*/

    combo_entry2 = GTK_COMBO (combo2)->entry;
    gtk_widget_ref (combo_entry2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry2", combo_entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry2);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Next >>");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    obj_progwin.window4 = obj_create_progressbar();
    gtk_widget_show (obj_progwin.window4);
    gtk_widget_realize (obj_progwin.label1);
   
    gtk_label_set_text(GTK_LABEL (obj_progwin.label1),"Loading Schemas, Please Wait");
   
    gtk_label_set_text(GTK_LABEL (obj_progwin.label1),"Loading Tablespaces, Please Wait");
    gtk_progress_bar_update( GTK_PROGRESS_BAR(obj_progwin.progressbar1),.50);

    generic_populate_schemas(combo1,combo_entry1, obj_cinfo.login->dbid);
    generic_populate_tablespace(combo2,combo_entry2, obj_cinfo.login->dbid);
   
    gtk_label_set_text(GTK_LABEL (obj_progwin.label1),"Loading Columns, Please Wait");
    gtk_progress_bar_update( GTK_PROGRESS_BAR(obj_progwin.progressbar1),.99);
    gtk_widget_destroy(obj_progwin.window4);

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_twbutton1_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_twbutton2_clicked),
			NULL);

    return window1;
}


GtkWidget*
obj_create_twwin (void)
{
    GtkWidget *vbox2;
    GtkWidget *hbox2;
    GtkWidget *scrolledwindow3;
    GtkWidget *label7;
    GtkWidget *label8;
    GtkWidget *fixed3;
    GtkWidget *button4;
    GtkWidget *label9;
    GtkWidget *label10;
    GtkWidget *button5;
    GtkWidget *button6;
    GtkWidget *button7;
    GtkWidget *button8;
    GtkWidget *hseparator1;
    GtkWidget *pixmap1;
    char      *buf;

    buf=malloc(strlen(tableinfo.name)+strlen(tableinfo.schema)+30);
    sprintf(buf,"Creating Table %s.%s",tableinfo.name,tableinfo.schema);

    window3 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window3), "window3", window3);
    gtk_window_set_title (GTK_WINDOW (window3), (buf));
    gtk_window_set_position (GTK_WINDOW (window3), GTK_WIN_POS_CENTER);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox2);
    gtk_object_set_data_full (GTK_OBJECT (window3), "vbox2", vbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (vbox2,696, 375);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (window3), vbox2);

    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_widget_ref (hbox2);
    gtk_object_set_data_full (GTK_OBJECT (window3), "hbox2", hbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox2);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox2, TRUE, TRUE, 0);

    scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow3);
    gtk_object_set_data_full (GTK_OBJECT (window3), "scrolledwindow3", scrolledwindow3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (scrolledwindow3, 250, 250);
    gtk_widget_show (scrolledwindow3);
    gtk_box_pack_start (GTK_BOX (hbox2), scrolledwindow3, TRUE, TRUE, 0);

    clist2 = gtk_clist_new (4);
    gtk_widget_ref (clist2);
    gtk_object_set_data_full (GTK_OBJECT (window3), "clist2", clist2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist2);
    gtk_container_add (GTK_CONTAINER (scrolledwindow3), clist2);
    gtk_clist_set_column_width (GTK_CLIST (clist2), 0, 80);
    gtk_clist_set_column_width (GTK_CLIST (clist2), 1, 40);
    gtk_clist_set_column_width (GTK_CLIST (clist2), 2, 20);
    gtk_clist_set_column_width (GTK_CLIST (clist2), 3, 80);
    gtk_clist_column_titles_show (GTK_CLIST (clist2));

    label7 = gtk_label_new (("ColName"));
    gtk_widget_ref (label7);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label7", label7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label7);
    gtk_clist_set_column_widget (GTK_CLIST (clist2), 0, label7);

    label8 = gtk_label_new (("Type"));
    gtk_widget_ref (label8);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label8", label8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label8);
    gtk_clist_set_column_widget (GTK_CLIST (clist2), 1, label8);

    label13 = gtk_label_new (("Len"));
    gtk_widget_ref (label13);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label13", label13,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label13);
    gtk_clist_set_column_widget (GTK_CLIST (clist2), 2, label13);

    label14 = gtk_label_new (("Default"));
    gtk_widget_ref (label14);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label14", label14,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label14);
    gtk_clist_set_column_widget (GTK_CLIST (clist2), 3, label14);

    fixed3 = gtk_fixed_new ();
    gtk_widget_ref (fixed3);
    gtk_object_set_data_full (GTK_OBJECT (window3), "fixed3", fixed3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed3);
    gtk_box_pack_start (GTK_BOX (hbox2), fixed3, TRUE, TRUE, 0);

    entry2 = gtk_entry_new ();
    gtk_widget_ref (entry2);
    gtk_object_set_data_full (GTK_OBJECT (window3), "entry2", entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry2);
    gtk_fixed_put (GTK_FIXED (fixed3), entry2, 120, 14);
    gtk_widget_set_uposition (entry2, 120, 14);
    gtk_widget_set_usize (entry2, 158, 22);

    pixmap1 = create_misc_pixmap (ADVANTIO_PIXMAP);
    gtk_widget_ref (pixmap1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "pixmap1", pixmap1,
		      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (pixmap1);
    gtk_fixed_put (GTK_FIXED (fixed3), pixmap1, 330, 14);

    entry3 = gtk_entry_new ();
    gtk_widget_ref (entry3);
    gtk_object_set_data_full (GTK_OBJECT (window3), "entry3", entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry3);
    gtk_fixed_put (GTK_FIXED (fixed3), entry3, 120, 78);
    gtk_widget_set_uposition (entry3, 120, 78);
    gtk_widget_set_usize (entry3, 158, 22);

    entry4 = gtk_entry_new ();
    gtk_widget_ref (entry4);
    gtk_object_set_data_full (GTK_OBJECT (window3), "entry4", entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry4);
    gtk_fixed_put (GTK_FIXED (fixed3), entry4, 120, 110);
    gtk_widget_set_uposition (entry4, 120, 110);
    gtk_widget_set_usize (entry4, 158, 22);

    combo3 = gtk_combo_new ();
    gtk_widget_ref (combo3);
    gtk_object_set_data_full (GTK_OBJECT (window3), "combo3", combo3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo3);
    gtk_fixed_put (GTK_FIXED (fixed3), combo3, 120, 46);
    gtk_widget_set_uposition (combo3, 120, 46);
    gtk_widget_set_usize (combo3, 175, 22);

    combo_entry3 = GTK_COMBO (combo3)->entry;
    gtk_widget_ref (combo_entry3);
    gtk_object_set_data_full (GTK_OBJECT (window3), "combo_entry3", combo_entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry3);

    button3 = pixmap_button (create_button_pixmap(BUTTON_PIX_REMOVE),"Remove");
    gtk_widget_ref (button3);
    gtk_object_set_data_full (GTK_OBJECT (window3), "button3", button3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button3);
    gtk_fixed_put (GTK_FIXED (fixed3), button3, 104, 166);
    gtk_widget_set_uposition (button3, 104, 166);
    gtk_widget_set_usize (button3, 79, 24);

    button4 = pixmap_button (create_button_pixmap(BUTTON_PIX_ADD),"Add");
    gtk_widget_ref (button4);
    gtk_object_set_data_full (GTK_OBJECT (window3), "button4", button4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button4);
    gtk_fixed_put (GTK_FIXED (fixed3), button4, 216, 166);
    gtk_widget_set_uposition (button4, 216, 166);
    gtk_widget_set_usize (button4, 79, 24);

    label9 = gtk_label_new (("Column Name:"));
    gtk_widget_ref (label9);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label9", label9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label9);
    gtk_fixed_put (GTK_FIXED (fixed3), label9, 5, 14);
    gtk_widget_set_uposition (label9, 5, 14);
    /* gtk_widget_set_usize (label9, 80, 24); */

    label10 = gtk_label_new (("Data Type:"));
    gtk_widget_ref (label10);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label10", label10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label10);
    gtk_fixed_put (GTK_FIXED (fixed3), label10, 5, 46);
    gtk_widget_set_uposition (label10, 5, 46);
    /* gtk_widget_set_usize (label10, 80, 24); */

    label11 = gtk_label_new (("Length:"));
    gtk_widget_ref (label11);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label11", label11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label11);
    gtk_fixed_put (GTK_FIXED (fixed3), label11, 5, 78);
    gtk_widget_set_uposition (label11, 5, 78);
    /* gtk_widget_set_usize (label11, 80, 24); */

    label15 = gtk_label_new (("length,precision"));
    gtk_widget_ref (label15);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label15", label15,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label15);
    gtk_fixed_put (GTK_FIXED (fixed3), label15, 290, 78);
    gtk_widget_set_uposition (label15, 290, 78);
    /* gtk_widget_set_usize (label15, 120, 24); */

    label12 = gtk_label_new (("Default Value:"));
    gtk_widget_ref (label12);
    gtk_object_set_data_full (GTK_OBJECT (window3), "label12", label12,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label12);
    gtk_fixed_put (GTK_FIXED (fixed3), label12, 5, 110);
    gtk_widget_set_uposition (label12, 5, 110);
    /* gtk_widget_set_usize (label12, 80, 24); */

    button5 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button5);
    gtk_object_set_data_full (GTK_OBJECT (window3), "button5", button5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button5);
    gtk_fixed_put (GTK_FIXED (fixed3), button5, 210, 252);
    gtk_widget_set_uposition (button5, 210, 252);
    gtk_widget_set_usize (button5, 80, 24);

    button6 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button6);
    gtk_object_set_data_full (GTK_OBJECT (window3), "button6", button6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button6);
    gtk_fixed_put (GTK_FIXED (fixed3), button6, 110, 252);
    gtk_widget_set_uposition (button6, 110, 252);
    gtk_widget_set_usize (button6, 80, 24);

    button7 = pixmap_button (create_button_pixmap(BUTTON_PIX_BACK),"Back");
    gtk_widget_ref (button7);
    gtk_object_set_data_full (GTK_OBJECT (window3), "button7", button7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button7);
    gtk_fixed_put (GTK_FIXED (fixed3), button7, 10, 252);
    gtk_widget_set_uposition (button7, 10, 252);
    gtk_widget_set_usize (button7, 80, 24);

    button8 = pixmap_button (create_button_pixmap(BUTTON_PIX_STORAGE),"Storage");
    gtk_widget_ref (button8);
    gtk_object_set_data_full (GTK_OBJECT (window3), "button8", button8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button8);
    gtk_fixed_put (GTK_FIXED (fixed3), button8, 310, 252);
    gtk_widget_set_uposition (button8, 310, 252);
    gtk_widget_set_usize (button8, 80, 24);

    hseparator1 = gtk_hseparator_new ();
    gtk_widget_ref (hseparator1);
    gtk_object_set_data_full (GTK_OBJECT (window3), "hseparator1", hseparator1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hseparator1);
    gtk_fixed_put (GTK_FIXED (fixed3), hseparator1, 24, 212);
    gtk_widget_set_uposition (hseparator1, 24, 212);
    gtk_widget_set_usize (hseparator1, 272, 16);

    text1 = add_sql_window(vbox2);

    gtk_signal_connect (GTK_OBJECT (window3), "destroy",
			GTK_SIGNAL_FUNC (destroy_win3), NULL);
    gtk_signal_connect (GTK_OBJECT (clist2), "select_row",
			GTK_SIGNAL_FUNC (on_twclist2_click_column),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button3), "clicked",
			GTK_SIGNAL_FUNC (on_twbutton3_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button4), "clicked",
			GTK_SIGNAL_FUNC (on_twbutton4_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button5), "clicked",
			GTK_SIGNAL_FUNC (on_twbutton5_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button6), "clicked",
			GTK_SIGNAL_FUNC (on_twbutton6_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button7), "clicked",
			GTK_SIGNAL_FUNC (on_twbutton7_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button8), "clicked",
			GTK_SIGNAL_FUNC (on_twbutton8_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry3), "changed",
			GTK_SIGNAL_FUNC (on_combo_twentry3_changed),
			NULL);

    storage=" ";
    numcol=0;
  
    return window3;
}

GtkWidget*
obj_create_twstoragewin (void)
{
    GtkWidget *vbox2;
    char *buf;
    GtkWidget *button5;
    GtkWidget *button6;
    GtkWidget *fixed3;

    buf=malloc(strlen(tableinfo.name)+strlen(tableinfo.schema)+30);
    sprintf(buf,"Creating Table %s.%s",tableinfo.name,tableinfo.schema);

    window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window2), "window2", window2);
    gtk_window_set_title (GTK_WINDOW (window2), (buf));
    gtk_window_set_position (GTK_WINDOW (window2), GTK_WIN_POS_CENTER);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "vbox2", vbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (vbox2, 410, 490);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (window2), vbox2);

    obj_create_storagewin(window2,vbox2,0);

    fixed3 = gtk_fixed_new ();
    gtk_widget_ref (fixed3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed3", fixed3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed3);
    gtk_box_pack_start (GTK_BOX (vbox2), fixed3, TRUE, TRUE, 0);

    button5 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "button5", button5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button5);
    gtk_fixed_put (GTK_FIXED (fixed3), button5, 232, 10);
    gtk_widget_set_uposition (button5, 232, 10);
    gtk_widget_set_usize (button5, 80, 24);

    button6 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"OK");
    gtk_widget_ref (button6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "button6", button6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button6);
    gtk_fixed_put (GTK_FIXED (fixed3), button6, 120, 10);
    gtk_widget_set_uposition (button6, 120, 10);
    gtk_widget_set_usize (button6, 80, 24);

    gtk_signal_connect (GTK_OBJECT (button5), "clicked",
			GTK_SIGNAL_FUNC (on_storage_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button6), "clicked",
			GTK_SIGNAL_FUNC (on_storage_clicked),
			NULL);


    return window2;
}

/*
GtkWidget*
obj_create_twwinmain (void)
{
    GtkWidget *witch1,*witch2;
    witch1=get_twwinmain_widget();
    witch2=create_witch (1,&witch1);
    return witch2;
}
*/

