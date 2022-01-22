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
#include "tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

char      *object;
char      *owner;
char      *dbid;
GtkWidget *clist;
GtkWidget *entry1;
GtkWidget *entry2;
GtkWidget *text1;
int       datacrow;

void
obj_on_clist10_select_row(
    GtkCList        *clist,
    gint             row,
    gint             column,
    GdkEvent        *event,
    gpointer         user_data)
{
    datacrow = row;
    obj_buildsql();
}

void
obj_on_entry10_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    obj_buildsql();
}

void
obj_on_addcolumnwin_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (text1)));
    
    if (execute_immediate_sql(dbid, cp)) {
	msgbox("column added");
    }
    
    g_free(cp);
    g_free(owner);
    g_free(object);
    g_free(dbid);
    gtk_widget_destroy(user_data);
    
}

void
obj_on_canceladd_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    g_free(owner);
    g_free(object);
    g_free(dbid);
    gtk_widget_destroy(user_data);
    
}       

void
obj_on_entry11_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    obj_buildsql();
}

void
obj_buildsql(void)
{
    GString	 *query;
    char	 *ctype;
    char	 *colname;
    char	 *length  = 0;

    gtk_clist_get_text(GTK_CLIST(clist), datacrow, 1, &ctype);

    /* make length insensitive for datatypes which don't need it setting */
    if(strncmp(ctype,"long",4)==0 ||
       strncmp(ctype,"clob",4)==0 ||
       strncmp(ctype,"blob",4)==0 ||
       strncmp(ctype,"date",4)==0) {
	gtk_widget_set_sensitive(entry2, FALSE);
    } else {
	length = gtk_entry_get_text(GTK_ENTRY(entry2));
	gtk_widget_set_sensitive(entry2, TRUE);
    }
  
    colname=gtk_entry_get_text(GTK_ENTRY(entry1));

    query = g_string_new("ALTER TABLE ");
    g_string_sprintfa(query, " %s.%s ADD (%s %s",
		      owner, object, colname, ctype);
    if (length && strlen(length) > 0) {
	g_string_sprintfa(query, "(%s)", length);
    }
    g_string_append(query, ")");

    clear_text(text1);
    gtk_text_insert (GTK_TEXT (text1), NULL, NULL, NULL,
		     query->str, query->len);
    g_string_free(query, 1);
}


GtkWidget*
obj_create_addcolumn (gpointer data)
{
    GtkWidget	 *window1;
    GtkWidget	 *vbox1;
    GtkWidget	 *hbox1;
    GtkWidget	 *scrolledwindow1;
    GtkWidget	 *fixed1;
    GtkWidget	 *label4;
    GtkWidget	 *label5;
    GtkWidget	 *button1;
    GtkWidget	 *button2;
    GtkWidget    *hbuttonbox1;
    int          num_elements=0, i=0;
    char         *path_elements[4];
    DirNode      *dir  = (DirNode *)data;
    char *datatypes[8] = {"varchar2", "number", "date", "char", "long",
			  "raw", "blob", "clob" } ;

    num_elements=parse_path(dir->path, path_elements, 4);
    
    dbid = g_malloc(strlen(path_elements[DIRPATH_DBID])+1);
    strcpy(dbid,path_elements[DIRPATH_DBID]);
    owner = g_malloc(strlen(path_elements[DIRPATH_OWNER])+1);
    strcpy(owner,path_elements[DIRPATH_OWNER]);
    object = g_malloc(strlen(path_elements[DIRPATH_OBJECT])+1);
    strcpy(object,path_elements[DIRPATH_OBJECT]);
    free_path_elements(path_elements, num_elements);

    window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
    gtk_window_set_title (GTK_WINDOW (window1), "Add Column");
    gtk_window_set_policy (GTK_WINDOW (window1), TRUE, TRUE, FALSE);
    gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
 
    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_object_set_data (GTK_OBJECT (window1), "vbox1", vbox1);
    gtk_widget_set_usize (vbox1, 429, 277);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window1), vbox1);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_object_set_data (GTK_OBJECT (window1), "hbox1", hbox1);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (window1), "scrolledwindow1", scrolledwindow1);
    gtk_widget_set_usize (scrolledwindow1, 100, 290);
    gtk_widget_show (scrolledwindow1);
    gtk_box_pack_start (GTK_BOX (hbox1), scrolledwindow1, TRUE, TRUE, 0);

    clist = gtk_clist_new (2);
    gtk_object_set_data (GTK_OBJECT (window1), "clist", clist);
    gtk_widget_show (clist);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), clist);
    gtk_signal_connect (GTK_OBJECT (clist), "select_row",
			GTK_SIGNAL_FUNC (obj_on_clist10_select_row),
			NULL);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 5);
    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 80);

    for (i = 0; i < sizeof(datatypes)/sizeof(datatypes[0]); i++) {
	char *cols[2];
	cols[0] = 0;
	cols[1] = datatypes[i];
	gtk_clist_append( (GtkCList *) clist, cols);
    }

    fixed1 = gtk_fixed_new ();
    gtk_object_set_data (GTK_OBJECT (window1), "fixed1", fixed1);
    gtk_widget_show (fixed1);
    gtk_box_pack_start (GTK_BOX (hbox1), fixed1, TRUE, TRUE, 0);

    entry1 = gtk_entry_new ();
    gtk_object_set_data (GTK_OBJECT (window1), "entry1", entry1);
    gtk_widget_show (entry1);
    gtk_fixed_put (GTK_FIXED (fixed1), entry1, 32, 24);
    gtk_widget_set_uposition (entry1, 32, 24);
    gtk_widget_set_usize (entry1, 158, 22);
    gtk_signal_connect (GTK_OBJECT (entry1), "changed",
			GTK_SIGNAL_FUNC (obj_on_entry10_changed),
			NULL);

    entry2 = gtk_entry_new ();
    gtk_object_set_data (GTK_OBJECT (window1), "entry2", entry2);
    gtk_widget_show (entry2);
    gtk_fixed_put (GTK_FIXED (fixed1), entry2, 32, 72);
    gtk_widget_set_uposition (entry2, 32, 72);
    gtk_widget_set_usize (entry2, 158, 22);
    gtk_signal_connect (GTK_OBJECT (entry2), "changed",
			GTK_SIGNAL_FUNC (obj_on_entry11_changed),
			NULL);

    label4 = gtk_label_new ("Column Name");
    gtk_object_set_data (GTK_OBJECT (window1), "label4", label4);
    gtk_widget_show (label4);
    gtk_fixed_put (GTK_FIXED (fixed1), label4, 32, 8);
    gtk_widget_set_uposition (label4, 32, 8);

    label5 = gtk_label_new ("Length");
    gtk_object_set_data (GTK_OBJECT (window1), "label5", label5);
    gtk_widget_show (label5);
    gtk_fixed_put (GTK_FIXED (fixed1), label5, 32, 56);
    gtk_widget_set_uposition (label5, 32, 56);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
				  (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"OK");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
				  (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    
    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
				  (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);

    text1 = gtk_text_new (NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (window1), "text1", text1);
    gtk_widget_show (text1);
    gtk_box_pack_start (GTK_BOX (vbox1), text1, TRUE, TRUE, 0);
    gtk_text_set_editable (GTK_TEXT (text1), TRUE);
    
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			    GTK_SIGNAL_FUNC (obj_on_addcolumnwin_clicked),
			    window1);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			    GTK_SIGNAL_FUNC (obj_on_canceladd_clicked),
			    window1);

    return window1;
}
