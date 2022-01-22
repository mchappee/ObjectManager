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

GtkWidget *dialog1;

void
msgbox_adios(void)
{
   gtk_widget_destroy(dialog1);
   dialog1=NULL;
}

GtkWidget*
msgbox_x (char *msg, int w, int h, int modal, GtkCList *clist)
{
    GtkWidget *dialog_vbox1;
    GtkWidget *scrolledwindow1;
    GtkWidget *text1;
    GtkWidget *dialog_action_area1;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    GtkWidget *msgboxclist;
    GdkColormap *cmap;
    GdkColor color;

    if(!dialog1){
	dialog1 = gtk_dialog_new ();
	gtk_object_set_data (GTK_OBJECT (dialog1), "dialog1", dialog1);
	gtk_window_set_title (GTK_WINDOW (dialog1), ("Message for you sir!"));
	GTK_WINDOW (dialog1)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_position (GTK_WINDOW (dialog1), GTK_WIN_POS_CENTER);
	if(modal)
	    gtk_window_set_modal (GTK_WINDOW (dialog1), TRUE);
	gtk_window_set_policy (GTK_WINDOW (dialog1), TRUE, TRUE, FALSE);

	dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
	gtk_object_set_data (GTK_OBJECT (dialog1), "dialog_vbox1", dialog_vbox1);
	if(w==-1 || h==-1)
	    gtk_widget_set_usize (dialog_vbox1, 260, 187);
	else
	    gtk_widget_set_usize (dialog_vbox1, w, h);
	gtk_widget_show (dialog_vbox1);

	scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_ref (scrolledwindow1);
	gtk_object_set_data_full (GTK_OBJECT (dialog1), "scrolledwindow1", scrolledwindow1,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (scrolledwindow1);
	gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
	/*gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);*/

        if(!clist){

	    text1 = gtk_text_new (NULL, NULL);
	    gtk_widget_ref (text1);
	    gtk_object_set_data_full (GTK_OBJECT (dialog1), "text1", text1,
				      (GtkDestroyNotify) gtk_widget_unref);
	    gtk_widget_show (text1);
	    gtk_text_set_word_wrap(GTK_TEXT (text1),TRUE);
	    gtk_container_add (GTK_CONTAINER (scrolledwindow1), text1);

	    cmap = gdk_colormap_get_system();
	    color.red = 0;
	    color.green = 0;
	    color.blue = 0xffff;
	    if (!gdk_color_alloc(cmap, &color)) {
		printf("\ncouldn't allocate color in msgbox");
	    }

	    gtk_text_insert (GTK_TEXT (text1), NULL, &color,NULL, msg, strlen(msg));
	    
        }else{
	
	    msgboxclist = GTK_WIDGET(clist);
	    gtk_widget_ref (msgboxclist);
	    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "msgboxclist", msgboxclist,
				      (GtkDestroyNotify) gtk_widget_unref);
	    gtk_widget_show (msgboxclist);
	    gtk_container_add (GTK_CONTAINER (scrolledwindow1), msgboxclist);
	    gtk_widget_set_usize (msgboxclist, 400, 187);
	    glist_set_opt_col_width(msgboxclist, 0);
            glist_set_opt_col_width(msgboxclist, 1);
            glist_set_opt_col_width(msgboxclist, 2);
            glist_set_opt_col_width(msgboxclist, 3);
	    gtk_clist_column_titles_show (GTK_CLIST (msgboxclist));
	
	}

	dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
	gtk_object_set_data (GTK_OBJECT (dialog1), "dialog_action_area1", dialog_action_area1);
	gtk_widget_show (dialog_action_area1);
	gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

	hbuttonbox1 = gtk_hbutton_box_new ();
	gtk_widget_ref (hbuttonbox1);
	gtk_object_set_data_full (GTK_OBJECT (dialog1), "hbuttonbox1", hbuttonbox1,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (hbuttonbox1);
	gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox1, TRUE, TRUE, 0);

        button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"OK");
	gtk_widget_ref (button1);
	gtk_object_set_data_full (GTK_OBJECT (dialog1), "button1", button1,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (button1);
	gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
	GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

	gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			    GTK_SIGNAL_FUNC (msgbox_adios),
			    NULL);

	gtk_widget_show(dialog1);
  
	return dialog1;
    }else
	return NULL;

}

GtkWidget *msgbox (char *msg)
{
    return msgbox_x(msg, -1, -1, 0, NULL);
}




