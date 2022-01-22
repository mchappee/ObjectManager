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
#include "tablespacefuncs.h"
#include "../datafiles/datafilefuncs.h"
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
}datafilesql_struct;

typedef struct tssql_str{
    int			numdatafiles;
    char		*tablespace_name;
    int			online;
    int			permanent;
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
    datafilesql_struct	dfsql[1024];
}tssql_struct;

static tssql_struct tssql;

static void 
build_tablespace_sql(void)
{
    GString *query = g_string_new("CREATE TABLESPACE ");
    int a=0;
    char *dfile;
    
    if(tssql.tablespace_name!=NULL)
	g_string_sprintfa(query, "%s ",tssql.tablespace_name);
    
    if(tssql.numdatafiles){
	g_string_append(query, " DATAFILE ");
	while(a < tssql.numdatafiles){
	    gtk_clist_get_text(GTK_CLIST(create_ts_clist1), a, 0, &dfile);
	    g_string_append(query, dfile);
	    if(a < (tssql.numdatafiles - 1))
		g_string_append(query, ", ");
	  
	    a++;
	
	}
    }

    if(tssql.override){
	g_string_append(query,"DEFAULT STORAGE (");
	if(tssql.initialsize!=NULL){
	    if(tssql.initialsize_k)
		g_string_sprintfa(query, " INITIAL %sK ",tssql.initialsize);
	    else
		g_string_sprintfa(query, " INITIAL %sM ",tssql.initialsize);
	}
    
	if(tssql.nextsize!=NULL){
	    if(tssql.nextsize_k)
		g_string_sprintfa(query, "NEXT %sK ",tssql.nextsize);
	    else
		g_string_sprintfa(query, "NEXT %sM ",tssql.nextsize);
	}
      
	if(tssql.minnumber!=NULL)
	    g_string_sprintfa(query, "MINEXTENTS %s ",tssql.minnumber);    
    
	if(tssql.unlimited)
	    g_string_append(query, "MAXEXTENTS UNLIMITED ");
	else{
	    if(tssql.maxnumber!=NULL)
		g_string_sprintfa(query, "MAXEXTENTS %s ",tssql.maxnumber);
	}
    
	if(tssql.increase!=NULL)
	    g_string_sprintfa(query, "PCTINCREASE %s ",tssql.increase);
    
	g_string_append(query,") ");
    
	if(tssql.minsize!=NULL){
	    if(tssql.minsize_k)
		g_string_sprintfa(query, "MINIMUM EXTENT %sK ",tssql.minsize);
	    else
		g_string_sprintfa(query, "MINIMUM EXTENT %sM ",tssql.minsize);
	}
    }

    if(!tssql.online)
	g_string_append(query, "OFFLINE ");
    
    if(!tssql.permanent)
	g_string_append(query, "TEMPORARY ");

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
	tssql.tablespace_name=entry;
    }else{
	tssql.tablespace_name=NULL;
    }

    build_tablespace_sql();

}


static void
on_tsadd_button5_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *datafile = getdatafile();
    char *list_text[2];
    
    if(datafile != NULL) {
	list_text[0]=datafile;
	list_text[1]=NULL;
	gtk_clist_append( (GtkCList *) create_ts_clist1, list_text);
	g_free(datafile);
	tssql.numdatafiles++;
	build_tablespace_sql();
    }
}


static void
on_tsremove_button6_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    GList* sel = GTK_CLIST(create_ts_clist1)->selection;
    
    gtk_clist_remove (GTK_CLIST(create_ts_clist1),GPOINTER_TO_INT ( sel->data ));
    tssql.numdatafiles--;
    build_tablespace_sql();
}


static void
on_tsoffline_radiobutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.online=0;
    build_tablespace_sql();
}


static void
on_tsonline_radiobutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.online=1;
    build_tablespace_sql();
}


static void
on_tsperm_radiobutton3_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.permanent=1;
    build_tablespace_sql();
}


static void
on_tstemp_radiobutton4_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.permanent=0;
    build_tablespace_sql();
}


static void
on_tsinitial_entry3_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	tssql.initialsize=entry;
    }else{
	tssql.initialsize=NULL;
    }

    build_tablespace_sql();

}


static void
on_tsnext_entry4_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	tssql.nextsize=entry;
    }else{
	tssql.nextsize=NULL;
    }

    build_tablespace_sql();

}


static void
on_tsmin_entry5_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	tssql.minsize=entry;
    }else{
	tssql.minsize=NULL;
    }
    
    build_tablespace_sql();

}


static void
on_tsinc_entry6_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	tssql.increase=entry;
    }else{
	tssql.increase=NULL;
    }

    build_tablespace_sql();

}


static void
on_tsmax_entry7_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	tssql.minnumber=entry;
    }else{
	tssql.minnumber=NULL;
    }

    build_tablespace_sql();

}


static void
on_tsinitialk_radiobutton5_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.initialsize_k=1;
    build_tablespace_sql();
}


static void
on_tsinitialm_radiobutton6_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.initialsize_k=0;
    build_tablespace_sql();
}


static void
on_tsminm_radiobutton10_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.minsize_k=0;
    build_tablespace_sql();
}


static void
on_tsmink_radiobutton9_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.minsize_k=1;
    build_tablespace_sql();
}


static void
on_tsnextk_radiobutton7_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.nextsize_k=1;
    build_tablespace_sql();
}


static void
on_tsnextm_radiobutton8_toggled(
    GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    tssql.nextsize_k=0;
    build_tablespace_sql();
}


static void
on_tsfixedval_entry8_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry = gtk_entry_get_text(GTK_ENTRY(editable));

    if(strlen(entry)>0){
	tssql.maxnumber=entry;
    }else{
	tssql.maxnumber=NULL;
    }

    build_tablespace_sql();
}


static void
on_tsunlimit_radiobutton11_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.unlimited=1;
    build_tablespace_sql();
}


static void
on_tsfixed_radiobutton12_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    tssql.unlimited=0;
    build_tablespace_sql();
}


static void
on_tsoverride_checkbutton1_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==0){
	tssql.override=0;
	gtk_widget_set_sensitive(create_ts_frame3,0);
    }else{
        gtk_widget_set_sensitive(create_ts_frame3,1);
	tssql.override=1;
    }
    
    build_tablespace_sql();
}


static void
on_tscreate_button3_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (create_ts_text2),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (create_ts_text2)));
    
    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("Tablespace has been created");
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
create_tablespace (void)
{
    GtkWidget	 *window2;
    GtkWidget	 *vbox3;
    GtkWidget	 *notebook2;
    GtkWidget	 *fixed5;
    GtkWidget	 *label6;
    GtkWidget	 *button5;
    GtkWidget	 *button6;
    GtkWidget	 *label15;
    GtkWidget	 *frame1;
    GtkWidget	 *fixed6;
    GSList	 *g1_group  = NULL;
    GtkWidget	 *radiobutton2;
    GtkWidget	 *radiobutton1;
    GtkWidget	 *frame2;
    GtkWidget	 *fixed7;
    GSList	 *g2_group  = NULL;
    GtkWidget	 *radiobutton3;
    GtkWidget	 *radiobutton4;
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
    GtkWidget	 *scrolledwindow3;

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

    label6 = gtk_label_new (("Tablespace Name:"));
    gtk_widget_ref (label6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label6", label6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label6);
    gtk_fixed_put (GTK_FIXED (fixed5), label6, 16, 8);
    gtk_widget_set_uposition (label6, 16, 8);
    /* gtk_widget_set_usize (label6, 112, 24); */

    scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "scrolledwindow3", scrolledwindow3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_set_usize (scrolledwindow3, 232, 88);
    gtk_widget_show (scrolledwindow3);
    gtk_fixed_put (GTK_FIXED (fixed5), scrolledwindow3, 16, 120);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    create_ts_clist1 = gtk_clist_new (2);
    gtk_widget_ref (create_ts_clist1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "create_ts_clist1", create_ts_clist1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (create_ts_clist1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow3), create_ts_clist1);
    gtk_widget_set_uposition (create_ts_clist1, 16, 120);
    gtk_widget_set_usize (create_ts_clist1, 400, 100);
    gtk_clist_set_column_width (GTK_CLIST (create_ts_clist1), 0, 300);
    gtk_clist_set_column_width (GTK_CLIST (create_ts_clist1), 1, 10);
    gtk_clist_column_titles_show (GTK_CLIST (create_ts_clist1));

    button5 = pixmap_button (create_button_pixmap(BUTTON_PIX_ADD),"Add");
    gtk_widget_ref (button5);
    gtk_object_set_data_full (GTK_OBJECT (window2), "button5", button5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button5);
    gtk_fixed_put (GTK_FIXED (fixed5), button5, 256, 120);
    gtk_widget_set_uposition (button5, 256, 120);
    gtk_widget_set_usize (button5, 85, 24);

    button6 = pixmap_button (create_button_pixmap(BUTTON_PIX_REMOVE),"Remove");
    gtk_widget_ref (button6);
    gtk_object_set_data_full (GTK_OBJECT (window2), "button6", button6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button6);
    gtk_fixed_put (GTK_FIXED (fixed5), button6, 256, 152);
    gtk_widget_set_uposition (button6, 256, 152);
    gtk_widget_set_usize (button6, 85, 24);

    label15 = gtk_label_new (("Datafile Info:"));
    gtk_widget_ref (label15);
    gtk_object_set_data_full (GTK_OBJECT (window2), "label15", label15,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label15);
    gtk_fixed_put (GTK_FIXED (fixed5), label15, 16, 104);
    gtk_widget_set_uposition (label15, 16, 104);
    /* gtk_widget_set_usize (label15, 80, 16); */
    gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);

    frame1 = gtk_frame_new (("Status"));
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (window2), "frame1", frame1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_fixed_put (GTK_FIXED (fixed5), frame1, 16, 48);
    gtk_widget_set_uposition (frame1, 16, 48);
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

    frame2 = gtk_frame_new (("Type"));
    gtk_widget_ref (frame2);
    gtk_object_set_data_full (GTK_OBJECT (window2), "frame2", frame2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame2);
    gtk_fixed_put (GTK_FIXED (fixed5), frame2, 16, 232);
    gtk_widget_set_uposition (frame2, 16, 232);
    gtk_widget_set_usize (frame2, 296, 48);

    fixed7 = gtk_fixed_new ();
    gtk_widget_ref (fixed7);
    gtk_object_set_data_full (GTK_OBJECT (window2), "fixed7", fixed7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed7);
    gtk_container_add (GTK_CONTAINER (frame2), fixed7);

    radiobutton3 = gtk_radio_button_new_with_label (g2_group, ("Permanent"));
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
    gtk_widget_ref (radiobutton3);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton3", radiobutton3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton3);
    gtk_fixed_put (GTK_FIXED (fixed7), radiobutton3, 16, 0);
    gtk_widget_set_uposition (radiobutton3, 16, 0);
    gtk_widget_set_usize (radiobutton3, 93, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton3), TRUE);

    radiobutton4 = gtk_radio_button_new_with_label (g2_group, ("Temporary"));
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton4));
    gtk_widget_ref (radiobutton4);
    gtk_object_set_data_full (GTK_OBJECT (window2), "radiobutton4", radiobutton4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton4);
    gtk_fixed_put (GTK_FIXED (fixed7), radiobutton4, 184, 0);
    gtk_widget_set_uposition (radiobutton4, 184, 0);
    gtk_widget_set_usize (radiobutton4, 93, 24);

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

    label12 = gtk_label_new (("Minimum Size:"));
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

    gtk_signal_connect (GTK_OBJECT (create_ts_entry2), "changed",
			GTK_SIGNAL_FUNC (on_tsname_entry2_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button5), "clicked",
			GTK_SIGNAL_FUNC (on_tsadd_button5_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button6), "clicked",
			GTK_SIGNAL_FUNC (on_tsremove_button6_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_tsoffline_radiobutton2_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_tsonline_radiobutton1_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
			GTK_SIGNAL_FUNC (on_tsperm_radiobutton3_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton4), "toggled",
			GTK_SIGNAL_FUNC (on_tstemp_radiobutton4_toggled),
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

    tssql.numdatafiles=0;
    tssql.tablespace_name=NULL;
    tssql.online=1;
    tssql.permanent=1;
    tssql.override=0;
    tssql.initialsize=NULL;
    tssql.initialsize_k=1;
    tssql.nextsize=NULL;
    tssql.nextsize_k=1;
    tssql.minsize=NULL;
    tssql.minsize_k=1;
    tssql.increase=NULL;
    tssql.minnumber=NULL;
    tssql.unlimited=1;
    tssql.maxnumber=NULL;

    gtk_widget_set_sensitive(create_ts_frame3,0);

    return window2;
}

static void
obj_on_coalesce_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *query;
    char	 qtemplate[]  = "alter tablespace %s coalesce";
    char         *dbid, *owner;
    
    dbid = strtok(user_data,"~");
    owner = strtok(NULL,"\n");

    query = g_malloc(strlen(qtemplate) + strlen(owner) + 1);
    sprintf(query, qtemplate, owner);

    if(execute_immediate_sql(dbid, query))
        msgbox("Tablespace Coalesced");
 
    g_free(query);
   
}


void 
obj_create_tablespace_toolbar(gpointer data)
{
    GtkWidget	 *button1;
    GtkWidget	 *tmp_toolbar_icon;
    int          tbstate;

    obj_mainwin.dynamictoolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_widget_ref (obj_mainwin.dynamictoolbar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.dynamictoolbar1", obj_mainwin.dynamictoolbar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_RELIEF_NONE);
    gtk_widget_show (obj_mainwin.dynamictoolbar1);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.toolbar1), obj_mainwin.dynamictoolbar1, NULL, NULL);

    tmp_toolbar_icon = create_tablespace_pixmap (TABLESPACETOOLBAR_COALESCE);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button1 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  ("Coalesce"),
					  "ALTER TABLESPACE ... COALESCE", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);

    tbstate=prefs_get_int_var(PREF_TBSTATE);
    if(tbstate == 1)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_TEXT);

    if(tbstate == 2)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_ICONS);

    if(tbstate == 3)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_BOTH);

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (obj_on_coalesce_clicked),
			data);
}

