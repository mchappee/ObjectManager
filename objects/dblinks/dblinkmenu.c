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
#include "dblinkfuncs.h"

GtkWidget*
create_dblinkmenu (
    gpointer data)
{
    GtkWidget *dblinkmenu;
    GtkAccelGroup *dblinkmenu_accels;
    guint tmp_key;
    GtkWidget *dbl_refresh;
    GtkWidget *dbl_create;
    GtkWidget *dbl_delete;
    GtkWidget *dbl_test;
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    dblinkmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (dblinkmenu), "dblinkmenu", dblinkmenu);
    dblinkmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (dblinkmenu));

    dbl_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dbl_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (dbl_refresh, "activate_item", dblinkmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dbl_refresh);
    gtk_object_set_data_full (GTK_OBJECT (dblinkmenu), "dbl_refresh", dbl_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dbl_refresh);
    gtk_container_add (GTK_CONTAINER (dblinkmenu), dbl_refresh);
    gtk_tooltips_set_tip (tooltips, dbl_refresh, _("Refresh the dblink info display"), NULL);

    dbl_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dbl_create)->child),
				     _("_Create"));
    gtk_widget_add_accelerator (dbl_create, "activate_item", dblinkmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dbl_create);
    gtk_object_set_data_full (GTK_OBJECT (dblinkmenu), "dbl_create", dbl_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dbl_create);
    gtk_container_add (GTK_CONTAINER (dblinkmenu), dbl_create);
    gtk_tooltips_set_tip (tooltips, dbl_create, _("Create new dblink"), NULL);

    dbl_delete = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dbl_delete)->child),
				     _("_Drop"));
    gtk_widget_add_accelerator (dbl_delete, "activate_item", dblinkmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dbl_delete);
    gtk_object_set_data_full (GTK_OBJECT (dblinkmenu), "dbl_delete", dbl_delete,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dbl_delete);
    gtk_container_add (GTK_CONTAINER (dblinkmenu), dbl_delete);
    gtk_tooltips_set_tip (tooltips, dbl_delete, _("Drop dblink from database"), NULL);

    dbl_test = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dbl_test)->child),
				     _("_Test"));
    gtk_widget_add_accelerator (dbl_test, "activate_item", dblinkmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dbl_test);
    gtk_object_set_data_full (GTK_OBJECT (dblinkmenu), "dbl_test", dbl_test,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dbl_test);
    gtk_container_add (GTK_CONTAINER (dblinkmenu), dbl_test);
    gtk_tooltips_set_tip (tooltips, dbl_test, _("Test the DB Link"), NULL);

    gtk_signal_connect (GTK_OBJECT (dbl_refresh), "activate",
			GTK_SIGNAL_FUNC (on_dbl_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (dbl_create), "activate",
			GTK_SIGNAL_FUNC (on_dbl_create_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (dbl_delete), "activate",
			GTK_SIGNAL_FUNC (on_dbl_delete_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (dbl_test), "activate",
			GTK_SIGNAL_FUNC (on_dbl_test_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (dblinkmenu), "tooltips", tooltips);

    return dblinkmenu;
}

GtkWidget*
create_all_dblinkmenu (
    gpointer data)
{
    GtkWidget *dblinkmenu;
    GtkAccelGroup *dblinkmenu_accels;
    guint tmp_key;
    GtkWidget *dbl_refresh;
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    dblinkmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (dblinkmenu), "dblinkmenu", dblinkmenu);
    dblinkmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (dblinkmenu));

    dbl_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (dbl_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (dbl_refresh, "activate_item", dblinkmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (dbl_refresh);
    gtk_object_set_data_full (GTK_OBJECT (dblinkmenu), "dbl_refresh", dbl_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (dbl_refresh);
    gtk_container_add (GTK_CONTAINER (dblinkmenu), dbl_refresh);
    gtk_tooltips_set_tip (tooltips, dbl_refresh, _("Refresh the dblink info display"), NULL);


    gtk_signal_connect (GTK_OBJECT (dbl_refresh), "activate",
			GTK_SIGNAL_FUNC (on_dbl_all_refresh_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (dblinkmenu), "tooltips", tooltips);

    return dblinkmenu;
}

