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
#include "synonymfuncs.h"

GtkWidget*
create_synonymmenu (
    gpointer	   data)
{
    GtkWidget	   *synonymmenu;
    GtkAccelGroup  *synonymmenu_accels;
    guint	   tmp_key;
    GtkWidget	   *syn_refresh;
    GtkWidget	   *syn_create;
    GtkWidget	   *syn_delete;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    synonymmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (synonymmenu), "synonymmenu", synonymmenu);
    synonymmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (synonymmenu));

    syn_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (syn_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (syn_refresh, "activate_item", synonymmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (syn_refresh);
    gtk_object_set_data_full (GTK_OBJECT (synonymmenu), "syn_refresh", syn_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (syn_refresh);
    gtk_container_add (GTK_CONTAINER (synonymmenu), syn_refresh);
    gtk_tooltips_set_tip (tooltips, syn_refresh, _("Refresh the synonym info display"), NULL);

    syn_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (syn_create)->child),
				     _("_Create"));
    gtk_widget_add_accelerator (syn_create, "activate_item", synonymmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (syn_create);
    gtk_object_set_data_full (GTK_OBJECT (synonymmenu), "syn_create", syn_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (syn_create);
    gtk_container_add (GTK_CONTAINER (synonymmenu), syn_create);
    gtk_tooltips_set_tip (tooltips, syn_create, _("Create new synonym"), NULL);

    syn_delete = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (syn_delete)->child),
				     _("_Drop"));
    gtk_widget_add_accelerator (syn_delete, "activate_item", synonymmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (syn_delete);
    gtk_object_set_data_full (GTK_OBJECT (synonymmenu), "syn_delete", syn_delete,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (syn_delete);
    gtk_container_add (GTK_CONTAINER (synonymmenu), syn_delete);
    gtk_tooltips_set_tip (tooltips, syn_delete, _("Drop synonym from database"), NULL);


    gtk_signal_connect (GTK_OBJECT (syn_refresh), "activate",
			GTK_SIGNAL_FUNC (on_syn_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (syn_create), "activate",
			GTK_SIGNAL_FUNC (on_syn_create_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (syn_delete), "activate",
			GTK_SIGNAL_FUNC (on_syn_delete_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (synonymmenu), "tooltips", tooltips);

    return synonymmenu;
}

GtkWidget*
create_all_synonymmenu (
    gpointer	   data)
{
    GtkWidget	   *synonymmenu;
    GtkAccelGroup  *synonymmenu_accels;
    guint	   tmp_key;
    GtkWidget	   *syn_refresh;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    synonymmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (synonymmenu), "synonymmenu", synonymmenu);
    synonymmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (synonymmenu));

    syn_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (syn_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (syn_refresh, "activate_item", synonymmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (syn_refresh);
    gtk_object_set_data_full (GTK_OBJECT (synonymmenu), "syn_refresh", syn_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (syn_refresh);
    gtk_container_add (GTK_CONTAINER (synonymmenu), syn_refresh);
    gtk_tooltips_set_tip (tooltips, syn_refresh, _("Refresh the synonym info display"), NULL);


    gtk_signal_connect (GTK_OBJECT (syn_refresh), "activate",
			GTK_SIGNAL_FUNC (on_syn_all_refresh_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (synonymmenu), "tooltips", tooltips);

    return synonymmenu;
}

