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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "include/objectmansrc.h"
#include "sessionfuncs.h"

GtkWidget*
create_sessionmenu (
    gpointer	   data)
{
    GtkWidget	   *sessionmenu;
    GtkAccelGroup  *sessionmenu_accels;
    guint	   tmp_key;
    GtkWidget	   *ses_refresh;
    GtkWidget	   *ses_kill;
    GtkWidget	   *ses_disconnect;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    sessionmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (sessionmenu), "sessionmenu", sessionmenu);
    sessionmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (sessionmenu));

    ses_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ses_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (ses_refresh, "activate_item", sessionmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ses_refresh);
    gtk_object_set_data_full (GTK_OBJECT (sessionmenu), "ses_refresh", ses_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ses_refresh);
    gtk_container_add (GTK_CONTAINER (sessionmenu), ses_refresh);
    gtk_tooltips_set_tip (tooltips, ses_refresh, _("Refresh the session info display"), NULL);

    ses_kill = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ses_kill)->child),
				     _("_Kill"));
    gtk_widget_add_accelerator (ses_kill, "activate_item", sessionmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ses_kill);
    gtk_object_set_data_full (GTK_OBJECT (sessionmenu), "ses_kill", ses_kill,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ses_kill);
    gtk_container_add (GTK_CONTAINER (sessionmenu), ses_kill);
    gtk_tooltips_set_tip (tooltips, ses_kill, _("Kill this user's session"), NULL);

    ses_disconnect = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ses_disconnect)->child),
				     _("_Disconnect"));
    gtk_widget_add_accelerator (ses_disconnect, "activate_item", sessionmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ses_disconnect);
    gtk_object_set_data_full (GTK_OBJECT (sessionmenu), "ses_disconnect", ses_disconnect,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ses_disconnect);
    gtk_container_add (GTK_CONTAINER (sessionmenu), ses_disconnect);
    gtk_tooltips_set_tip (tooltips, ses_disconnect, _("Disconnect session from database"), NULL);

    gtk_signal_connect (GTK_OBJECT (ses_refresh), "activate",
			GTK_SIGNAL_FUNC (on_ses_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (ses_kill), "activate",
			GTK_SIGNAL_FUNC (on_ses_kill_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (ses_disconnect), "activate",
			GTK_SIGNAL_FUNC (on_ses_disconnect_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (sessionmenu), "tooltips", tooltips);

    return sessionmenu;
}

GtkWidget*
create_all_sessionmenu (
    gpointer	   data)
{
    GtkWidget	   *sessionmenu;
    GtkAccelGroup  *sessionmenu_accels;
    guint	   tmp_key;
    GtkWidget	   *ses_refresh;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    sessionmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (sessionmenu), "sessionmenu", sessionmenu);
    sessionmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (sessionmenu));

    ses_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ses_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (ses_refresh, "activate_item", sessionmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ses_refresh);
    gtk_object_set_data_full (GTK_OBJECT (sessionmenu), "ses_refresh", ses_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ses_refresh);
    gtk_container_add (GTK_CONTAINER (sessionmenu), ses_refresh);
    gtk_tooltips_set_tip (tooltips, ses_refresh, _("Refresh the session info display"), NULL);

    gtk_signal_connect (GTK_OBJECT (ses_refresh), "activate",
			GTK_SIGNAL_FUNC (on_ses_all_refresh_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (sessionmenu), "tooltips", tooltips);

    return sessionmenu;
}

