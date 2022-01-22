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
#include <gdk/gdkkeysyms.h>

#include "include/objectman.h"
#include "common-files/common.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

GtkWidget *combo_entry1;
GtkWidget *combo_entry2;
GtkWidget *radiobutton1;
GtkWidget *radiobutton2;
GtkWidget *radiobutton3;
GtkWidget *checkbutton1;
GtkWidget *checkbutton2;
/*GtkWidget *checkbutton3;*/
GtkText   *localbuffer;

static void
on_find_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    char *cp,*searchst,*tmpstr;
    int pos=0,len=0;

    cp = gtk_editable_get_chars(GTK_EDITABLE (localbuffer),(gint) 0,
        (gint) gtk_text_get_length (GTK_TEXT (localbuffer)));

    pos = gtk_editable_get_position (GTK_EDITABLE (localbuffer));

    if(pos==strlen(cp)){
        gtk_editable_set_position (GTK_EDITABLE (localbuffer), 1);
	pos = 1;
    }
    
    len = gtk_text_get_length(GTK_TEXT(localbuffer));

    searchst=gtk_entry_get_text(GTK_ENTRY(combo_entry1));
    tmpstr=g_malloc(strlen(searchst)+1);

    if (GTK_TOGGLE_BUTTON (checkbutton2)->active==0){
        pos++;
        while(pos < len) {
            strncpy(tmpstr,cp+pos,strlen(searchst));
	    if(strnocasestr(tmpstr,searchst)!=NULL){
	        if(!GTK_TOGGLE_BUTTON (checkbutton1)->active || strstr(tmpstr,searchst)!=NULL){
	            gtk_editable_set_position (GTK_EDITABLE (localbuffer), pos);
	            gtk_editable_select_region (GTK_EDITABLE (localbuffer), pos, pos + strlen(searchst));
	            break;
		}
	    }
	    pos++;
        }
    }else{
        int i;
	pos--;
        for (i = pos; i >= 0; i--){
            strncpy(tmpstr,cp+i,strlen(searchst));
	    if(strnocasestr(tmpstr,searchst)!=NULL){
	        if(!GTK_TOGGLE_BUTTON (checkbutton1)->active || strstr(tmpstr,searchst)!=NULL){
	            gtk_editable_set_position (GTK_EDITABLE (localbuffer), i);
	            gtk_editable_select_region (GTK_EDITABLE (localbuffer),i, i + strlen(searchst));
	            break;
		}
	    }
	    
        }
    }
    g_free(cp);
    g_free(tmpstr);

}


static void
on_replace_button_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    char    *search=gtk_entry_get_text(GTK_ENTRY(combo_entry1));
    char    *replace=gtk_entry_get_text(GTK_ENTRY(combo_entry2));
    int     startpos=gtk_editable_get_position (GTK_EDITABLE (localbuffer));
    
    gtk_editable_delete_text (GTK_EDITABLE (localbuffer), startpos, startpos + strlen(search));
    startpos = gtk_editable_get_position (GTK_EDITABLE (localbuffer));
    gtk_editable_insert_text (GTK_EDITABLE (localbuffer),replace, strlen (replace), &startpos);

}

static void
on_radiobutton2_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    gtk_editable_set_position (GTK_EDITABLE (localbuffer), 0);
}

static void
on_radiobutton3_toggled(
    GtkToggleButton *togglebutton,
    gpointer         user_data)
{
    gtk_editable_set_position (GTK_EDITABLE (localbuffer), 
        gtk_text_get_length (GTK_TEXT (localbuffer))-1);
}


static void
on_replaceall_button_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    char *cp,*searchst,*tmpstr;
    int pos=0,len=0;

    cp = gtk_editable_get_chars(GTK_EDITABLE (localbuffer),(gint) 0,
        (gint) gtk_text_get_length (GTK_TEXT (localbuffer)));

    pos = gtk_editable_get_position (GTK_EDITABLE (localbuffer));

    if(pos==strlen(cp)){
        gtk_editable_set_position (GTK_EDITABLE (localbuffer), 1);
	pos = 1;
    }
    
    len = gtk_text_get_length(GTK_TEXT(localbuffer));

    searchst=gtk_entry_get_text(GTK_ENTRY(combo_entry1));
    tmpstr=g_malloc(strlen(searchst)+1);

    if (GTK_TOGGLE_BUTTON (checkbutton2)->active==0){
        int    i;
	pos--;
        for (i = pos; i < len; i++){
            strncpy(tmpstr,cp+i,strlen(searchst));
	    if(strnocasestr(tmpstr,searchst)!=NULL){
	        if(!GTK_TOGGLE_BUTTON (checkbutton1)->active || strstr(tmpstr,searchst)!=NULL){
	            gtk_editable_set_position (GTK_EDITABLE (localbuffer), i);
		    on_replace_button_clicked(NULL, NULL);
		    cp = gtk_editable_get_chars(GTK_EDITABLE (localbuffer),(gint) 0,
                        (gint) gtk_text_get_length (GTK_TEXT (localbuffer)));
		}
	    }
	    
        }
    }else{
        int    i;
	pos--;
        for (i = pos; i >= 0; i--){
            strncpy(tmpstr,cp+i,strlen(searchst));
	    if(strnocasestr(tmpstr,searchst)!=NULL){
	        if(!GTK_TOGGLE_BUTTON (checkbutton1)->active || strstr(tmpstr,searchst)!=NULL){
	            gtk_editable_set_position (GTK_EDITABLE (localbuffer), i);
		    on_replace_button_clicked(NULL, NULL);
		}
	    }
	    
        }
    }
    g_free(cp);
    g_free(tmpstr);

}


static void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(user_data);
}


GtkWidget*
create_search_window (GtkText *textbox)
{
  GtkWidget *window1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *combo1;
  GtkWidget *label2;
  GtkWidget *combo2;
  GSList *g1_group = NULL;
  GtkWidget *hseparator1;
  GtkWidget *hbuttonbox1;
  GtkWidget *find_button;
  GtkWidget *replace_button;
  GtkWidget *replaceall_button;
  GtkWidget *cancel_button;

  localbuffer = textbox;

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
  gtk_window_set_title (GTK_WINDOW (window1), ("Search / Replace"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW (window1), TRUE);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
/*  gtk_widget_set_usize(vbox1, 400, 260);*/
  gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  label1 = gtk_label_new (("Search For:"));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  combo1 = gtk_combo_new ();
  gtk_widget_ref (combo1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "combo1", combo1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo1);
  gtk_box_pack_start (GTK_BOX (vbox1), combo1, FALSE, FALSE, 0);

  combo_entry1 = GTK_COMBO (combo1)->entry;
  gtk_widget_ref (combo_entry1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry1", combo_entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry1);

  label2 = gtk_label_new (("Replace With:"));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (vbox1), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  combo2 = gtk_combo_new ();
  gtk_widget_ref (combo2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "combo2", combo2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo2);
  gtk_box_pack_start (GTK_BOX (vbox1), combo2, FALSE, FALSE, 0);

  combo_entry2 = GTK_COMBO (combo2)->entry;
  gtk_widget_ref (combo_entry2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry2", combo_entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry2);

  radiobutton1 = gtk_radio_button_new_with_label (g1_group, ("Search From Cursor"));
  g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
  gtk_widget_ref (radiobutton1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton1", radiobutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton1, FALSE, FALSE, 0);

  radiobutton2 = gtk_radio_button_new_with_label (g1_group, ("Search From Beginning"));
  g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
  gtk_widget_ref (radiobutton2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton2", radiobutton2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton2);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton2, FALSE, FALSE, 0);

  radiobutton3 = gtk_radio_button_new_with_label (g1_group, ("Search From End"));
  g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
  gtk_widget_ref (radiobutton3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton3", radiobutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton3);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton3, FALSE, FALSE, 0);

  checkbutton1 = gtk_check_button_new_with_label (("Case Sensitive"));
  gtk_widget_ref (checkbutton1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton1", checkbutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbutton1);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbutton1, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton1), TRUE);

  checkbutton2 = gtk_check_button_new_with_label (("Reverse Search"));
  gtk_widget_ref (checkbutton2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton2", checkbutton2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbutton2);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbutton2, FALSE, FALSE, 0);

/*  checkbutton3 = gtk_check_button_new_with_label (("Confirm Replacement"));
  gtk_widget_ref (checkbutton3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton3", checkbutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbutton3);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbutton3, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton3), TRUE);*/

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_ref (hseparator1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hseparator1", hseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, TRUE, TRUE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);

  find_button = gtk_button_new_with_label (("Find"));
  gtk_widget_ref (find_button);
  gtk_object_set_data_full (GTK_OBJECT (window1), "find_button", find_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (find_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), find_button);
  GTK_WIDGET_SET_FLAGS (find_button, GTK_CAN_DEFAULT);

  replace_button = gtk_button_new_with_label (("Replace"));
  gtk_widget_ref (replace_button);
  gtk_object_set_data_full (GTK_OBJECT (window1), "replace_button", replace_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (replace_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), replace_button);
  GTK_WIDGET_SET_FLAGS (replace_button, GTK_CAN_DEFAULT);

  replaceall_button = gtk_button_new_with_label (("Replace All"));
  gtk_widget_ref (replaceall_button);
  gtk_object_set_data_full (GTK_OBJECT (window1), "replaceall_button", replaceall_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (replaceall_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), replaceall_button);
  GTK_WIDGET_SET_FLAGS (replaceall_button, GTK_CAN_DEFAULT);

  cancel_button = gtk_button_new_with_label (("Cancel"));
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (window1), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (find_button), "clicked",
                      GTK_SIGNAL_FUNC (on_find_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (replace_button), "clicked",
                      GTK_SIGNAL_FUNC (on_replace_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (replaceall_button), "clicked",
                      GTK_SIGNAL_FUNC (on_replaceall_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button), "clicked",
                      GTK_SIGNAL_FUNC (on_cancel_button_clicked),
                      window1);
  gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton2_toggled),
			NULL);
  gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
			GTK_SIGNAL_FUNC (on_radiobutton3_toggled),
			NULL);

  return window1;
}

