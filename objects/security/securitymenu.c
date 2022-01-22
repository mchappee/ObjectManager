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
#include "securityfuncs.h"

GtkWidget*
create_securitymenu (
    gpointer data)
{
    GtkWidget *indexmenu;
    GtkAccelGroup *indexmenu_accels;
    guint tmp_key;
    GtkWidget *ndx_refresh;
    GtkWidget *ndx_create;
    GtkWidget *ndx_delete;
/*    GtkWidget *ndx_rename;
    GtkWidget *ndx_script;*/
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    indexmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (indexmenu), "indexmenu", indexmenu);
    indexmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (indexmenu));

    ndx_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ndx_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (ndx_refresh, "activate_item", indexmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ndx_refresh);
    gtk_object_set_data_full (GTK_OBJECT (indexmenu), "ndx_refresh", ndx_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ndx_refresh);
    gtk_container_add (GTK_CONTAINER (indexmenu), ndx_refresh);
    gtk_tooltips_set_tip (tooltips, ndx_refresh, _("Refresh the index info display"), NULL);

    ndx_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ndx_create)->child),
				     _("_Create"));
    gtk_widget_add_accelerator (ndx_create, "activate_item", indexmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ndx_create);
    gtk_object_set_data_full (GTK_OBJECT (indexmenu), "ndx_create", ndx_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ndx_create);
    gtk_container_add (GTK_CONTAINER (indexmenu), ndx_create);
    gtk_tooltips_set_tip (tooltips, ndx_create, _("Create new index"), NULL);

    ndx_delete = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ndx_delete)->child),
				     _("_Drop"));
    gtk_widget_add_accelerator (ndx_delete, "activate_item", indexmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ndx_delete);
    gtk_object_set_data_full (GTK_OBJECT (indexmenu), "ndx_delete", ndx_delete,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ndx_delete);
    gtk_container_add (GTK_CONTAINER (indexmenu), ndx_delete);
    gtk_tooltips_set_tip (tooltips, ndx_delete, _("Drop index from database"), NULL);

/*    ndx_rename = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ndx_rename)->child),
				     _("Re_name"));
    gtk_widget_add_accelerator (ndx_rename, "activate_item", indexmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ndx_rename);
    gtk_object_set_data_full (GTK_OBJECT (indexmenu), "ndx_rename", ndx_rename,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ndx_rename);
    gtk_container_add (GTK_CONTAINER (indexmenu), ndx_rename);
    gtk_tooltips_set_tip (tooltips, ndx_rename, _("Rename index"), NULL);

    ndx_script = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ndx_script)->child),
				     _("_Script"));
    gtk_widget_add_accelerator (ndx_script, "activate_item", indexmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ndx_script);
    gtk_object_set_data_full (GTK_OBJECT (indexmenu), "ndx_script", ndx_script,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ndx_script);
    gtk_container_add (GTK_CONTAINER (indexmenu), ndx_script);
    gtk_tooltips_set_tip (tooltips, ndx_script, _("Save SQL definition of index to file"), NULL);*/

    gtk_signal_connect (GTK_OBJECT (ndx_refresh), "activate",
			GTK_SIGNAL_FUNC (on_ndx_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (ndx_create), "activate",
			GTK_SIGNAL_FUNC (on_ndx_create_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (ndx_delete), "activate",
			GTK_SIGNAL_FUNC (on_ndx_delete_activate),
			data);
/*    gtk_signal_connect (GTK_OBJECT (ndx_rename), "activate",
			GTK_SIGNAL_FUNC (on_ndx_rename_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (ndx_script), "activate",
			GTK_SIGNAL_FUNC (on_ndx_script_activate),
			data);*/

    gtk_object_set_data (GTK_OBJECT (indexmenu), "tooltips", tooltips);

    return indexmenu;
}

