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
#include <stdio.h>
#include <stdlib.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "include/objectman.h"

GtkWidget*
create_otab_menu (
    gpointer data)
{
    GtkWidget *otab_menu;
    GtkAccelGroup *otab_menu_accels;
    guint tmp_key;
    GtkWidget *otab_refresh;
    GtkWidget *otab_create;
/*    GtkWidget *otab_script;*/
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    otab_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (otab_menu), "otab_menu", otab_menu);
    otab_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (otab_menu));

    otab_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (otab_refresh)->child),
				     "_Refresh");
    gtk_widget_add_accelerator (otab_refresh, "activate_item", otab_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (otab_refresh);
    gtk_object_set_data_full (GTK_OBJECT (otab_menu), "otab_refresh", otab_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (otab_refresh);
    gtk_container_add (GTK_CONTAINER (otab_menu), otab_refresh);
    gtk_tooltips_set_tip (tooltips, otab_refresh, "Refresh the list of table owners and tables", NULL);

    otab_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (otab_create)->child),
				     "_Create");
    gtk_widget_add_accelerator (otab_create, "activate_item", otab_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (otab_create);
    gtk_object_set_data_full (GTK_OBJECT (otab_menu), "otab_create", otab_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (otab_create);
    gtk_container_add (GTK_CONTAINER (otab_menu), otab_create);
    gtk_tooltips_set_tip (tooltips, otab_create, "Create a new table", NULL);

/*    otab_script = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (otab_script)->child),
				     "_Script");
    gtk_widget_add_accelerator (otab_script, "activate_item", otab_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (otab_script);
    gtk_object_set_data_full (GTK_OBJECT (otab_menu), "otab_script", otab_script,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (otab_script);
    gtk_container_add (GTK_CONTAINER (otab_menu), otab_script);
    gtk_tooltips_set_tip (tooltips, otab_script, "Generate SQL scripts for all tables in the database", NULL);
*/
    gtk_signal_connect (GTK_OBJECT (otab_refresh), "activate",
			GTK_SIGNAL_FUNC (on_objtype_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (otab_create), "activate",
			GTK_SIGNAL_FUNC (on_otab_create_activate),
			data);
/*    gtk_signal_connect (GTK_OBJECT (otab_script), "activate",
			GTK_SIGNAL_FUNC (on_otab_script_activate),
			data);*/

    gtk_object_set_data (GTK_OBJECT (otab_menu), "tooltips", tooltips);

    return otab_menu;
}

GtkWidget*
create_otablespace_menu (
    gpointer data)
{
    GtkWidget *otab_menu;
    GtkAccelGroup *otab_menu_accels;
    guint tmp_key;
    GtkWidget *otab_refresh;
    GtkWidget *otab_create;
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    otab_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (otab_menu), "otab_menu", otab_menu);
    otab_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (otab_menu));

    otab_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (otab_refresh)->child),
				     "_Refresh");
    gtk_widget_add_accelerator (otab_refresh, "activate_item", otab_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (otab_refresh);
    gtk_object_set_data_full (GTK_OBJECT (otab_menu), "otab_refresh", otab_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (otab_refresh);
    gtk_container_add (GTK_CONTAINER (otab_menu), otab_refresh);
    gtk_tooltips_set_tip (tooltips, otab_refresh, "Refresh the list of tablespaces", NULL);

    otab_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (otab_create)->child),
				     "_Create");
    gtk_widget_add_accelerator (otab_create, "activate_item", otab_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (otab_create);
    gtk_object_set_data_full (GTK_OBJECT (otab_menu), "otab_create", otab_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (otab_create);
    gtk_container_add (GTK_CONTAINER (otab_menu), otab_create);
    gtk_tooltips_set_tip (tooltips, otab_create, "Create a new tablespace", NULL);

    gtk_signal_connect (GTK_OBJECT (otab_refresh), "activate",
			GTK_SIGNAL_FUNC (on_objtype_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (otab_create), "activate",
			GTK_SIGNAL_FUNC (on_otablespace_create_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (otab_menu), "tooltips", tooltips);

    return otab_menu;
}


GtkWidget*
create_oindex_menu (
    gpointer	   data)
{
    GtkWidget	   *oindex_menu;
    GtkAccelGroup  *oindex_menu_accels;
    guint	   tmp_key;
    GtkWidget	   *oindex_refresh;
    GtkWidget	   *oindex_create;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    oindex_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (oindex_menu), "oindex_menu", oindex_menu);
    oindex_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (oindex_menu));

    oindex_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (oindex_refresh)->child),
				     "_Refresh");
    gtk_widget_add_accelerator (oindex_refresh, "activate_item", oindex_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (oindex_refresh);
    gtk_object_set_data_full (GTK_OBJECT (oindex_menu), "oindex_refresh", oindex_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (oindex_refresh);
    gtk_container_add (GTK_CONTAINER (oindex_menu), oindex_refresh);
    gtk_tooltips_set_tip (tooltips, oindex_refresh, "Refresh the list of index owners and indexes", NULL);

    oindex_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (oindex_create)->child),
				     "_Create");
    gtk_widget_add_accelerator (oindex_create, "activate_item", oindex_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (oindex_create);
    gtk_object_set_data_full (GTK_OBJECT (oindex_menu), "oindex_create", oindex_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (oindex_create);
    gtk_container_add (GTK_CONTAINER (oindex_menu), oindex_create);

    gtk_signal_connect (GTK_OBJECT (oindex_refresh), "activate",
			GTK_SIGNAL_FUNC (on_objtype_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (oindex_create), "activate",
			GTK_SIGNAL_FUNC (on_oindex_create_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (oindex_menu), "tooltips", tooltips);

    return oindex_menu;
}

GtkWidget*
create_osynonym_menu (
    gpointer	   data)
{
    GtkWidget	   *osynonym_menu;
    GtkAccelGroup  *osynonym_menu_accels;
    guint	   tmp_key;
    GtkWidget	   *osyn_refresh;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    osynonym_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (osynonym_menu), "osynonym_menu", osynonym_menu);
    osynonym_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (osynonym_menu));

    osyn_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (osyn_refresh)->child),
				     "_Refresh");
    gtk_widget_add_accelerator (osyn_refresh, "activate_item", osynonym_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (osyn_refresh);
    gtk_object_set_data_full (GTK_OBJECT (osynonym_menu), "osyn_refresh", osyn_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (osyn_refresh);
    gtk_container_add (GTK_CONTAINER (osynonym_menu), osyn_refresh);
    gtk_tooltips_set_tip (tooltips, osyn_refresh, "Refresh the list of synonym owners and synonyms", NULL);

    gtk_signal_connect (GTK_OBJECT (osyn_refresh), "activate",
			GTK_SIGNAL_FUNC (on_objtype_refresh_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (osynonym_menu), "tooltips", tooltips);

    return osynonym_menu;
}

GtkWidget*
create_oview_menu (
    gpointer data)
{
    GtkWidget *oview_menu;
    GtkAccelGroup *oview_menu_accels;
    guint tmp_key;
    GtkWidget *oview_refresh;
    GtkWidget *oview_create;
/*    GtkWidget *oview_script;*/
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    oview_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (oview_menu), "oview_menu", oview_menu);
    oview_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (oview_menu));

    oview_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (oview_refresh)->child),
				     "_Refresh");
    gtk_widget_add_accelerator (oview_refresh, "activate_item", oview_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (oview_refresh);
    gtk_object_set_data_full (GTK_OBJECT (oview_menu), "oview_refresh", oview_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (oview_refresh);
    gtk_container_add (GTK_CONTAINER (oview_menu), oview_refresh);
    gtk_tooltips_set_tip (tooltips, oview_refresh, "Refresh the list of view owners and views", NULL);

    oview_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (oview_create)->child),
				     "_Create");
    gtk_widget_add_accelerator (oview_create, "activate_item", oview_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (oview_create);
    gtk_object_set_data_full (GTK_OBJECT (oview_menu), "oview_create", oview_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (oview_create);
    gtk_container_add (GTK_CONTAINER (oview_menu), oview_create);
    gtk_tooltips_set_tip (tooltips, oview_create, "Create a new view in the database", NULL);

/*    oview_script = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (oview_script)->child),
				     "_Script");
    gtk_widget_add_accelerator (oview_script, "activate_item", oview_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (oview_script);
    gtk_object_set_data_full (GTK_OBJECT (oview_menu), "oview_script", oview_script,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (oview_script);
    gtk_container_add (GTK_CONTAINER (oview_menu), oview_script);
    gtk_tooltips_set_tip (tooltips, oview_script, "Generate SQL scripts for database views", NULL);
*/
    gtk_signal_connect (GTK_OBJECT (oview_refresh), "activate",
			GTK_SIGNAL_FUNC (on_objtype_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (oview_create), "activate",
			GTK_SIGNAL_FUNC (on_oview_create_activate),
			NULL);
/*    gtk_signal_connect (GTK_OBJECT (oview_script), "activate",
			GTK_SIGNAL_FUNC (on_oview_script_activate),
			data);
*/
    gtk_object_set_data (GTK_OBJECT (oview_menu), "tooltips", tooltips);

    return oview_menu;
}

GtkWidget*
create_root_menu (
    gpointer	   data)
{
    GtkWidget	   *gowner_menu;
    GtkAccelGroup  *gowner_menu_accels;
    guint	   tmp_key;
    GtkWidget	   *gowner_refresh;
    GtkWidget	   *gowner_disconnect;
    GtkWidget      *gproc_exportall;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    gowner_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (gowner_menu), "gowner_menu", gowner_menu);
    gowner_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (gowner_menu));

    gowner_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (gowner_refresh)->child),
				     "_Refresh");
    gtk_widget_add_accelerator (gowner_refresh, "activate_item", gowner_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (gowner_refresh);
    gtk_object_set_data_full (GTK_OBJECT (gowner_menu), "gowner_refresh", gowner_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (gowner_refresh);
    gtk_container_add (GTK_CONTAINER (gowner_menu), gowner_refresh);
    gtk_tooltips_set_tip (tooltips, gowner_refresh, "Refresh the list of owners", NULL);

    gowner_disconnect = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (gowner_disconnect)->child),
				     "Remove _Tree Item");
    gtk_widget_add_accelerator (gowner_disconnect, "activate_item", gowner_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (gowner_disconnect);
    gtk_object_set_data_full (GTK_OBJECT (gowner_menu), "gowner_disconnect", gowner_disconnect,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (gowner_disconnect);
    gtk_container_add (GTK_CONTAINER (gowner_menu), gowner_disconnect);
    gtk_tooltips_set_tip (tooltips, gowner_disconnect, "Removes the item from the tree.  Duh.", NULL);

    gproc_exportall = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (gproc_exportall)->child),
				     "_Export All Code Objects");
    gtk_widget_add_accelerator (gproc_exportall, "activate_item", gowner_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (gproc_exportall);
    gtk_object_set_data_full (GTK_OBJECT (gowner_menu), "gproc_exportall", gproc_exportall,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (gproc_exportall);
    gtk_container_add (GTK_CONTAINER (gowner_menu), gproc_exportall);
    gtk_tooltips_set_tip (tooltips, gproc_exportall, "Export all code objects to disk", NULL);

    gtk_signal_connect (GTK_OBJECT (gowner_refresh), "activate",
			GTK_SIGNAL_FUNC (on_root_refresh_activate),
			data);

    gtk_signal_connect (GTK_OBJECT (gowner_disconnect), "activate",
			GTK_SIGNAL_FUNC (on_root_disconnect_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (gproc_exportall), "activate",
			GTK_SIGNAL_FUNC (on_oproc_exportall_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (gowner_menu), "tooltips", tooltips);

    return gowner_menu;
}


GtkWidget*
create_generic_owner_menu (
    gpointer	   data)
{
    GtkWidget	   *gowner_menu;
    GtkAccelGroup  *gowner_menu_accels;
    guint	   tmp_key;
    GtkWidget	   *gowner_refresh;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    gowner_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (gowner_menu), "gowner_menu", gowner_menu);
    gowner_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (gowner_menu));

    gowner_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (gowner_refresh)->child),
				     "_Refresh");
    gtk_widget_add_accelerator (gowner_refresh, "activate_item", gowner_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (gowner_refresh);
    gtk_object_set_data_full (GTK_OBJECT (gowner_menu), "gowner_refresh", gowner_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (gowner_refresh);
    gtk_container_add (GTK_CONTAINER (gowner_menu), gowner_refresh);
    gtk_tooltips_set_tip (tooltips, gowner_refresh, "Refresh the list of owners", NULL);

    gtk_signal_connect (GTK_OBJECT (gowner_refresh), "activate",
			GTK_SIGNAL_FUNC (on_gowner_refresh_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (gowner_menu), "tooltips", tooltips);

    return gowner_menu;
}

GtkWidget*
create_oprocedure_menu (
    gpointer	   data)
{
    GtkWidget	   *gproc_menu;
    GtkAccelGroup  *gproc_menu_accels;
    guint	   tmp_key;
    GtkWidget	   *gproc_refresh;
    GtkWidget	   *gproc_exportall;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    gproc_menu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (gproc_menu), "gproc_menu", gproc_menu);
    gproc_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (gproc_menu));

    gproc_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (gproc_refresh)->child),
				     "_Refresh");
    gtk_widget_add_accelerator (gproc_refresh, "activate_item", gproc_menu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (gproc_refresh);
    gtk_object_set_data_full (GTK_OBJECT (gproc_menu), "gproc_refresh", gproc_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (gproc_refresh);
    gtk_container_add (GTK_CONTAINER (gproc_menu), gproc_refresh);
    gtk_tooltips_set_tip (tooltips, gproc_refresh, "Refresh the list of owners", NULL);

    gproc_exportall = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (gproc_exportall)->child),
				     "_Export All");
    gtk_widget_add_accelerator (gproc_exportall, "activate_item", gproc_menu_accels,
				tmp_key, 0, 0);

    gtk_signal_connect (GTK_OBJECT (gproc_refresh), "activate",
			GTK_SIGNAL_FUNC (on_gowner_refresh_activate),
			data);

    gtk_signal_connect (GTK_OBJECT (gproc_exportall), "activate",
			GTK_SIGNAL_FUNC (on_oproc_exportall_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (gproc_menu), "tooltips", tooltips);

    return gproc_menu;
}


