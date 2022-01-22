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

GtkWidget *entry3;
char *pcommand=NULL;

static void
on_ok_button_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char **response = (char **)gtk_object_get_data (GTK_OBJECT (user_data), "response");

    *response = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry3)));

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


static GtkWidget *
create_printwin (
    char *pcom,
    char **response)
{
    GtkWidget	 *vbox1;
    GtkWidget	 *fixed1;
    GtkWidget	 *pixmap2;
    GtkWidget	 *label1;
    GtkWidget	 *toolbar1;
    GtkWidget	 *tmp_toolbar_icon;
    GtkWidget	 *button13;
    GtkWidget	 *button14;
    GtkWidget	 *window3;
    char	 *homedir;
  
    homedir=user_home_dir();

    window3 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window3), "window3", window3);
    gtk_window_set_title (GTK_WINDOW (window3), "Print");
    gtk_window_set_policy (GTK_WINDOW (window3), TRUE, TRUE, FALSE);
    gtk_signal_connect (GTK_OBJECT (window3), "destroy",
			(GtkSignalFunc) gtk_widget_destroy , NULL);
    gtk_window_set_position (GTK_WINDOW (window3), GTK_WIN_POS_CENTER);

    gtk_object_set_data (GTK_OBJECT (window3), "response", response);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_object_set_data (GTK_OBJECT (window3), "vbox1", vbox1);
    gtk_widget_set_usize (vbox1, 375, 188);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window3), vbox1);

    fixed1 = gtk_fixed_new ();
    gtk_object_set_data (GTK_OBJECT (window3), "fixed1", fixed1);
    gtk_widget_show (fixed1);
    gtk_box_pack_start (GTK_BOX (vbox1), fixed1, TRUE, TRUE, 0);

    entry3 = gtk_entry_new ();
    gtk_object_set_data (GTK_OBJECT (window3), "entry3", entry3);
    gtk_widget_show (entry3);
    gtk_fixed_put (GTK_FIXED (fixed1), entry3, 16, 48);
    gtk_widget_set_uposition (entry3, 16, 48);
    gtk_widget_set_usize (entry3, 232, 24);
    gtk_entry_set_text(GTK_ENTRY(entry3),pcom);

    pixmap2 = create_misc_pixmap (PRINTER_PIXMAP);
    if (pixmap2 == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    gtk_object_set_data (GTK_OBJECT (window3), "pixmap2", pixmap2);
    gtk_widget_show (pixmap2);
    gtk_fixed_put (GTK_FIXED (fixed1), pixmap2, 304, 40);
    gtk_widget_set_uposition (pixmap2, 304, 40);
    gtk_widget_set_usize (pixmap2, 21, 21);

    label1 = gtk_label_new ("Printer Command:");
    gtk_object_set_data (GTK_OBJECT (window3), "label1", label1);
    gtk_widget_show (label1);
    gtk_fixed_put (GTK_FIXED (fixed1), label1, 8, 24);
    gtk_widget_set_uposition (label1, 16, 24);
    /* gtk_widget_set_usize (label1, 120, 16); */

    toolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_object_set_data (GTK_OBJECT (window3), "toolbar1", toolbar1);
    gtk_widget_show (toolbar1);
    gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, TRUE, 7);
    gtk_container_border_width (GTK_CONTAINER (toolbar1), 11);
    gtk_toolbar_set_space_size (GTK_TOOLBAR (toolbar1), 0);

    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_PRINT);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button13 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
					   GTK_TOOLBAR_CHILD_BUTTON,
					   NULL,
					   "Print",
					   NULL, NULL,
					   tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (window3), "button13", button13);
    gtk_widget_show (button13);
    gtk_container_border_width (GTK_CONTAINER (button13), 6);

    tmp_toolbar_icon = create_main_pixmap (MAINTOOLBAR_QUIT);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button14 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
					   GTK_TOOLBAR_CHILD_BUTTON,
					   NULL,
					   "Cancel",
					   NULL, NULL,
					   tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (window3), "button14", button14);
    gtk_widget_show (button14);
    gtk_container_border_width (GTK_CONTAINER (button14), 6);

    gtk_signal_connect (GTK_OBJECT (button14), "clicked",
			GTK_SIGNAL_FUNC (on_cancel_button_clicked),
			(gpointer)window3);
    gtk_signal_connect (GTK_OBJECT (button13), "clicked",
			GTK_SIGNAL_FUNC (on_ok_button_clicked),
			(gpointer)window3);
    gtk_signal_connect(GTK_OBJECT(window3), "destroy",
                       GTK_SIGNAL_FUNC(on_dialog_destroyed),
                       (gpointer)window3);

    return window3;
}

char *
printwin(
    char *msg,
    ...)
{
    va_list     args;
    char        *msgf;
    GtkWidget   *dialog;
    char        *response=0;

    va_start(args, msg);
    msgf = vmsgfmt(msg, args);
    va_end(args);

    dialog = create_printwin(msgf, &response);
    g_free(msgf);
    gtk_widget_show(dialog);
    gtk_main();

    return response;
}

