/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright � Matthew Chappee <matthew@orasoft.org>
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
#include "datafilefuncs.h"

GtkWidget*
create_datafilemenu (
    gpointer data)
{
    GtkWidget *datafilemenu;
    GtkAccelGroup *datafilemenu_accels;
    guint tmp_key;
    GtkWidget *dta_refresh;
    GtkWidget *dta_create;
/*    GtkWidget *dta_rename;*/
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    datafilemenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (datafilemenu), "datafilemenu", datafilemenu);
    datafilemenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (datafilemenu));

    dta_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dta_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (dta_refresh, "activate_item", datafilemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dta_refresh);
    gtk_object_set_data_full (GTK_OBJECT (datafilemenu), "dta_refresh", dta_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dta_refresh);
    gtk_container_add (GTK_CONTAINER (datafilemenu), dta_refresh);
    gtk_tooltips_set_tip (tooltips, dta_refresh, _("Refresh the datafile info display"), NULL);

    dta_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dta_create)->child),
				     _("_Create"));
    gtk_widget_add_accelerator (dta_create, "activate_item", datafilemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dta_create);
    gtk_object_set_data_full (GTK_OBJECT (datafilemenu), "dta_create", dta_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dta_create);
    gtk_container_add (GTK_CONTAINER (datafilemenu), dta_create);
    gtk_tooltips_set_tip (tooltips, dta_create, _("Create new datafile"), NULL);
/*
    dta_rename = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dta_rename)->child),
				     _("Re_name"));
    gtk_widget_add_accelerator (dta_rename, "activate_item", datafilemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dta_rename);
    gtk_object_set_data_full (GTK_OBJECT (datafilemenu), "dta_rename", dta_rename,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dta_rename);
    gtk_container_add (GTK_CONTAINER (datafilemenu), dta_rename);
    gtk_tooltips_set_tip (tooltips, dta_rename, _("Rename datafile"), NULL);*/

    gtk_signal_connect (GTK_OBJECT (dta_refresh), "activate",
			GTK_SIGNAL_FUNC (on_dta_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (dta_create), "activate",
			GTK_SIGNAL_FUNC (on_dta_create_activate),
			data);/*
    gtk_signal_connect (GTK_OBJECT (dta_rename), "activate",
			GTK_SIGNAL_FUNC (on_dta_rename_activate),
			data);*/

    gtk_object_set_data (GTK_OBJECT (datafilemenu), "tooltips", tooltips);

    return datafilemenu;
}

GtkWidget*
create_all_datafilemenu (
    gpointer data)
{
    GtkWidget *datafilemenu;
    GtkAccelGroup *datafilemenu_accels;
    guint tmp_key;
    GtkWidget *dta_refresh;
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    datafilemenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (datafilemenu), "datafilemenu", datafilemenu);
    datafilemenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (datafilemenu));

    dta_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dta_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (dta_refresh, "activate_item", datafilemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dta_refresh);
    gtk_object_set_data_full (GTK_OBJECT (datafilemenu), "dta_refresh", dta_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dta_refresh);
    gtk_container_add (GTK_CONTAINER (datafilemenu), dta_refresh);
    gtk_tooltips_set_tip (tooltips, dta_refresh, _("Refresh the datafile info display"), NULL);


    gtk_signal_connect (GTK_OBJECT (dta_refresh), "activate",
			GTK_SIGNAL_FUNC (on_dta_all_refresh_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (datafilemenu), "tooltips", tooltips);

    return datafilemenu;
}

