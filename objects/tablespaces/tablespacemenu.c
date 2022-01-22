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

#include <gtk/gtk.h>
#include "common-files/common.h"
#include "include/objectman.h"
#include "tablespacefuncs.h"
#include "objects/tables/tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

GtkWidget*
create_otsmenu (
    gpointer data)
{
    GtkWidget *tsmenu;
    GtkAccelGroup *tsmenu_accels;
    GtkWidget *ts_refresh;
    guint tmp_key;
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    tsmenu = gtk_menu_new ();
    gtk_object_set_data (GTK_OBJECT (tsmenu), "tsmenu", tsmenu);
    tsmenu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (tsmenu));

    ts_refresh = gtk_menu_item_new_with_label ("");
    tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ts_refresh)->child),
				     ("_Refresh"));
    gtk_widget_add_accelerator (ts_refresh, "activate_item", tsmenu_accels,
				tmp_key, 0, 0);
    gtk_widget_ref (ts_refresh);
    gtk_object_set_data_full (GTK_OBJECT (tsmenu), "ts_refresh", ts_refresh,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ts_refresh);
    gtk_container_add (GTK_CONTAINER (tsmenu), ts_refresh);
    gtk_tooltips_set_tip (tooltips, ts_refresh, ("Refresh the tablespace info display"), NULL);

    gtk_signal_connect (GTK_OBJECT (ts_refresh), "activate",
			GTK_SIGNAL_FUNC (on_ts_refresh_activate),
			data);

    gtk_object_set_data (GTK_OBJECT (tsmenu), "tooltips", tooltips);

    return tsmenu;
}


void
on_ts_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}


