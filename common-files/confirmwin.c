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
#include "common.h"


static void
on_ok_button_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    int *response = (int *)gtk_object_get_data (GTK_OBJECT (user_data), "response");

    *response = 1;
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}

static void
on_dialog_destroyed(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}

static void
on_cancel_button_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}

static GtkWidget*
create_dialog (
    const char *msg,
    int *response)
{
    GtkWidget *dialog1;
    GtkWidget *dialog_vbox1;
    GtkWidget *scrolledwindow1;
    GtkWidget *text1;
    GtkWidget *dialog_action_area1;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    GtkWidget *button2;
    GdkColormap *cmap;
    GdkColor color;

    dialog1 = gtk_dialog_new ();
    gtk_window_set_title (GTK_WINDOW (dialog1), ("Are You Sure?"));
    gtk_window_set_position (GTK_WINDOW (dialog1), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (dialog1), TRUE);
    gtk_window_set_policy (GTK_WINDOW (dialog1), TRUE, TRUE, FALSE);

    gtk_object_set_data (GTK_OBJECT (dialog1), "response", response);

    dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
    gtk_object_set_data (GTK_OBJECT (dialog1), "dialog_vbox1", dialog_vbox1);
    gtk_widget_set_usize (dialog_vbox1, 260, 187);
    gtk_widget_show (dialog_vbox1);

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow1);
    gtk_object_set_data_full (GTK_OBJECT (dialog1), "scrolledwindow1", scrolledwindow1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow1);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

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

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"OK");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (dialog1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");;
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (dialog1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_cancel_button_clicked),
			(gpointer)dialog1);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_ok_button_clicked),
			(gpointer)dialog1);
    gtk_signal_connect(GTK_OBJECT(dialog1), "destroy",
                       GTK_SIGNAL_FUNC(on_dialog_destroyed),
                       (gpointer)dialog1);


    return dialog1;
}

int
confirm(
    const char *msg,
    ...)
{
    va_list     args;
    char        *msgf;
    GtkWidget   *dialog;
    int         response = 0;

    va_start(args, msg);
    msgf = vmsgfmt(msg, args);
    va_end(args);

    dialog = create_dialog(msgf, &response);
    g_free(msgf);
    gtk_widget_show(dialog);
    gtk_main();
    return response;
}

