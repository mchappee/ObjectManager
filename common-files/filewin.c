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
#include "common.h"

static void
on_gfs_ok_button_clicked(
    GtkButton	      *button,
    gpointer	      user_data)
{
    GtkFileSelection  *fs      = GTK_FILE_SELECTION(user_data);
  
    char **response = (char **)gtk_object_get_data (GTK_OBJECT (user_data), 
						    "response");
    
    *response = g_strdup(gtk_file_selection_get_filename(fs));

    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}

static void
on_gfs_dialog_destroyed(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}

static void
on_gfs_cancel_button_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}

static GtkWidget*
create_generic_fileselection (
    char	 *filemsg,
    char	 **response)
{
    GtkWidget	 *fileselection1;
    GtkWidget	 *ok_button1;
    GtkWidget	 *cancel_button1;

    fileselection1 = gtk_file_selection_new ((filemsg));
    gtk_object_set_data (GTK_OBJECT (fileselection1), "fileselection1", fileselection1);
    gtk_container_set_border_width (GTK_CONTAINER (fileselection1), 10);
    /*gtk_window_set_modal (GTK_WINDOW (fileselection1), TRUE);*/
    gtk_window_set_position (GTK_WINDOW (fileselection1), GTK_WIN_POS_CENTER);
  
    gtk_object_set_data (GTK_OBJECT (fileselection1), "response", response);

    ok_button1 = GTK_FILE_SELECTION (fileselection1)->ok_button;
    gtk_widget_show (ok_button1);
    GTK_WIDGET_SET_FLAGS (ok_button1, GTK_CAN_DEFAULT);

    cancel_button1 = GTK_FILE_SELECTION (fileselection1)->cancel_button;
    gtk_widget_show (cancel_button1);
    GTK_WIDGET_SET_FLAGS (cancel_button1, GTK_CAN_DEFAULT);

    gtk_signal_connect (GTK_OBJECT (ok_button1), "clicked",
			GTK_SIGNAL_FUNC (on_gfs_ok_button_clicked),
			(gpointer) fileselection1);
    gtk_signal_connect (GTK_OBJECT (cancel_button1), "clicked",
			GTK_SIGNAL_FUNC (on_gfs_cancel_button_clicked),
			(gpointer) fileselection1);
    gtk_signal_connect(GTK_OBJECT(fileselection1), "destroy",
                       GTK_SIGNAL_FUNC(on_gfs_dialog_destroyed),
                       (gpointer)fileselection1);

    return fileselection1;
}

char *
filewin(
    char *msg,
    ...)
{
    va_list	 args;
    char	 *msgf;
    GtkWidget	 *dialog;
    char	 *response=0;

    va_start(args, msg);
    msgf = vmsgfmt(msg, args);
    va_end(args);
    
    dialog = create_generic_fileselection(msgf, &response);
    g_free(msgf);
    gtk_widget_show(dialog);
    gtk_main();

    return response;
}

char *
filewin_x(
    char *extension,
    char *msg,
    ...)
{
    va_list	 args;
    char	 *msgf;
    GtkWidget	 *dialog;
    char	 *response=0;

    va_start(args, msg);
    msgf = vmsgfmt(msg, args);
    va_end(args);
    
    dialog = create_generic_fileselection(msgf, &response);
    gtk_file_selection_complete (GTK_FILE_SELECTION(dialog), extension);
    g_free(msgf);
    gtk_widget_show(dialog);
    gtk_main();

    return response;
}

