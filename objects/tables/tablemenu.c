/* ObjectManager - The OraSoft Oracle DBA Tool
 *
 * Copyright ? Matthew Chappee <matthew@orasoft.org>
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
/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "include/objectman.h"
#include "tablefuncs.h"

GtkWidget*
create_tablemenu (
    gpointer	   data)
{
    GtkWidget	   *tablemenu;
    GtkAccelGroup  *tablemenu_accels;
    guint	   tmp_key;
    GtkWidget	   *tab_refresh;
    GtkWidget	   *tab_create;
    GtkWidget	   *tab_delete;
    GtkWidget	   *tab_rename;
    GtkWidget	   *tab_clear;
    GtkWidget	   *tab_separator1;
    GtkWidget	   *tab_separator2;
    GtkWidget	   *tab_export;
    GtkWidget      *tab_grant_privilege;
    GtkWidget	   *tab_export_menu;
    GtkAccelGroup  *tab_export_menu_accels;
    GtkWidget	   *tab_export_csv;
    GtkWidget	   *tab_export_html;
    GtkWidget	   *tab_export_sql;
    GtkWidget	   *dl_export;  
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    tablemenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (tablemenu), "tablemenu", tablemenu);
    tablemenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (tablemenu));

    tab_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (tab_refresh, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_refresh);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_refresh", tab_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_refresh);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_refresh);
    gtk_tooltips_set_tip (tooltips, tab_refresh, _("Refresh the table info display"), NULL);

    tab_create = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_create)->child),
				     _("_Create"));
    gtk_widget_add_accelerator (tab_create, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_create);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_create", tab_create,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_create);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_create);
    gtk_tooltips_set_tip (tooltips, tab_create, _("Create new table"), NULL);

    tab_delete = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_delete)->child),
				     _("_Drop"));
    gtk_widget_add_accelerator (tab_delete, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_delete);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_delete", tab_delete,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_delete);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_delete);
    gtk_tooltips_set_tip (tooltips, tab_delete, _("Drop table from database"), NULL);

    tab_rename = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_rename)->child),
				     _("Re_name"));
    gtk_widget_add_accelerator (tab_rename, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_rename);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_rename", tab_rename,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_rename);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_rename);
    gtk_tooltips_set_tip (tooltips, tab_rename, _("Rename table"), NULL);

    tab_clear = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_clear)->child),
				     _("T_runcate"));
    gtk_widget_add_accelerator (tab_clear, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_clear);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_clear", tab_clear,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_clear);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_clear);
    gtk_tooltips_set_tip (tooltips, tab_clear, _("Truncate table"), NULL);

    tab_separator1 = gtk_menu_item_new ();
    gtk_widget_ref (tab_separator1);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_separator1", tab_separator1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_separator1);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_separator1);
    gtk_widget_set_sensitive (tab_separator1, FALSE);

    tab_export = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_export)->child),
				     _("_Export"));
    gtk_widget_add_accelerator (tab_export, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_export);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_export", tab_export,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_export);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_export);
    gtk_tooltips_set_tip (tooltips, tab_export, _("Export data from table"), NULL);

    tab_export_menu = gtk_menu_new ();
    gtk_widget_ref (tab_export_menu);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_export_menu", tab_export_menu,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (tab_export), tab_export_menu);
    tab_export_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (tab_export_menu));

    tab_export_csv = gtk_menu_item_new_with_label (_("csv"));
    gtk_widget_ref (tab_export_csv);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_export_csv", tab_export_csv,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_export_csv);
    gtk_container_add (GTK_CONTAINER (tab_export_menu), tab_export_csv);

    tab_export_html = gtk_menu_item_new_with_label (_("html"));
    gtk_widget_ref (tab_export_html);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_export_html", tab_export_html,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_export_html);
    gtk_container_add (GTK_CONTAINER (tab_export_menu), tab_export_html);

    tab_export_sql = gtk_menu_item_new_with_label (_("sql"));
    gtk_widget_ref (tab_export_sql);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_export_sql", tab_export_sql,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_export_sql);
    gtk_container_add (GTK_CONTAINER (tab_export_menu), tab_export_sql);


    if(obj_ginfo.dynamicexport){
        int    i;
        for (i = 0; i < obj_ginfo.total_dlinfo; i++) {
	    dl_export = gtk_menu_item_new_with_label (_(dl_info[i].dl_menuitem));
	    gtk_widget_ref (dl_export);
	    gtk_object_set_data_full (GTK_OBJECT (dl_export), "dl_id", (gpointer)dl_info[i].dl_id,
			      (GtkDestroyNotify) gtk_widget_unref);
	    gtk_widget_show (dl_export);
	    gtk_container_add (GTK_CONTAINER (tab_export_menu), dl_export);
	    gtk_signal_connect (GTK_OBJECT (dl_export), "activate",
			GTK_SIGNAL_FUNC (on_dl_export_activate),
			data);
	}
    }

    tab_separator2 = gtk_menu_item_new ();
    gtk_widget_ref (tab_separator2);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_separator2", tab_separator2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_separator2);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_separator2);
    gtk_widget_set_sensitive (tab_separator2, FALSE);

    tab_grant_privilege = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_grant_privilege)->child),
				     _("G_rant Privilege"));
    gtk_widget_add_accelerator (tab_grant_privilege, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_grant_privilege);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_grant_privilege", tab_grant_privilege,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_grant_privilege);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_grant_privilege);
    gtk_tooltips_set_tip (tooltips, tab_grant_privilege, _("Grant Privilege On This Table"), NULL);


/*    tab_script = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_script)->child),
				     _("_Script"));
    gtk_widget_add_accelerator (tab_script, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_script);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_script", tab_script,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_script);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_script);
    gtk_tooltips_set_tip (tooltips, tab_script, _("Save SQL definition of table to file"), NULL); */

    gtk_signal_connect (GTK_OBJECT (tab_refresh), "activate",
			GTK_SIGNAL_FUNC (on_tab_refresh_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (tab_create), "activate",
			GTK_SIGNAL_FUNC (on_tab_create_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (tab_delete), "activate",
			GTK_SIGNAL_FUNC (on_tab_delete_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (tab_rename), "activate",
			GTK_SIGNAL_FUNC (on_tab_rename_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (tab_clear), "activate",
			GTK_SIGNAL_FUNC (on_tab_clear_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (tab_export_csv), "activate",
			GTK_SIGNAL_FUNC (on_tab_export_csv_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (tab_export_html), "activate",
			GTK_SIGNAL_FUNC (on_tab_export_html_activate),
			data);
    gtk_signal_connect (GTK_OBJECT (tab_export_sql), "activate",
			GTK_SIGNAL_FUNC (on_tab_export_sql_activate),
			data);
/*    gtk_signal_connect (GTK_OBJECT (tab_script), "activate",
			GTK_SIGNAL_FUNC (on_tab_script_activate),
			data);*/
    gtk_signal_connect (GTK_OBJECT (tab_grant_privilege), "activate",
			GTK_SIGNAL_FUNC (on_tab_grant_privilege_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (tablemenu), "tooltips", tooltips);

    return tablemenu;
}

GtkWidget*
create_all_tablemenu (
    gpointer	   data)
{
    GtkWidget	   *tablemenu;
    GtkAccelGroup  *tablemenu_accels;
    guint	   tmp_key;
    GtkWidget	   *tab_refresh;
    GtkTooltips	   *tooltips;

    tooltips = gtk_tooltips_new ();

    tablemenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (tablemenu), "tablemenu", tablemenu);
    tablemenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (tablemenu));

    tab_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (tab_refresh)->child),
				     _("_Refresh"));
    gtk_widget_add_accelerator (tab_refresh, "activate_item", tablemenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (tab_refresh);
    gtk_object_set_data_full (GTK_OBJECT (tablemenu), "tab_refresh", tab_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tab_refresh);
    gtk_container_add (GTK_CONTAINER (tablemenu), tab_refresh);
    gtk_tooltips_set_tip (tooltips, tab_refresh, _("Refresh the table info display"), NULL);


    gtk_signal_connect (GTK_OBJECT (tab_refresh), "activate",
			GTK_SIGNAL_FUNC (on_tab_all_refresh_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (tablemenu), "tooltips", tooltips);

    return tablemenu;
}
