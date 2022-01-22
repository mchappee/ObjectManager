/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright © Matthew Chappee <matthew@orasoft.org>
 *             Paul Flinders <paul@orasoft.org>
 *             Wayne Hoxsie <wayne@hoxnet.com>
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

#define _(a) a

GtkWidget *entry1;
GtkWidget *entry2;
GtkWidget *entry3;
GtkWidget *entry4;
char      *gfilename;

static void on_tns_ok_clicked(GtkButton *button,gpointer user_data)
{
    char *entry01=NULL;
    char *entry02=NULL;
    char *entry03=NULL;
    char *entry04=NULL;
    FILE *tnsfile;

    entry01=gtk_entry_get_text(GTK_ENTRY(entry1));
    if(!entry01[0]){
      return;
    }

    entry02=gtk_entry_get_text(GTK_ENTRY(entry2));
    if(!entry02[0]){
      return;
    }

    entry03=gtk_entry_get_text(GTK_ENTRY(entry3));
    if(!entry03[0]){
	return;
    }

    entry04=gtk_entry_get_text(GTK_ENTRY(entry4));
    if(!entry04[0]){
      return;
    }
    if(NULL==(tnsfile=fopen(gfilename,"w"))){
      printf("File I/O error: %s", gfilename);
      return;
    }
    fprintf(tnsfile,"%s =\n  (DESCRIPTION =\n    (ADDRESS = (PROTOCOL= TCP)"
                    "(Host = %s)(Port= %s))\n  (CONNECT_DATA = (SID = %s))"
		    "\n)\n",entry01,entry02,entry03,entry04);
    fclose(tnsfile);
    gtk_main_quit();
}


static void on_tns_cancel_clicked(GtkButton *button,gpointer user_data)
{
    
    gtk_widget_destroy(user_data);
    gtk_main_quit();
}

static void
on_window1_destroyed(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}


static GtkWidget*
create_tns_win (void)
{
    GtkWidget *window1;
    GtkWidget *vbox2;
    GtkWidget *hbox1;
    GtkWidget *label1;
    GtkWidget *hbox2;
    GtkWidget *label2;
    GtkWidget *hbox3;
    GtkWidget *label3;
    GtkWidget *hbox4;
    GtkWidget *label4;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
    gtk_window_set_title (GTK_WINDOW (window1), _("TNS Names"));
    gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "vbox2", vbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (window1), vbox2);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_ref (hbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbox1", hbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox1, TRUE, TRUE, 0);

    label1 = gtk_label_new (_("TNS Name"));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, TRUE, 3);

    entry1 = gtk_entry_new ();
    gtk_widget_ref (entry1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry1", entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry1);
    gtk_box_pack_start (GTK_BOX (hbox1), entry1, TRUE, TRUE, 2);
    gtk_tooltips_set_tip (tooltips, entry1, _("ToolTip"), NULL);

    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_widget_ref (hbox2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbox2", hbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox2);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox2, TRUE, TRUE, 0);

    label2 = gtk_label_new (_("Hostname"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_box_pack_start (GTK_BOX (hbox2), label2, TRUE, TRUE, 6);

    entry2 = gtk_entry_new ();
    gtk_widget_ref (entry2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry2", entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry2);
    gtk_box_pack_start (GTK_BOX (hbox2), entry2, TRUE, TRUE, 2);
    gtk_tooltips_set_tip (tooltips, entry2, _("ToolTip"), NULL);

    hbox3 = gtk_hbox_new (FALSE, 0);
    gtk_widget_ref (hbox3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbox3", hbox3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox3);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox3, TRUE, TRUE, 0);

    label3 = gtk_label_new (_("Port"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_box_pack_start (GTK_BOX (hbox3), label3, TRUE, TRUE, 22);

    entry3 = gtk_entry_new ();
    gtk_widget_ref (entry3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry3", entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry3);
    gtk_box_pack_start (GTK_BOX (hbox3), entry3, TRUE, TRUE, 2);
    gtk_tooltips_set_tip (tooltips, entry3, _("ToolTip"), NULL);

    hbox4 = gtk_hbox_new (FALSE, 0);
    gtk_widget_ref (hbox4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbox4", hbox4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox4);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox4, TRUE, TRUE, 0);

    label4 = gtk_label_new (_("SID"));
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_box_pack_start (GTK_BOX (hbox4), label4, TRUE, TRUE, 23);

    entry4 = gtk_entry_new ();
    gtk_widget_ref (entry4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry4", entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry4);
    gtk_box_pack_start (GTK_BOX (hbox4), entry4, TRUE, TRUE, 2);
    gtk_tooltips_set_tip (tooltips, entry4, _("ToolTip"), NULL);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox2), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"OK");
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

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_tns_ok_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_tns_cancel_clicked),
			window1);
    gtk_signal_connect(GTK_OBJECT(window1), "destroy",
                       GTK_SIGNAL_FUNC(on_window1_destroyed),
                       (gpointer)window1);

    gtk_object_set_data (GTK_OBJECT (window1), "tooltips", tooltips);

    return window1;
}


int
tns_win(
    char    *filename)
{
    GtkWidget   *dialog;
    int         response = 0;

    gfilename = g_strdup(filename);
    dialog = create_tns_win();
    gtk_widget_show(dialog);
    gtk_main();
    return response;
}




