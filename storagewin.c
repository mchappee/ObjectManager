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

typedef struct storagesql_str{
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
}storagesql_struct;

typedef struct storageparts_str{
    char	 *part17;
    char	 *part18;
    char	 *part18point5;
    char	 *part19;
    char	 *part19point5;
    char	 *part20;
    char	 *part21;
    char	 *part22;
    char	 *part23;
    char	 *part24;
}storageparts_struct;

typedef struct twstorage_str{
  GtkWidget *radiobutton5;
  GtkWidget *radiobutton6;
  GtkWidget *radiobutton4;
  GtkWidget *radiobutton3;
  GtkWidget *radiobutton8;
  GtkWidget *radiobutton7;
  GtkWidget *entry7;
  GtkWidget *entry8;
  GtkWidget *entry9;
  GtkWidget *entry2;
  GtkWidget *entry3;
  GtkWidget *entry4;
  GtkWidget *entry5;
  GtkWidget *entry6;
  GtkWidget *entry10;
}obj_storage_struct;

static storagesql_struct storagesql;
static storageparts_struct storageparts;
obj_storage_struct obj_storagewin;

static void
build_storage_sql(void)
{
    char *sqltext;
    
    sqltext=g_malloc(strlen(storagesql.initsize)+strlen(storagesql.nextsize)+
		     strlen(storagesql.pctincrease)+strlen(storagesql.min)+strlen(storagesql.max)+strlen(storagesql.pctfree)+
		     strlen(storagesql.inittrans)+strlen(storagesql.maxtrans)+strlen(storagesql.lists)+200);
    
    
    if(strcmp(storagesql.initsize," ")||strcmp(storagesql.nextsize," ")||strcmp(storagesql.min," ")||
       strcmp(storagesql.max," ")||strcmp(storagesql.pctincrease," ")||
       strcmp(storagesql.lists," ")||storagesql.maxunlimited==1){
	storageparts.part17="STORAGE (";
	storageparts.part24=") ";
    }else{
	storageparts.part17=" ";
	storageparts.part24=" ";
    }
  
    storageparts.part18=g_malloc(strlen(storagesql.initsize)+20);
    storageparts.part18[0]=0;
    if(strncmp(storagesql.initsize," ",1)!=0){
	sprintf(storageparts.part18,"INITIAL %s ",storagesql.initsize);
    }else{
	strcpy(storageparts.part18," ");
    }  
    
    if(strcmp(storagesql.initsize," ")!=0){
	if(storagesql.initk)
	    storageparts.part18point5="K ";
	else
	    storageparts.part18point5="M ";
    }else{
	storageparts.part18point5=" ";
    }
    
    storageparts.part19=g_malloc(strlen(storagesql.nextsize)+20);  
    if(strcmp(storagesql.nextsize," ")!=0)
	sprintf(storageparts.part19,"NEXT %s ",storagesql.nextsize);
    else
	strcpy(storageparts.part19," ");
    
    if(strcmp(storagesql.nextsize," ")!=0){
	if(storagesql.nextk)
	    storageparts.part19point5="K ";
	else
	    storageparts.part19point5="M ";
    }else{
	storageparts.part19point5=" ";
    }
    
    storageparts.part20=g_malloc(strlen(storagesql.min)+20);  
    if(strcmp(storagesql.min," ")!=0)
	sprintf(storageparts.part20,"MINEXTENTS %s ",storagesql.min);
    else
	strcpy(storageparts.part20," ");
    
    storageparts.part21=g_malloc(strlen(storagesql.max)+20); 
    if(storagesql.maxunlimited){
	sprintf(storageparts.part21,"MAXEXTENTS UNLIMITED ");
    }else{ 
	if(strcmp(storagesql.max," ")!=0)
	    sprintf(storageparts.part21,"MAXEXTENTS %s ",storagesql.max);
	else
	    strcpy(storageparts.part21," ");
    }  
    
    storageparts.part22=g_malloc(strlen(storagesql.pctincrease)+20);  
    if(strcmp(storagesql.pctincrease," ")!=0)
	sprintf(storageparts.part22,"PCTINCREASE %s ",storagesql.pctincrease);
    else
	strcpy(storageparts.part22," ");
    
    storageparts.part23=g_malloc(strlen(storagesql.lists)+20);  
    if(strcmp(storagesql.lists," ")!=0)
	sprintf(storageparts.part23,"FREELISTS %s ",storagesql.lists);
    else
	strcpy(storageparts.part23," ");
  
    
    sprintf(sqltext,"%s%s%s%s%s%s%s%s%s%s",
	    storageparts.part17,storageparts.part18,storageparts.part18point5,storageparts.part19,
	    storageparts.part19point5,storageparts.part20,
	    storageparts.part21,storageparts.part22,storageparts.part23,storageparts.part24);
  
    storage=g_malloc(strlen(sqltext)+1);
    strcpy(storage,sqltext);
  
}

static void
stg_on_entry2_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.initsize=entry;
    else
	storagesql.initsize=" ";
    
    build_storage_sql();
}


static void
stg_on_entry3_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.nextsize=entry;  
    else
	storagesql.nextsize=" ";
    
    build_storage_sql();
}


static void
stg_on_entry4_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.pctincrease=entry;
    else
	storagesql.pctincrease=" ";
    
    build_storage_sql();
}


static void
stg_on_entry5_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.min=entry;
    else
	storagesql.min=" ";
    
    build_storage_sql();
}


static void
stg_on_entry6_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.max=entry;
    else
	storagesql.max=" ";
    
    build_storage_sql();
}


static void
stg_on_radiobutton5_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    storagesql.nextk=1;
    build_storage_sql();}


static void
stg_on_radiobutton6_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    storagesql.nextk=0;
    build_storage_sql();
}


static void
stg_on_radiobutton4_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    storagesql.initk=0;
    build_storage_sql();
}


static void
stg_on_radiobutton3_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    storagesql.initk=1;
    build_storage_sql();
}


static void
stg_on_radiobutton8_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    storagesql.maxunlimited=0;
    build_storage_sql();
}


static void
stg_on_radiobutton7_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    storagesql.maxunlimited=1;
    build_storage_sql();
}


static void
stg_on_entry7_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.pctfree=entry;
    else
	storagesql.pctfree=" ";
    
    build_storage_sql();
}


static void
stg_on_entry8_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.inittrans=entry;
    else
	storagesql.inittrans=" ";
    
    build_storage_sql();
}


static void
stg_on_entry9_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.maxtrans=entry;
    else
	storagesql.maxtrans=" ";
    
    build_storage_sql();
}


static void
stg_on_entry10_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    char *entry=NULL;
    
    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    
    if(strlen(entry)>0)
	storagesql.lists=entry;
    else
	storagesql.lists=" ";
    
    build_storage_sql();
}


GtkWidget*
obj_create_storagewin(
    GtkWidget	 *window,
    GtkWidget	 *packer,
    int		 packtype)
{
    
    GtkWidget	 *fixed4;
    GtkWidget	 *frame3;
    GtkWidget	 *fixed5;
    GtkWidget	 *label11;
    GtkWidget	 *label12;
    GtkWidget	 *label15;
    GtkWidget	 *label17;
    GtkWidget	 *label16;
    GtkWidget	 *label18;
    GtkWidget	 *label13;
    GtkWidget	 *label14;
    GSList	 *g3_group  = NULL;
    GSList	 *g2_group  = NULL;
    GSList	 *g4_group  = NULL;
    GtkWidget	 *frame4;
    GtkWidget	 *fixed6;
    GtkWidget	 *label19;
    GtkWidget	 *frame5;
    GtkWidget	 *fixed7;
    GtkWidget	 *label20;
    GtkWidget	 *label21;
    GtkWidget	 *frame6;
    GtkWidget	 *fixed8;
    GtkWidget	 *label22;

    fixed4 = gtk_fixed_new ();
    gtk_widget_ref (fixed4);
    gtk_object_set_data_full (GTK_OBJECT (window), "fixed4", fixed4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed4);
    if(packtype)
	gtk_container_add (GTK_CONTAINER (packer), fixed4);
    else
	gtk_box_pack_start (GTK_BOX (packer), fixed4, TRUE, TRUE, 0);
  
    frame3 = gtk_frame_new (("Extents"));
    gtk_widget_ref (frame3);
    gtk_object_set_data_full (GTK_OBJECT (window), "frame3", frame3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame3);
    gtk_fixed_put (GTK_FIXED (fixed4), frame3, 8, 8);
    gtk_widget_set_uposition (frame3, 8, 8);
    gtk_widget_set_usize (frame3, 392, 190);

    fixed5 = gtk_fixed_new ();
    gtk_widget_ref (fixed5);
    gtk_object_set_data_full (GTK_OBJECT (window), "fixed5", fixed5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed5);
    gtk_container_add (GTK_CONTAINER (frame3), fixed5);

    obj_storagewin.entry2 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry2);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry2", obj_storagewin.entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry2);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.entry2, 96, 16);
    gtk_widget_set_uposition (obj_storagewin.entry2, 96, 16);
    gtk_widget_set_usize (obj_storagewin.entry2, 168, 24);

    obj_storagewin.entry3 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry3);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry3", obj_storagewin.entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry3);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.entry3, 96, 40);
    gtk_widget_set_uposition (obj_storagewin.entry3, 96, 40);
    gtk_widget_set_usize (obj_storagewin.entry3, 168, 24);

    obj_storagewin.entry4 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry4);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry4", obj_storagewin.entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry4);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.entry4, 96, 72);
    gtk_widget_set_uposition (obj_storagewin.entry4, 96, 72);
    gtk_widget_set_usize (obj_storagewin.entry4, 168, 24);

    obj_storagewin.entry5 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry5);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry5", obj_storagewin.entry5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry5);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.entry5, 96, 96);
    gtk_widget_set_uposition (obj_storagewin.entry5, 96, 96);
    gtk_widget_set_usize (obj_storagewin.entry5, 168, 24);

    obj_storagewin.entry6 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry6);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry6", obj_storagewin.entry6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry6);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.entry6, 288, 136);
    gtk_widget_set_uposition (obj_storagewin.entry6, 288, 136);
    gtk_widget_set_usize (obj_storagewin.entry6, 88, 22);

    label11 = gtk_label_new (("Initial Size:"));
    gtk_widget_ref (label11);
    gtk_object_set_data_full (GTK_OBJECT (window), "label11", label11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label11);
    gtk_fixed_put (GTK_FIXED (fixed5), label11, 8, 16);
    gtk_widget_set_uposition (label11, 8, 16);
    /* gtk_widget_set_usize (label11, 88, 24); */

    label12 = gtk_label_new (("Next Size:"));
    gtk_widget_ref (label12);
    gtk_object_set_data_full (GTK_OBJECT (window), "label12", label12,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label12);
    gtk_fixed_put (GTK_FIXED (fixed5), label12, 8, 40);
    gtk_widget_set_uposition (label12, 8, 40);
    /* gtk_widget_set_usize (label12, 88, 24); */

    label15 = gtk_label_new (("Increase By:"));
    gtk_widget_ref (label15);
    gtk_object_set_data_full (GTK_OBJECT (window), "label15", label15,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label15);
    gtk_fixed_put (GTK_FIXED (fixed5), label15, 8, 72);
    gtk_widget_set_uposition (label15, 8, 72);
    /* gtk_widget_set_usize (label15, 89, 24); */

    label17 = gtk_label_new (("%"));
    gtk_widget_ref (label17);
    gtk_object_set_data_full (GTK_OBJECT (window), "label17", label17,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label17);
    gtk_fixed_put (GTK_FIXED (fixed5), label17, 264, 80);
    gtk_widget_set_uposition (label17, 264, 80);
    /* gtk_widget_set_usize (label17, 41, 16); */

    label16 = gtk_label_new (("Minimum:"));
    gtk_widget_ref (label16);
    gtk_object_set_data_full (GTK_OBJECT (window), "label16", label16,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label16);
    gtk_fixed_put (GTK_FIXED (fixed5), label16, 8, 96);
    gtk_widget_set_uposition (label16, 8, 96);
    /* gtk_widget_set_usize (label16, 88, 24); */

    label18 = gtk_label_new (("Maximum:"));
    gtk_widget_ref (label18);
    gtk_object_set_data_full (GTK_OBJECT (window), "label18", label18,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label18);
    gtk_fixed_put (GTK_FIXED (fixed5), label18, 8, 136);
    gtk_widget_set_uposition (label18, 8, 136);
    /* gtk_widget_set_usize (label18, 88, 24); */

    label13 = gtk_label_new (("bytes"));
    gtk_widget_ref (label13);
    gtk_object_set_data_full (GTK_OBJECT (window), "label13", label13,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label13);
    gtk_fixed_put (GTK_FIXED (fixed5), label13, 336, 16);
    gtk_widget_set_uposition (label13, 336, 16);
    /* gtk_widget_set_usize (label13, 48, 24); */

    label14 = gtk_label_new (("bytes"));
    gtk_widget_ref (label14);
    gtk_object_set_data_full (GTK_OBJECT (window), "label14", label14,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label14);
    gtk_fixed_put (GTK_FIXED (fixed5), label14, 336, 40);
    gtk_widget_set_uposition (label14, 336, 40);
    /* gtk_widget_set_usize (label14, 48, 24); */

    obj_storagewin.radiobutton5 = gtk_radio_button_new_with_label (g3_group, ("K"));
    g3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (obj_storagewin.radiobutton5));
    gtk_widget_ref (obj_storagewin.radiobutton5);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.radiobutton5", obj_storagewin.radiobutton5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.radiobutton5);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.radiobutton5, 264, 40);
    gtk_widget_set_uposition (obj_storagewin.radiobutton5, 264, 40);
    gtk_widget_set_usize (obj_storagewin.radiobutton5, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (obj_storagewin.radiobutton5), TRUE);

    obj_storagewin.radiobutton6 = gtk_radio_button_new_with_label (g3_group, ("M"));
    g3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (obj_storagewin.radiobutton6));
    gtk_widget_ref (obj_storagewin.radiobutton6);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.radiobutton6", obj_storagewin.radiobutton6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.radiobutton6);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.radiobutton6, 296, 40);
    gtk_widget_set_uposition (obj_storagewin.radiobutton6, 296, 40);
    gtk_widget_set_usize (obj_storagewin.radiobutton6, 32, 24);

    obj_storagewin.radiobutton4 = gtk_radio_button_new_with_label (g2_group, ("M"));
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (obj_storagewin.radiobutton4));
    gtk_widget_ref (obj_storagewin.radiobutton4);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.radiobutton4", obj_storagewin.radiobutton4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.radiobutton4);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.radiobutton4, 296, 16);
    gtk_widget_set_uposition (obj_storagewin.radiobutton4, 296, 16);
    gtk_widget_set_usize (obj_storagewin.radiobutton4, 32, 24);

    obj_storagewin.radiobutton3 = gtk_radio_button_new_with_label (g2_group, ("K"));
    g2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (obj_storagewin.radiobutton3));
    gtk_widget_ref (obj_storagewin.radiobutton3);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.radiobutton3", obj_storagewin.radiobutton3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.radiobutton3);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.radiobutton3, 264, 16);
    gtk_widget_set_uposition (obj_storagewin.radiobutton3, 264, 16);
    gtk_widget_set_usize (obj_storagewin.radiobutton3, 32, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (obj_storagewin.radiobutton3), TRUE);

    obj_storagewin.radiobutton8 = gtk_radio_button_new_with_label (g4_group, ("Fixed:"));
    g4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (obj_storagewin.radiobutton8));
    gtk_widget_ref (obj_storagewin.radiobutton8);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.radiobutton8", obj_storagewin.radiobutton8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.radiobutton8);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.radiobutton8, 192, 136);
    gtk_widget_set_uposition (obj_storagewin.radiobutton8, 192, 136);
    gtk_widget_set_usize (obj_storagewin.radiobutton8, 93, 24);

    obj_storagewin.radiobutton7 = gtk_radio_button_new_with_label (g4_group, ("Unlimited"));
    g4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (obj_storagewin.radiobutton7));
    gtk_widget_ref (obj_storagewin.radiobutton7);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.radiobutton7", obj_storagewin.radiobutton7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.radiobutton7);
    gtk_fixed_put (GTK_FIXED (fixed5), obj_storagewin.radiobutton7, 96, 136);
    gtk_widget_set_uposition (obj_storagewin.radiobutton7, 96, 136);
    gtk_widget_set_usize (obj_storagewin.radiobutton7, 93, 24);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (obj_storagewin.radiobutton7), TRUE);

    frame4 = gtk_frame_new (("Space Usage"));
    gtk_widget_ref (frame4);
    gtk_object_set_data_full (GTK_OBJECT (window), "frame4", frame4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame4);
    gtk_fixed_put (GTK_FIXED (fixed4), frame4, 8, 200);
    gtk_widget_set_uposition (frame4, 8, 200);
    gtk_widget_set_usize (frame4, 392, 64);

    fixed6 = gtk_fixed_new ();
    gtk_widget_ref (fixed6);
    gtk_object_set_data_full (GTK_OBJECT (window), "fixed6", fixed6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed6);
    gtk_container_add (GTK_CONTAINER (frame4), fixed6);

    obj_storagewin.entry7 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry7);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry7", obj_storagewin.entry7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry7);
    gtk_fixed_put (GTK_FIXED (fixed6), obj_storagewin.entry7, 112, 8);
    gtk_widget_set_uposition (obj_storagewin.entry7, 112, 8);
    gtk_widget_set_usize (obj_storagewin.entry7, 160, 24);

    label19 = gtk_label_new (("Percent Free:"));
    gtk_widget_ref (label19);
    gtk_object_set_data_full (GTK_OBJECT (window), "label19", label19,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label19);
    gtk_fixed_put (GTK_FIXED (fixed6), label19, 8, 8);
    gtk_widget_set_uposition (label19, 8, 8);
    /* gtk_widget_set_usize (label19, 104, 24); */

    frame5 = gtk_frame_new (("Transactions"));
    gtk_widget_ref (frame5);
    gtk_object_set_data_full (GTK_OBJECT (window), "frame5", frame5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame5);
    gtk_fixed_put (GTK_FIXED (fixed4), frame5, 8, 266);
    gtk_widget_set_uposition (frame5, 8, 266);
    gtk_widget_set_usize (frame5, 392, 64);

    fixed7 = gtk_fixed_new ();
    gtk_widget_ref (fixed7);
    gtk_object_set_data_full (GTK_OBJECT (window), "fixed7", fixed7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed7);
    gtk_container_add (GTK_CONTAINER (frame5), fixed7);

    obj_storagewin.entry8 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry8);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry8", obj_storagewin.entry8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry8);
    gtk_fixed_put (GTK_FIXED (fixed7), obj_storagewin.entry8, 88, 8);
    gtk_widget_set_uposition (obj_storagewin.entry8, 88, 8);
    gtk_widget_set_usize (obj_storagewin.entry8, 96, 24);

    obj_storagewin.entry9 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry9);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry9", obj_storagewin.entry9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry9);
    gtk_fixed_put (GTK_FIXED (fixed7), obj_storagewin.entry9, 264, 8);
    gtk_widget_set_uposition (obj_storagewin.entry9, 264, 8);
    gtk_widget_set_usize (obj_storagewin.entry9, 96, 24);

    label20 = gtk_label_new (("Initial:"));
    gtk_widget_ref (label20);
    gtk_object_set_data_full (GTK_OBJECT (window), "label20", label20,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label20);
    gtk_fixed_put (GTK_FIXED (fixed7), label20, 8, 8);
    gtk_widget_set_uposition (label20, 8, 8);
    /* gtk_widget_set_usize (label20, 80, 24); */

    label21 = gtk_label_new (("Maximum:"));
    gtk_widget_ref (label21);
    gtk_object_set_data_full (GTK_OBJECT (window), "label21", label21,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label21);
    gtk_fixed_put (GTK_FIXED (fixed7), label21, 192, 8);
    gtk_widget_set_uposition (label21, 192, 8);
    /* gtk_widget_set_usize (label21, 72, 24); */

    frame6 = gtk_frame_new (("Free Lists"));
    gtk_widget_ref (frame6);
    gtk_object_set_data_full (GTK_OBJECT (window), "frame6", frame6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame6);
    gtk_fixed_put (GTK_FIXED (fixed4), frame6, 8, 338);
    gtk_widget_set_uposition (frame6, 8, 338);
    gtk_widget_set_usize (frame6, 392, 64);

    fixed8 = gtk_fixed_new ();
    gtk_widget_ref (fixed8);
    gtk_object_set_data_full (GTK_OBJECT (window), "fixed8", fixed8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed8);
    gtk_container_add (GTK_CONTAINER (frame6), fixed8);

    obj_storagewin.entry10 = gtk_entry_new ();
    gtk_widget_ref (obj_storagewin.entry10);
    gtk_object_set_data_full (GTK_OBJECT (window), "obj_storagewin.entry10", obj_storagewin.entry10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_storagewin.entry10);
    gtk_fixed_put (GTK_FIXED (fixed8), obj_storagewin.entry10, 88, 8);
    gtk_widget_set_uposition (obj_storagewin.entry10, 88, 8);
    gtk_widget_set_usize (obj_storagewin.entry10, 200, 24);

    label22 = gtk_label_new (("Lists:"));
    gtk_widget_ref (label22);
    gtk_object_set_data_full (GTK_OBJECT (window), "label22", label22,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label22);
    gtk_fixed_put (GTK_FIXED (fixed8), label22, 8, 8);
    gtk_widget_set_uposition (label22, 8, 8);
    /* gtk_widget_set_usize (label22, 88, 24); */

    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry2), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry2_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry3), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry3_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry4), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry4_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry5), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry5_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry6), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry6_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.radiobutton5), "toggled",
			GTK_SIGNAL_FUNC (stg_on_radiobutton5_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.radiobutton6), "toggled",
			GTK_SIGNAL_FUNC (stg_on_radiobutton6_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.radiobutton4), "toggled",
			GTK_SIGNAL_FUNC (stg_on_radiobutton4_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.radiobutton3), "toggled",
			GTK_SIGNAL_FUNC (stg_on_radiobutton3_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.radiobutton8), "toggled",
			GTK_SIGNAL_FUNC (stg_on_radiobutton8_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.radiobutton7), "toggled",
			GTK_SIGNAL_FUNC (stg_on_radiobutton7_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry7), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry7_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry8), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry8_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry9), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry9_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (obj_storagewin.entry10), "changed",
			GTK_SIGNAL_FUNC (stg_on_entry10_changed),
			NULL);

    storagesql.initsize=" ";
    storagesql.nextsize=" ";
    storagesql.initk=1;
    storagesql.nextk=1;
    storagesql.pctincrease=" ";
    storagesql.min=" ";
    storagesql.maxunlimited=1;
    storagesql.max=" ";
    storagesql.pctfree=" ";
    storagesql.inittrans=" ";
    storagesql.maxtrans=" ";
    storagesql.lists=" ";

    return fixed4; /* ??? */
}

