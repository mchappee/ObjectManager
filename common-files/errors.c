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
#include <errno.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "xpm/error.xpm"
#include "common.h"

static void
on_ok_button_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
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

static GtkWidget*
create_error_dialog (
    const char *error_level,
    const char *msg)
{
    GdkBitmap	     *error_mask;
    GdkColormap	     *colormap;
    GdkPixmap	     *error_pixmap;
    GtkWidget	     *dialog_action_area1;
    GtkWidget	     *dialog_vbox1;
    GtkWidget	     *error_dialog;
    GtkWidget	     *fixed1;
    GtkWidget	     *label1;
    GtkWidget	     *ok_button;
    GtkWidget	     *pixmap1;
    gchar	     *title   = g_malloc(32 + strlen(error_level));

    error_dialog = gtk_dialog_new ();
    colormap = gtk_widget_get_colormap (error_dialog);
    error_pixmap = 
	gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &error_mask,
					       NULL, proc_message);

    gtk_object_set_data (GTK_OBJECT (error_dialog), "error_dialog", error_dialog);
    sprintf(title, "Object Manager %s", error_level);

    gtk_window_set_title (GTK_WINDOW (error_dialog), title);
    g_free(title);
    gtk_window_set_position (GTK_WINDOW (error_dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (error_dialog), TRUE);
    gtk_window_set_policy (GTK_WINDOW (error_dialog), FALSE, FALSE, FALSE);

    dialog_vbox1 = GTK_DIALOG (error_dialog)->vbox;
    gtk_object_set_data (GTK_OBJECT (error_dialog), "dialog_vbox1", dialog_vbox1);
    gtk_widget_show (dialog_vbox1);

    fixed1 = gtk_fixed_new ();
    gtk_widget_show (fixed1);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), fixed1, TRUE, TRUE, 0);

    label1 = gtk_label_new (msg);
    gtk_widget_show (label1);
    gtk_fixed_put (GTK_FIXED (fixed1), label1, 60, 0);
    gtk_widget_set_uposition (label1, 60, 0);
    gtk_widget_set_usize (label1, -1, 152);
    gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
    gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);
    gtk_misc_set_padding (GTK_MISC (label1), 4, 0);

    if (error_pixmap) {
	pixmap1 = gtk_pixmap_new (error_pixmap, error_mask);
	gdk_pixmap_unref (error_pixmap);
	gdk_bitmap_unref (error_mask);

	gtk_widget_show (pixmap1);
	gtk_fixed_put (GTK_FIXED (fixed1), pixmap1, 16, 64);
	gtk_widget_set_uposition (pixmap1, 16, 64);
	gtk_widget_set_usize (pixmap1, 32, 32);
    }

    dialog_action_area1 = GTK_DIALOG (error_dialog)->action_area;
    gtk_object_set_data (GTK_OBJECT (error_dialog), "dialog_action_area1", dialog_action_area1);
    gtk_widget_show (dialog_action_area1);
    gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

    ok_button = gtk_button_new_with_label ("OK");
    gtk_widget_show (ok_button);
    gtk_box_pack_start (GTK_BOX (dialog_action_area1), ok_button, FALSE, FALSE, 0);

    gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
			GTK_SIGNAL_FUNC (on_ok_button_clicked),
			(gpointer)error_dialog);

    gtk_signal_connect(GTK_OBJECT(error_dialog), "destroy",
                       GTK_SIGNAL_FUNC(on_dialog_destroyed),
                       (gpointer)error_dialog);

    return error_dialog;
}

void
file_open_error(const char *fname, int e, char *rw)
{
    GtkWidget   *error_dialog;
    char        *msg;

    msg = g_malloc(strlen(fname) + strlen(strerror(e)) + 32);
    
    sprintf(msg, "Can't open %s for %s\n%s", fname, rw, strerror(e));
    error_dialog = create_error_dialog("Warning", msg);
    gtk_widget_show(error_dialog);
    gtk_main();
    g_free(msg);
}

void
show_warning_msg(
    const char *msg,
    ...)
{
    va_list     args;
    char        *msgf;
    GtkWidget   *error_dialog;
    
    va_start(args, msg);
    msgf = vmsgfmt(msg, args);
    va_end(args);

    error_dialog = create_error_dialog("Warning", msgf);
    g_free(msgf);
    gtk_widget_show(error_dialog);
    gtk_main();
}

void
fatal_error(
    const char *msg,
    ...)
{
    va_list     args;
    char        *msgf;
    GtkWidget   *error_dialog;

    va_start(args, msg);
    msgf = vmsgfmt(msg, args);
    va_end(args);

    error_dialog = create_error_dialog("Warning", msgf);
    g_free(msgf);
    gtk_widget_show(error_dialog);
    gtk_main();
    if (gtk_main_level() == 0) {
	exit(1);
    } else {
	gtk_main_quit();
    }
}
