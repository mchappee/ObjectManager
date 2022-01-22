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
#include "sequencefuncs.h"

GtkWidget*
create_sequencemenu (
    gpointer	   data)
{
    GtkWidget	   *sequencemenu;
    GtkAccelGroup  *sequencemenu_accels;
    guint	   tmp_key;
    GtkWidget	   *seq_refresh;
    GtkWidget	   *seq_create;
    GtkWidget	   *seq_delete;
    GtkTooltips	   *tooltips;
    GtkWidget	   *seq_grant_privilege;
    GtkWidget	   *seq_separator2;

    tooltips = gtk_tooltips_new ();

    sequencemenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (sequencemenu), "sequencemenu", sequencemenu);
    sequencemenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (sequencemenu));

    seq_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (seq_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (seq_refresh, "activate_item", sequencemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (seq_refresh);
    gtk_object_set_data_full (GTK_OBJECT (sequencemenu), "seq_refresh", seq_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seq_refresh);
    gtk_container_add (GTK_CONTAINER (sequencemenu), seq_refresh);
    gtk_tooltips_set_tip (tooltips, seq_refresh, _("Refresh the sequence info display"), NULL);

    seq_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (seq_create)->child),
				     _("_Create"));
    gtk_widget_add_accelerator (seq_create, "activate_item", sequencemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (seq_create);
    gtk_object_set_data_full (GTK_OBJECT (sequencemenu), "seq_create", seq_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seq_create);
    gtk_container_add (GTK_CONTAINER (sequencemenu), seq_create);
    gtk_tooltips_set_tip (tooltips, seq_create, _("Create new sequence"), NULL);

    seq_delete = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (seq_delete)->child),
				     _("_Drop"));
    gtk_widget_add_accelerator (seq_delete, "activate_item", sequencemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (seq_delete);
    gtk_object_set_data_full (GTK_OBJECT (sequencemenu), "seq_delete", seq_delete,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seq_delete);
    gtk_container_add (GTK_CONTAINER (sequencemenu), seq_delete);
    gtk_tooltips_set_tip (tooltips, seq_delete, _("Drop sequence from database"), NULL);

    seq_separator2 = gtk_menu_item_new ();
    gtk_widget_ref (seq_separator2);
    gtk_object_set_data_full (GTK_OBJECT (sequencemenu), "seq_separator2", seq_separator2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seq_separator2);
    gtk_container_add (GTK_CONTAINER (sequencemenu), seq_separator2);
    gtk_widget_set_sensitive (seq_separator2, FALSE);

    seq_grant_privilege = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (seq_grant_privilege)->child),
				     _("G_rant Privilege"));
    gtk_widget_add_accelerator (seq_grant_privilege, "activate_item", sequencemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (seq_grant_privilege);
    gtk_object_set_data_full (GTK_OBJECT (sequencemenu), "seq_grant_privilege", seq_grant_privilege,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (seq_grant_privilege);
    gtk_container_add (GTK_CONTAINER (sequencemenu), seq_grant_privilege);
    gtk_tooltips_set_tip (tooltips, seq_grant_privilege, _("Grant Privilege On This Table"), NULL);

    gtk_signal_connect (GTK_OBJECT (seq_refresh), "activate",
			GTK_SIGNAL_FUNC (on_seq_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (seq_create), "activate",
			GTK_SIGNAL_FUNC (on_seq_create_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (seq_delete), "activate",
			GTK_SIGNAL_FUNC (on_seq_delete_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (seq_grant_privilege), "activate",
			GTK_SIGNAL_FUNC (on_seq_grant_privilege_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (sequencemenu), "tooltips", tooltips);

    return sequencemenu;
}


