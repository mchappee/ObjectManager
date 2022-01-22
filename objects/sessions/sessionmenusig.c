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
#include <gdk/gdkkeysyms.h>

#include "include/objectmansrc.h"
#include "include/toolbarsig.h"
#include "sessionfuncs.h"

#include <stdio.h>
#include <assert.h>


void
on_ses_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    /* reselecting the row will cause a refresh of the 
       session info */
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}

void
on_ses_all_refresh_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;

    /* reselecting the row will cause a refresh of the 
       session info */
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);

}


void
on_ses_kill_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);
    obj_on_sesbutton5_clicked(NULL,NULL);
}


void
on_ses_disconnect_activate(
    GtkMenuItem     *menuitem,
    gpointer         user_data)
{
    DirNode *dir = (DirNode *)user_data;
    gtk_clist_select_row (GTK_CLIST(dir->tree), dir->row, 0);
    obj_on_sesbutton4_clicked(NULL,NULL);}


