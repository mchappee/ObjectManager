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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "include/objectman.h"
#include "objects/tables/tablefuncs.h"

typedef struct indexsql_str{
    char	 *iname;
    char	 *ischema;
    char	 *itablespace;
    char	 *tschema;
    char	 *ttable;
    char	 *tcolumns;
    int		 unique;
    int		 sorted;
    int		 recoverable;
    char	 *initsize;
    char	 *nextsize;
    int		 initk;
    int		 nextk;
    char	 *pctincrease;
    char	 *min;
    int		 maxunlimited;
    char	 *max;
    char	 *pctfree;
    char	 *inittrans;
    char	 *maxtrans;
    char	 *lists;
    int		 totalcolumns;
}indexsql_struct;

GtkWidget *combo4;
GtkWidget *combo_entry3;
GtkWidget *combo_entry4;
GtkWidget *clist1;
GtkWidget *text1;
GtkWidget *radiobutton2;
GtkWidget *radiobutton1;
  
static indexsql_struct indexsql;

static void 
build_index_sql(void)
{
    GString *query = g_string_new("CREATE ");
    
    switch (indexsql.unique) {
    case 1: g_string_append(query, "UNIQUE "); break;
    case 2: g_string_append(query, "BITMAP "); break;
    }
    
    g_string_append(query, "INDEX ");

    /* NB Change the initialisation of members to NULL rather than " " */
    if (indexsql.ischema) {
	g_string_append(query, indexsql.ischema);
	g_string_append(query, ".");
    }

    if (indexsql.iname)
	g_string_append(query, indexsql.iname);
    else
	g_string_append(query, "<new-index>");
    g_string_append(query, " ON ");

    if (indexsql.tschema) {
	g_string_append(query, indexsql.tschema);
	g_string_append(query, ".");
    }

    if (indexsql.ttable)
	g_string_append(query, indexsql.ttable);
    else
	g_string_append(query, "<unspecified table>");

    g_string_append(query, " (");
    g_string_append(query, indexsql.tcolumns ? indexsql.tcolumns : "");
    g_string_append(query, ") ");

    if (indexsql.pctfree) {
	g_string_sprintfa(query, "PCTFREE %s ",indexsql.pctfree);
    }

    if(obj_cinfo.login->dbversion){   /* Oracle 8 */
        if(indexsql.inittrans) {
	    g_string_sprintfa(query, "INITTRANS %s ",indexsql.inittrans);
        }

        if(indexsql.maxtrans) {
	    g_string_sprintfa(query, "MAXTRANS %s ",indexsql.maxtrans);
        }
    }

    if(indexsql.initsize || indexsql.nextsize || indexsql.min ||
       indexsql.max || indexsql.pctincrease || indexsql.lists ||
       indexsql.maxunlimited==1 ) {

	g_string_append(query, "STORAGE (");

	if (indexsql.initsize) {
	    g_string_sprintfa(query, "INITIAL %s",indexsql.initsize);

	    if(indexsql.initk)
		g_string_append(query, "K ");
	    else
		g_string_append(query, "M ");
	}

	if (indexsql.nextsize) {
	    g_string_sprintfa(query, "NEXT %s",indexsql.nextsize);

	    if(indexsql.nextk)
		g_string_append(query, "K ");
	    else
		g_string_append(query, "M ");
	}

	if (indexsql.min) {
	    g_string_sprintfa(query, "MINEXTENTS %s ",indexsql.min);
	}

	if(indexsql.maxunlimited){
	    g_string_append(query, "MAXEXTENTS UNLIMITED ");
	} else if(indexsql.max) {
	    g_string_sprintfa(query, "MAXEXTENTS %s ",indexsql.max);
	}
	if(indexsql.pctincrease) {
	    g_string_sprintfa(query, "PCTINCREASE %s ",indexsql.pctincrease);
	}
	if(indexsql.lists) {
	    g_string_sprintfa(query, "FREELISTS %s ",indexsql.lists);
	}
	g_string_append(query, ") ");
    }

    if(indexsql.recoverable) {
	g_string_append(query, "RECOVERABLE ");
    }

    if(!indexsql.sorted) {
	g_string_append(query, "NOSORT ");
    }

    if(indexsql.itablespace) {
	g_string_sprintfa(query, "TABLESPACE %s ",indexsql.itablespace);
    }

    clear_text(text1);
    gtk_text_insert (GTK_TEXT (text1), NULL, NULL,NULL, query->str,
		     query->len);
		     
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(text1));
	
    g_string_free(query, 1);
}

/* General function to handle text entry changes
   user_data is a char ** which points to a field within indexsql */
static void
on_indexer_text_entry_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    char **param = (char **)user_data;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	*param = entry;
    }else{
	*param = 0;
    }
    build_index_sql();
}

static void
on_radiobutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    indexsql.unique=3;
    
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1){
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton1), FALSE);
	indexsql.unique=0;
    }
    
    build_index_sql();
}


static void
on_radiobutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    indexsql.unique=3;
 
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1){
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton2), FALSE);
	indexsql.unique=1;
    }
    build_index_sql();
}


static void
on_checkbutton1_clicked(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0)
	indexsql.sorted=0;
    else
	indexsql.sorted=1;
    
    build_index_sql();
}


static void
on_checkbutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (togglebutton)->active==0)
    indexsql.recoverable=0;
  else
    indexsql.recoverable=1;
    
  build_index_sql();
}


static void
on_combo_entry3_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	indexsql.tschema=entry;
	populate_index_tables(gtk_entry_get_text(GTK_ENTRY(editable)));
    }else{
	indexsql.tschema=0;
	gtk_list_clear_items (GTK_LIST (GTK_COMBO (combo4)->list), 0, -1);
    }
    build_index_sql();
}


static void
on_combo_entry4_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	indexsql.ttable=entry;
	populate_index_columns();
    }else{
	indexsql.ttable=0;
    }
    build_index_sql();
}


static void
on_clist1_click_column(
    GtkCList        *clist,
    gint             row,
    gint             column,
    GdkEvent        *event,
    gpointer         user_data)
{
    GList* sel;
    char *colname;
    char *collist;

    collist=g_malloc(31 * indexsql.totalcolumns);
    collist[0]=0;

    for (sel = clist->selection; sel != NULL; sel = sel->next ) {
	int row_index = GPOINTER_TO_INT ( sel->data );
	gtk_clist_get_text(GTK_CLIST(clist), row_index, 0, &colname);

	strcat(collist, sse(colname));
	if(sel->next)
	    strcat(collist,", ");
    }

    indexsql.tcolumns = g_strdup(collist);
    g_free(collist);
    build_index_sql();
}

static void
on_radiobutton5_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    indexsql.nextk=1;
    build_index_sql();
}


static void
on_radiobutton6_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    indexsql.nextk=0;
    build_index_sql();
}


static void
on_radiobutton4_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    indexsql.initk=0;
    build_index_sql();
}


static void
on_radiobutton3_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    indexsql.initk=1;
    build_index_sql();
}


static void
on_radiobutton8_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    indexsql.maxunlimited=0;
    build_index_sql();
}


static void
on_radiobutton7_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    indexsql.maxunlimited=1;
    build_index_sql();
}

static void
on_button1_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (text1)));

    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("Index Created");
    }
    g_free(cp);
}


static void
on_button2_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
    if(indexsql.tcolumns)
        g_free(indexsql.tcolumns);
	
    gtk_widget_destroy(user_data);
}

static void
free_glist_string_item(
    gpointer list_data,
    gpointer user_data)
{
    g_free(list_data);
}

void populate_index_columns(void)
{
    char	 *query;
    char	 *tabname;
    char	 *owner;
    static char	 qtemplate[] = 
	"SELECT   column_name, data_type "
	"FROM     all_tab_columns "
	"WHERE    table_name = '%s'"
	"         AND owner = '%s' "
	"ORDER BY column_id";

    if(obj_progwin.window4){
	gtk_label_set_text(GTK_LABEL (obj_progwin.label1),"Loading Columns, Please Wait");
	gtk_progress_bar_update( GTK_PROGRESS_BAR(obj_progwin.progressbar1),.50);
    }

    tabname=gtk_entry_get_text(GTK_ENTRY(combo_entry4));
    owner=gtk_entry_get_text(GTK_ENTRY(combo_entry3));

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + strlen(tabname) + 1);
    sprintf(query, qtemplate, tabname, owner);
    gtk_clist_clear(GTK_CLIST(clist1));
    set_clist_from_query(GTK_CLIST(clist1), query, 
			 obj_cinfo.login->dbid, 0, NULL);
    g_free(query);
    indexsql.totalcolumns = GTK_CLIST(clist1)->rows;
}

void 
populate_index_tables(
    const char *owner)
{
    GList *combo1_items = NULL;
    char *query;
    static char qtemplate[] = 
	"SELECT  table_name "
	"FROM    all_tables "
	"WHERE   owner = '%s'";

    query = g_malloc(sizeof(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);
    combo1_items = glist_from_query(query, obj_cinfo.login->dbid, 0, TRUE, NULL);

    indexsql.totalcolumns=0;

    if(combo1_items != NULL) {
	if(obj_cinfo.currenttype != OMDB_Table){
	    gtk_combo_set_popdown_strings (GTK_COMBO (combo4), combo1_items);
	}else{
	    gtk_combo_set_popdown_strings (GTK_COMBO (combo4), combo1_items);
	    gtk_entry_set_text(GTK_ENTRY(combo_entry4),
			       obj_cinfo.currentobject_noowner);
	}
	populate_index_columns();
    } else {
	/* User has no tables so clear out table & column lists */
	gtk_list_clear_items (GTK_LIST (GTK_COMBO (combo4)->list), 0, -1);
	gtk_entry_set_text(GTK_ENTRY(combo_entry4), "");
	gtk_clist_clear(GTK_CLIST(clist1));
    }

    g_list_foreach (combo1_items, free_glist_string_item, 0);
    g_list_free (combo1_items);
}


GtkWidget*
obj_create_indexwin (void)
{
    GtkWidget *vbox1;
    GtkWidget *notebook1;
    GtkWidget *fixed1;
    GtkWidget *frame2;
    GtkWidget *fixed3;
    GtkWidget *frame1;
    GtkWidget *fixed2;
    GtkWidget *scrolledwindow2;
    GtkWidget *label9;
    GtkWidget *label10;
    GtkWidget *label6;
    GtkWidget *label7;
    GtkWidget *label3;
    GtkWidget *label4;
    GtkWidget *label5;
    GtkWidget *label1;
    GtkWidget *fixed4;
    GtkWidget *frame3;
    GtkWidget *fixed5;
    GtkWidget *label11;
    GtkWidget *label12;
    GtkWidget *label15;
    GtkWidget *label17;
    GtkWidget *label16;
    GtkWidget *label18;
    GtkWidget *label13;
    GtkWidget *label14;
    GSList *g3_group = NULL;
    GSList *g2_group = NULL;
    GSList *g4_group = NULL;
    GtkWidget *frame4;
    GtkWidget *fixed6;
    GtkWidget *label19;
    GtkWidget *frame5;
    GtkWidget *fixed7;
    GtkWidget *label20;
    GtkWidget *label21;
    GtkWidget *frame6;
    GtkWidget *fixed8;
    GtkWidget *label22;
    GtkWidget *label2;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *combo1;
    GtkWidget *window1;
    GtkWidget *entry1;
    GtkWidget *combo2;
    GtkWidget *combo_entry2;
    GtkWidget *checkbutton1;
    GtkWidget *checkbutton2;
    GtkWidget *combo3;
    GtkWidget *combo_entry1;
    GtkWidget *entry2;
    GtkWidget *entry3;
    GtkWidget *entry4;
    GtkWidget *entry5;
    GtkWidget *entry6;
    GtkWidget *radiobutton5;
    GtkWidget *radiobutton6;
    GtkWidget *radiobutton4;
    GtkWidget *radiobutton3;
    GtkWidget *radiobutton8;
    GtkWidget *radiobutton7;
    GtkWidget *entry7;
    GtkWidget *entry8;
    GtkWidget *entry9;
    GtkWidget *entry10;

    window1 = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
    gtk_window_set_title (GTK_WINDOW (window1), ("Indexer"));
    gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (vbox1, 410, 581);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window1), vbox1);

    notebook1 = gtk_notebook_new ();
    gtk_widget_ref (notebook1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "notebook1", notebook1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notebook1);
    gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (notebook1), fixed1);

    entry1 = gtk_entry_new ();
    gtk_widget_ref (entry1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry1", entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry1);
    gtk_fixed_put (GTK_FIXED (fixed1), entry1, 104, 8);
    gtk_widget_set_uposition (entry1, 104, 8);
    gtk_widget_set_usize (entry1, 248, 24);

    combo2 = gtk_combo_new ();
    gtk_widget_ref (combo2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo2", combo2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo2);
    gtk_fixed_put (GTK_FIXED (fixed1), combo2, 104, 56);
    gtk_widget_set_uposition (combo2, 104, 56);
    gtk_widget_set_usize (combo2, 248, 24);

    combo_entry2 = GTK_COMBO (combo2)->entry;
    gtk_widget_ref (combo_entry2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry2", combo_entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry2);
    gtk_entry_set_editable (GTK_ENTRY (combo_entry2), FALSE);

    frame2 = gtk_frame_new (NULL);
    gtk_widget_ref (frame2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame2", frame2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame2);
    gtk_fixed_put (GTK_FIXED (fixed1), frame2, 8, 320);
    gtk_widget_set_uposition (frame2, 8, 320);
    gtk_widget_set_usize (frame2, 392, 48);

    fixed3 = gtk_fixed_new ();
    gtk_widget_ref (fixed3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed3", fixed3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed3);
    gtk_container_add (GTK_CONTAINER (frame2), fixed3);
  
    radiobutton2 = gtk_check_button_new_with_label (("Bitmapped"));
    gtk_widget_ref (radiobutton2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton2", radiobutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton2);
    gtk_fixed_put (GTK_FIXED (fixed3), radiobutton2, 96, 8);
    gtk_widget_set_uposition (radiobutton2, 96, 8);
    gtk_widget_set_usize (radiobutton2, 88, 24);
  
    radiobutton1 = gtk_check_button_new_with_label (("Unique"));
    gtk_widget_ref (radiobutton1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton1", radiobutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton1);
    gtk_fixed_put (GTK_FIXED (fixed3), radiobutton1, 8, 8);
    gtk_widget_set_uposition (radiobutton1, 8, 8);
    gtk_widget_set_usize (radiobutton1, 80, 24);

    checkbutton1 = gtk_check_button_new_with_label (("Sorted"));
    gtk_widget_ref (checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton1", checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed3), checkbutton1, 200, 8);
    gtk_widget_set_uposition (checkbutton1, 200, 8);
    gtk_widget_set_usize (checkbutton1, 80, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton1), TRUE);

    checkbutton2 = gtk_check_button_new_with_label (("Recoverable"));
    gtk_widget_ref (checkbutton2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton2", checkbutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton2);
    gtk_fixed_put (GTK_FIXED (fixed3), checkbutton2, 280, 8);
    gtk_widget_set_uposition (checkbutton2, 280, 8);
    gtk_widget_set_usize (checkbutton2, 99, 24);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame1", frame1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_fixed_put (GTK_FIXED (fixed1), frame1, 8, 88);
    gtk_widget_set_uposition (frame1, 8, 88);
    gtk_widget_set_usize (frame1, 392, 224);

    fixed2 = gtk_fixed_new ();
    gtk_widget_ref (fixed2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed2", fixed2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed2);
    gtk_container_add (GTK_CONTAINER (frame1), fixed2);

    combo3 = gtk_combo_new ();
    gtk_widget_ref (combo3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo3", combo3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo3);
    gtk_fixed_put (GTK_FIXED (fixed2), combo3, 96, 8);
    gtk_widget_set_uposition (combo3, 96, 8);
    gtk_widget_set_usize (combo3, 248, 24);

    combo_entry3 = GTK_COMBO (combo3)->entry;
    gtk_widget_ref (combo_entry3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry3", combo_entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry3);
    gtk_entry_set_editable (GTK_ENTRY (combo_entry3), FALSE);

    combo4 = gtk_combo_new ();
    gtk_widget_ref (combo4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo4", combo4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo4);
    gtk_fixed_put (GTK_FIXED (fixed2), combo4, 96, 32);
    gtk_widget_set_uposition (combo4, 96, 32);
    gtk_widget_set_usize (combo4, 248, 24);

    combo_entry4 = GTK_COMBO (combo4)->entry;
    gtk_widget_ref (combo_entry4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry4", combo_entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry4);
    gtk_entry_set_editable (GTK_ENTRY (combo_entry4), FALSE);

    scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow2", scrolledwindow2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow2);
    gtk_fixed_put (GTK_FIXED (fixed2), scrolledwindow2, 8, 64);
    gtk_widget_set_uposition (scrolledwindow2, 8, 64);
    gtk_widget_set_usize (scrolledwindow2, 376, 152);

    clist1 = gtk_clist_new (2);
    gtk_widget_ref (clist1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "clist1", clist1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow2), clist1);
    gtk_clist_set_column_width (GTK_CLIST (clist1), 0, 80);
    gtk_clist_set_column_width (GTK_CLIST (clist1), 1, 80);
    gtk_clist_set_selection_mode (GTK_CLIST (clist1), GTK_SELECTION_MULTIPLE);
    gtk_clist_column_titles_show (GTK_CLIST (clist1));

    label9 = gtk_label_new (("Columns"));
    gtk_widget_ref (label9);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label9", label9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label9);
    gtk_clist_set_column_widget (GTK_CLIST (clist1), 0, label9);

    label10 = gtk_label_new (("Data Type"));
    gtk_widget_ref (label10);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label10", label10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label10);
    gtk_clist_set_column_widget (GTK_CLIST (clist1), 1, label10);

    label6 = gtk_label_new (("Schema:"));
    gtk_widget_ref (label6);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label6", label6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label6);
    gtk_fixed_put (GTK_FIXED (fixed2), label6, 8, 8);
    gtk_widget_set_uposition (label6, 8, 8);
    /* gtk_widget_set_usize (label6, 88, 24); */

    label7 = gtk_label_new (("Table:"));
    gtk_widget_ref (label7);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label7", label7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label7);
    gtk_fixed_put (GTK_FIXED (fixed2), label7, 8, 32);
    gtk_widget_set_uposition (label7, 8, 32);
    /* gtk_widget_set_usize (label7, 88, 24); */

    label3 = gtk_label_new (("Name:"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed1), label3, 16, 8);
    gtk_widget_set_uposition (label3, 16, 8);
    /* gtk_widget_set_usize (label3, 88, 24); */

    label4 = gtk_label_new (("Schema:"));
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_fixed_put (GTK_FIXED (fixed1), label4, 16, 32);
    gtk_widget_set_uposition (label4, 16, 32);
    /* gtk_widget_set_usize (label4, 88, 24); */

    label5 = gtk_label_new (("Tablespace:"));
    gtk_widget_ref (label5);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label5", label5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label5);
    gtk_fixed_put (GTK_FIXED (fixed1), label5, 16, 56);
    gtk_widget_set_uposition (label5, 16, 56);
    /* gtk_widget_set_usize (label5, 88, 24); */

    combo1 = gtk_combo_new ();
    gtk_widget_ref (combo1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo1", combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo1);
    gtk_fixed_put (GTK_FIXED (fixed1), combo1, 104, 32);
    gtk_widget_set_uposition (combo1, 104, 32);
    gtk_widget_set_usize (combo1, 248, 24);

    combo_entry1 = GTK_COMBO (combo1)->entry;
    gtk_widget_ref (combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry1", combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry1);
    gtk_entry_set_editable (GTK_ENTRY (combo_entry1), FALSE);

    label1 = gtk_label_new (("General"));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

    fixed4 = gtk_fixed_new ();
    gtk_widget_ref (fixed4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed4", fixed4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed4);
    gtk_container_add (GTK_CONTAINER (notebook1), fixed4);

    frame3 = gtk_frame_new (("Extents"));
    gtk_widget_ref (frame3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame3", frame3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame3);
    gtk_fixed_put (GTK_FIXED (fixed4), frame3, 8, 8);
    gtk_widget_set_uposition (frame3, 8, 8);
    gtk_widget_set_usize (frame3, 392, 190);

    fixed5 = gtk_fixed_new ();
    gtk_widget_ref (fixed5);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed5", fixed5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed5);
    gtk_container_add (GTK_CONTAINER (frame3), fixed5);

    entry2 = gtk_entry_new ();
    gtk_widget_ref (entry2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry2", entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry2);
    gtk_fixed_put (GTK_FIXED (fixed5), entry2, 96, 16);
    gtk_widget_set_uposition (entry2, 96, 16);
    gtk_widget_set_usize (entry2, 168, 24);

    entry3 = gtk_entry_new ();
    gtk_widget_ref (entry3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry3", entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry3);
    gtk_fixed_put (GTK_FIXED (fixed5), entry3, 96, 40);
    gtk_widget_set_uposition (entry3, 96, 40);
    gtk_widget_set_usize (entry3, 168, 24);

    entry4 = gtk_entry_new ();
    gtk_widget_ref (entry4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry4", entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry4);
    gtk_fixed_put (GTK_FIXED (fixed5), entry4, 96, 72);
    gtk_widget_set_uposition (entry4, 96, 72);
    gtk_widget_set_usize (entry4, 168, 24);

    entry5 = gtk_entry_new ();
    gtk_widget_ref (entry5);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry5", entry5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry5);
    gtk_fixed_put (GTK_FIXED (fixed5), entry5, 96, 96);
    gtk_widget_set_uposition (entry5, 96, 96);
    gtk_widget_set_usize (entry5, 168, 24);

    entry6 = gtk_entry_new ();
    gtk_widget_ref (entry6);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry6", entry6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry6);
    gtk_fixed_put (GTK_FIXED (fixed5), entry6, 288, 136);
    gtk_widget_set_uposition (entry6, 288, 136);
    gtk_widget_set_usize (entry6, 88, 22);

    label11 = gtk_label_new (("Initial Size:"));
    gtk_widget_ref (label11);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label11", label11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label11);
    gtk_fixed_put (GTK_FIXED (fixed5), label11, 8, 16);
    gtk_widget_set_uposition (label11, 8, 16);
    /* gtk_widget_set_usize (label11, 88, 24); */

    label12 = gtk_label_new (("Next Size:"));
    gtk_widget_ref (label12);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label12", label12,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label12);
    gtk_fixed_put (GTK_FIXED (fixed5), label12, 8, 40);
    gtk_widget_set_uposition (label12, 8, 40);
    /* gtk_widget_set_usize (label12, 88, 24); */

    label15 = gtk_label_new (("Increase By:"));
    gtk_widget_ref (label15);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label15", label15,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label15);
    gtk_fixed_put (GTK_FIXED (fixed5), label15, 8, 72);
    gtk_widget_set_uposition (label15, 8, 72);
    /* gtk_widget_set_usize (label15, 89, 24); */

    label17 = gtk_label_new (("%"));
    gtk_widget_ref (label17);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label17", label17,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label17);
    gtk_fixed_put (GTK_FIXED (fixed5), label17, 264, 80);
    gtk_widget_set_uposition (label17, 264, 80);
    /* gtk_widget_set_usize (label17, 41, 16); */

    label16 = gtk_label_new (("Minimum:"));
    gtk_widget_ref (label16);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label16", label16,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label16);
    gtk_fixed_put (GTK_FIXED (fixed5), label16, 8, 96);
    gtk_widget_set_uposition (label16, 8, 96);
    /* gtk_widget_set_usize (label16, 88, 24); */

    label18 = gtk_label_new (("Maximum:"));
    gtk_widget_ref (label18);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label18", label18,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label18);
    gtk_fixed_put (GTK_FIXED (fixed5), label18, 8, 136);
    gtk_widget_set_uposition (label18, 8, 136);
    /* gtk_widget_set_usize (label18, 88, 24); */

    label13 = gtk_label_new (("bytes"));
    gtk_widget_ref (label13);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label13", label13,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label13);
    gtk_fixed_put (GTK_FIXED (fixed5), label13, 336, 16);
    gtk_widget_set_uposition (label13, 336, 16);
    gtk_widget_set_usize (label13, 48, 24);

    label14 = gtk_label_new (("bytes"));
    gtk_widget_ref (label14);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label14", label14,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label14);
    gtk_fixed_put (GTK_FIXED (fixed5), label14, 336, 40);
    gtk_widget_set_uposition (label14, 336, 40);
    gtk_widget_set_usize (label14, 48, 24); 

    radiobutton5 = gtk_radio_button_new_with_label (g3_group, ("K"));
    g3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton5));
    gtk_widget_ref (radiobutton5);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton5", radiobutton5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton5);
    gtk_fixed_put (GTK_FIXED (fixed5), radiobutton5, 264, 40);
    gtk_widget_set_uposition (radiobutton5, 264, 40);
    gtk_widget_set_usize (radiobutton5, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton5), TRUE);

    radiobutton6 = gtk_radio_button_new_with_label (g3_group, ("M"));
    g3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton6));
    gtk_widget_ref (radiobutton6);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton6", radiobutton6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton6);
    gtk_fixed_put (GTK_FIXED (fixed5), radiobutton6, 296, 40);
    gtk_widget_set_uposition (radiobutton6, 296, 40);
    gtk_widget_set_usize (radiobutton6, 32, 24);

    radiobutton4 = gtk_radio_button_new_with_label (g2_group, ("M"));
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton4));
    gtk_widget_ref (radiobutton4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton4", radiobutton4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton4);
    gtk_fixed_put (GTK_FIXED (fixed5), radiobutton4, 296, 16);
    gtk_widget_set_uposition (radiobutton4, 296, 16);
    gtk_widget_set_usize (radiobutton4, 32, 24);

    radiobutton3 = gtk_radio_button_new_with_label (g2_group, ("K"));
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
    gtk_widget_ref (radiobutton3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton3", radiobutton3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton3);
    gtk_fixed_put (GTK_FIXED (fixed5), radiobutton3, 264, 16);
    gtk_widget_set_uposition (radiobutton3, 264, 16);
    gtk_widget_set_usize (radiobutton3, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton3), TRUE);

    radiobutton8 = gtk_radio_button_new_with_label (g4_group, ("Fixed:"));
    g4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton8));
    gtk_widget_ref (radiobutton8);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton8", radiobutton8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton8);
    gtk_fixed_put (GTK_FIXED (fixed5), radiobutton8, 192, 136);
    gtk_widget_set_uposition (radiobutton8, 192, 136);
    gtk_widget_set_usize (radiobutton8, 93, 24);

    radiobutton7 = gtk_radio_button_new_with_label (g4_group, ("Unlimited"));
    g4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton7));
    gtk_widget_ref (radiobutton7);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton7", radiobutton7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton7);
    gtk_fixed_put (GTK_FIXED (fixed5), radiobutton7, 96, 136);
    gtk_widget_set_uposition (radiobutton7, 96, 136);
    gtk_widget_set_usize (radiobutton7, 93, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton7), TRUE);

    frame4 = gtk_frame_new (("Space Usage"));
    gtk_widget_ref (frame4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame4", frame4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame4);
    gtk_fixed_put (GTK_FIXED (fixed4), frame4, 8, 200);
    gtk_widget_set_uposition (frame4, 8, 200);
    gtk_widget_set_usize (frame4, 392, 64);

    fixed6 = gtk_fixed_new ();
    gtk_widget_ref (fixed6);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed6", fixed6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed6);
    gtk_container_add (GTK_CONTAINER (frame4), fixed6);

    entry7 = gtk_entry_new ();
    gtk_widget_ref (entry7);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry7", entry7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry7);
    gtk_fixed_put (GTK_FIXED (fixed6), entry7, 112, 8);
    gtk_widget_set_uposition (entry7, 112, 8);
    gtk_widget_set_usize (entry7, 160, 24);

    label19 = gtk_label_new (("Percent Free:"));
    gtk_widget_ref (label19);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label19", label19,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label19);
    gtk_fixed_put (GTK_FIXED (fixed6), label19, 8, 8);
    gtk_widget_set_uposition (label19, 8, 8);
    /* gtk_widget_set_usize (label19, 104, 24); */

    frame5 = gtk_frame_new (("Transactions"));
    gtk_widget_ref (frame5);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame5", frame5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame5);
    gtk_fixed_put (GTK_FIXED (fixed4), frame5, 8, 266);
    gtk_widget_set_uposition (frame5, 8, 266);
    gtk_widget_set_usize (frame5, 392, 64);

    fixed7 = gtk_fixed_new ();
    gtk_widget_ref (fixed7);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed7", fixed7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed7);
    gtk_container_add (GTK_CONTAINER (frame5), fixed7);

    entry8 = gtk_entry_new ();
    gtk_widget_ref (entry8);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry8", entry8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry8);
    gtk_fixed_put (GTK_FIXED (fixed7), entry8, 88, 8);
    gtk_widget_set_uposition (entry8, 88, 8);
    gtk_widget_set_usize (entry8, 96, 24);

    entry9 = gtk_entry_new ();
    gtk_widget_ref (entry9);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry9", entry9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry9);
    gtk_fixed_put (GTK_FIXED (fixed7), entry9, 264, 8);
    gtk_widget_set_uposition (entry9, 264, 8);
    gtk_widget_set_usize (entry9, 96, 24);

    label20 = gtk_label_new (("Initial:"));
    gtk_widget_ref (label20);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label20", label20,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label20);
    gtk_fixed_put (GTK_FIXED (fixed7), label20, 8, 8);
    gtk_widget_set_uposition (label20, 8, 8);
    /* gtk_widget_set_usize (label20, 80, 24); */

    label21 = gtk_label_new (("Maximum:"));
    gtk_widget_ref (label21);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label21", label21,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label21);
    gtk_fixed_put (GTK_FIXED (fixed7), label21, 192, 8);
    gtk_widget_set_uposition (label21, 192, 8);
    /* gtk_widget_set_usize (label21, 72, 24); */

    frame6 = gtk_frame_new (("Free Lists"));
    gtk_widget_ref (frame6);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame6", frame6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame6);
    gtk_fixed_put (GTK_FIXED (fixed4), frame6, 8, 338);
    gtk_widget_set_uposition (frame6, 8, 338);
    gtk_widget_set_usize (frame6, 392, 64);

    fixed8 = gtk_fixed_new ();
    gtk_widget_ref (fixed8);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed8", fixed8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed8);
    gtk_container_add (GTK_CONTAINER (frame6), fixed8);

    entry10 = gtk_entry_new ();
    gtk_widget_ref (entry10);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry10", entry10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry10);
    gtk_fixed_put (GTK_FIXED (fixed8), entry10, 88, 8);
    gtk_widget_set_uposition (entry10, 88, 8);
    gtk_widget_set_usize (entry10, 200, 24);

    label22 = gtk_label_new (("Lists:"));
    gtk_widget_ref (label22);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label22", label22,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label22);
    gtk_fixed_put (GTK_FIXED (fixed8), label22, 8, 8);
    gtk_widget_set_uposition (label22, 8, 8);
    /* gtk_widget_set_usize (label22, 88, 24); */

    label2 = gtk_label_new (("Storage"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, TRUE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    text1 = add_sql_window(vbox1);

    gtk_signal_connect (GTK_OBJECT (entry1), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.iname);
    gtk_signal_connect (GTK_OBJECT (combo_entry2), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.itablespace);
    gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_checkbutton1_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton2), "toggled",
			GTK_SIGNAL_FUNC (on_checkbutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry3), "changed",
			GTK_SIGNAL_FUNC (on_combo_entry3_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry4), "changed",
			GTK_SIGNAL_FUNC (on_combo_entry4_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (clist1), "select_row",
			GTK_SIGNAL_FUNC (on_clist1_click_column),
			NULL);
    gtk_signal_connect (GTK_OBJECT (clist1), "unselect_row",
			GTK_SIGNAL_FUNC (on_clist1_click_column),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.ischema);
    gtk_signal_connect (GTK_OBJECT (entry2), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.initsize);
    gtk_signal_connect (GTK_OBJECT (entry3), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.nextsize);
    gtk_signal_connect (GTK_OBJECT (entry4), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.pctincrease);
    gtk_signal_connect (GTK_OBJECT (entry5), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.min);
    gtk_signal_connect (GTK_OBJECT (entry6), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.max);
    gtk_signal_connect (GTK_OBJECT (radiobutton5), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton5_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton6), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton6_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton4), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton4_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton3_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton8), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton8_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton7), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton7_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (entry7), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.pctfree);
    gtk_signal_connect (GTK_OBJECT (entry8), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.inittrans);
    gtk_signal_connect (GTK_OBJECT (entry9), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.maxtrans);
    gtk_signal_connect (GTK_OBJECT (entry10), "changed",
			GTK_SIGNAL_FUNC (on_indexer_text_entry_changed),
			&indexsql.lists);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_button1_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_button2_clicked),
			window1);

    indexsql.iname=0;
    indexsql.ischema=0;
    indexsql.itablespace=0;
    indexsql.tschema=0;
    indexsql.ttable=0;
    indexsql.tcolumns=0;
    indexsql.unique=3;
    indexsql.sorted=1;
    indexsql.recoverable=0;
    indexsql.initsize=0;
    indexsql.nextsize=0;
    indexsql.initk=1;
    indexsql.nextk=1;
    indexsql.pctincrease=0;
    indexsql.min=0;
    indexsql.maxunlimited=1;
    indexsql.max=0;
    indexsql.pctfree=0;
    indexsql.inittrans=0;
    indexsql.maxtrans=0;
    indexsql.lists=0;

    obj_progwin.window4 = obj_create_progressbar();
    gtk_widget_show (obj_progwin.window4);
    gtk_widget_realize (obj_progwin.label1);
   
    gtk_label_set_text(GTK_LABEL (obj_progwin.label1),"Loading Schemas, Please Wait");

    /* Dupe call */
    generic_populate_schemas(combo1,combo_entry1, obj_cinfo.login->dbid);
    generic_populate_schemas(combo3,combo_entry3, obj_cinfo.login->dbid);
   
    gtk_label_set_text(GTK_LABEL (obj_progwin.label1),"Loading Tablespaces, Please Wait");
    gtk_progress_bar_update( GTK_PROGRESS_BAR(obj_progwin.progressbar1),.75);
   
    generic_populate_tablespace(combo2,combo_entry2, obj_cinfo.login->dbid);
    
    gtk_label_set_text(GTK_LABEL (obj_progwin.label1),"Loading Columns, Please Wait");
    gtk_progress_bar_update( GTK_PROGRESS_BAR(obj_progwin.progressbar1),.99);
    gtk_widget_destroy(obj_progwin.window4);

    return window1;
}

