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
#include "procfuncs.h"

GtkWidget*
create_procmenu (
    gpointer data)
{
    GtkWidget *procmenu;
    GtkAccelGroup *procmenu_accels;
    guint tmp_key;
    GtkWidget *proc_refresh;
    GtkWidget *proc_create;
    GtkWidget *proc_delete;
    GtkTooltips *tooltips;
    GtkWidget *proc_grant_privilege;
    GtkWidget *proc_separator2;

    tooltips = gtk_tooltips_new ();

    procmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (procmenu), "procmenu", procmenu);
    procmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (procmenu));

    proc_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (proc_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (proc_refresh, "activate_item", procmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (proc_refresh);
    gtk_object_set_data_full (GTK_OBJECT (procmenu), "proc_refresh", proc_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (proc_refresh);
    gtk_container_add (GTK_CONTAINER (procmenu), proc_refresh);
    gtk_tooltips_set_tip (tooltips, proc_refresh, _("Refresh the procedure info display"), NULL);

    proc_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (proc_create)->child),
				     _("_Create"));
    gtk_widget_add_accelerator (proc_create, "activate_item", procmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (proc_create);
    gtk_object_set_data_full (GTK_OBJECT (procmenu), "proc_create", proc_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (proc_create);
    gtk_container_add (GTK_CONTAINER (procmenu), proc_create);
    gtk_tooltips_set_tip (tooltips, proc_create, _("Create new procedure"), NULL);

    proc_delete = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (proc_delete)->child),
				     _("_Drop"));
    gtk_widget_add_accelerator (proc_delete, "activate_item", procmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (proc_delete);
    gtk_object_set_data_full (GTK_OBJECT (procmenu), "proc_delete", proc_delete,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (proc_delete);
    gtk_container_add (GTK_CONTAINER (procmenu), proc_delete);
    gtk_tooltips_set_tip (tooltips, proc_delete, _("Drop procedure from database"), NULL);

    proc_separator2 = gtk_menu_item_new ();
    gtk_widget_ref (proc_separator2);
    gtk_object_set_data_full (GTK_OBJECT (procmenu), "proc_separator2", proc_separator2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (proc_separator2);
    gtk_container_add (GTK_CONTAINER (procmenu), proc_separator2);
    gtk_widget_set_sensitive (proc_separator2, FALSE);

    proc_grant_privilege = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (proc_grant_privilege)->child),
				     _("G_rant Privilege"));
    gtk_widget_add_accelerator (proc_grant_privilege, "activate_item", procmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (proc_grant_privilege);
    gtk_object_set_data_full (GTK_OBJECT (procmenu), "proc_grant_privilege", proc_grant_privilege,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (proc_grant_privilege);
    gtk_container_add (GTK_CONTAINER (procmenu), proc_grant_privilege);
    gtk_tooltips_set_tip (tooltips, proc_grant_privilege, _("Grant Privilege On This Table"), NULL);


    gtk_signal_connect (GTK_OBJECT (proc_refresh), "activate",
			GTK_SIGNAL_FUNC (on_proc_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (proc_create), "activate",
			GTK_SIGNAL_FUNC (on_proc_create_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (proc_delete), "activate",
			GTK_SIGNAL_FUNC (on_proc_delete_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (proc_grant_privilege), "activate",
			GTK_SIGNAL_FUNC (on_proc_grant_privilege_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (procmenu), "tooltips", tooltips);

    return procmenu;
}

