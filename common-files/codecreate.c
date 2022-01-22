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
#include "objects/procedures/procfuncs.h"
#include "objects/tables/tablefuncs.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <signal.h>

GtkWidget *codecombo_entry1;
GtkWidget *codeentry1;
GtkWidget *codewindow1;

void
on_codebutton1_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *codename  = gtk_entry_get_text(GTK_ENTRY(codeentry1));
    char	 *ownername = gtk_entry_get_text(GTK_ENTRY(codecombo_entry1));
    char	 *query;

    if(strlen(codename)>0 && strlen(ownername)>0){
	switch(GPOINTER_TO_INT(user_data)) {
	case OMDB_Procedure: {
	    query=g_malloc(strlen(ownername)+strlen(codename)+201);
	    sprintf(query,"CREATE PROCEDURE %s.%s (dummy number)\n"
		    "AS\n\nmychar varchar2(50);\n\nBEGIN\n\nmychar := "
		    "'Oracle on Linux is groovy baby, yeah!';\n\nend;\n",ownername,codename);
	    break;
	}
	case OMDB_Function: {
	    query=g_malloc(strlen(ownername)+strlen(codename)+245);
	    sprintf(query,"CREATE FUNCTION %s.%s (dummy number)\n"
		    "RETURN number IS\n\nmynum number;\n\nBEGIN\n\nmychar := "
		    "'Oracle on Linux is groovy baby, yeah!';\n\n "
		    "mynum := 1\n\nreturn mynum;\n\nend;\n",ownername,codename);
	    break;
	}
	case OMDB_Package: {
	    query=g_malloc(strlen(ownername)+strlen(codename)+70);
	    sprintf(query,"CREATE PACKAGE %s.%s AS\n\nREM Enter Proc/Func "
		    "List Here\n\nend;\n",ownername,codename);
	    break;
	}
	case OMDB_PackageBody: {
	    query=g_malloc(strlen(ownername)+strlen(codename)+100);
	    sprintf(query,"CREATE PACKAGE BODY %s.%s (dummy number)"
		    "\nAS\n\nREM Enter Procs/Funcs Here\n\nend;\n",ownername,codename);
	    break;
	}
    
	default: {
	    query=NULL;
	    g_error("Something nasty happened.");
	}
	}
   
	if (execute_immediate_sql(obj_cinfo.login->dbid,query))
	    msgbox("Code Object Created");
    
	gtk_widget_destroy(codewindow1);
    }
}


void
on_codebutton2_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(codewindow1);
}


GtkWidget*
create_codewindow (
    char	    *codetype,
    OMDBObjectType  otype)
{
    GtkWidget	    *vbox1;
    GtkWidget	    *fixed1;
    GtkWidget	    *combo1;
    GtkWidget	    *label2;
    GtkWidget	    *label1;
    GtkWidget	    *hbuttonbox1;
    GtkWidget	    *button1;
    GtkWidget	    *button2;
    char	    *buf;

    buf=g_malloc(strlen(codetype)+8);
    sprintf(buf,"Create %s",codetype);
  
    codewindow1 = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_object_set_data (GTK_OBJECT (codewindow1), "codewindow1", codewindow1);
    gtk_window_set_title (GTK_WINDOW (codewindow1), (buf));
    gtk_window_set_position (GTK_WINDOW (codewindow1), GTK_WIN_POS_CENTER);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_widget_set_usize (vbox1, 266, 134);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (codewindow1), vbox1);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_box_pack_start (GTK_BOX (vbox1), fixed1, TRUE, TRUE, 0);

    codeentry1 = gtk_entry_new ();
    gtk_widget_ref (codeentry1);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "codeentry1", codeentry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (codeentry1);
    gtk_fixed_put (GTK_FIXED (fixed1), codeentry1, 32, 24);
    gtk_widget_set_uposition (codeentry1, 32, 24);
    gtk_widget_set_usize (codeentry1, 208, 24);

    combo1 = gtk_combo_new ();
    gtk_widget_ref (combo1);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "combo1", combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo1);
    gtk_fixed_put (GTK_FIXED (fixed1), combo1, 32, 72);
    gtk_widget_set_uposition (combo1, 32, 72);
    gtk_widget_set_usize (combo1, 208, 24);

    codecombo_entry1 = GTK_COMBO (combo1)->entry;
    gtk_widget_ref (codecombo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "codecombo_entry1", codecombo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (codecombo_entry1);

    label2 = gtk_label_new (("Owner"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed1), label2, 32, 56);
    gtk_widget_set_uposition (label2, 32, 56);
    gtk_widget_set_usize (label2, 208, 16);

    g_free(buf);
    buf=g_malloc(strlen(codetype)+6);
    sprintf(buf,"%s Name:",codetype);

    label1 = gtk_label_new ((buf));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_fixed_put (GTK_FIXED (fixed1), label1, 32, 8);
    gtk_widget_set_uposition (label1, 32, 8);
    gtk_widget_set_usize (label1, 208, 16);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Create");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (codewindow1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_codebutton1_clicked),
			(gpointer)otype);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_codebutton2_clicked),
			NULL);

    generic_populate_schemas(combo1,codecombo_entry1,obj_cinfo.login->dbid);
    return codewindow1;
}

GtkWidget
*create_procedure()
{
    return create_codewindow ("Procedure", OMDB_Procedure);
}

GtkWidget
*create_function()
{
    return create_codewindow ("Function", OMDB_Function);
}

GtkWidget
*create_package()
{
    return create_codewindow ("Package", OMDB_Package);
}

GtkWidget
*create_package_body()
{
    return create_codewindow ("Package Body", OMDB_PackageBody);
}


