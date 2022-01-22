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
#include "include/objectman.h"
#include "tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void
obj_on_clist10_select_row(
    GtkCList        *clist,
    gint             row,
    gint             column,
    GdkEvent        *event,
    gpointer         user_data)
{
    obj_ginfo.datacrow = row;
    obj_buildsql();
}

void
obj_on_entry10_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    obj_buildsql();
}

void
obj_on_addcolumnwin_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    char *cp = gtk_editable_get_chars(GTK_EDITABLE (obj_addcolumnwin.text10),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (obj_addcolumnwin.text10)));
    
    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
	msgbox("column added");
    }
    
    gtk_widget_destroy(obj_addcolumnwin.addcolumn);
    g_free(cp);
}

void
obj_on_canceladd_clicked(
    GtkButton       *button,
    gpointer         user_data)
{
    gtk_widget_destroy(obj_addcolumnwin.addcolumn);
}       

void
obj_on_entry11_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    obj_buildsql();
}

void
obj_buildsql(void)
{
    GString	 *query;
    char	 *ctype;
    char	 *colname;
    char	 *length  = 0;

    gtk_clist_get_text(GTK_CLIST(obj_addcolumnwin.clist10), obj_ginfo.datacrow, 1, &ctype);

    /* make length insensitive for datatypes which don't need it setting */
    if(strncmp(ctype,"long",4)==0 ||
       strncmp(ctype,"clob",4)==0 ||
       strncmp(ctype,"blob",4)==0 ||
       strncmp(ctype,"date",4)==0) {
	gtk_widget_set_sensitive(obj_addcolumnwin.entry11, FALSE);
    } else {
	length = gtk_entry_get_text(GTK_ENTRY(obj_addcolumnwin.entry11));
	gtk_widget_set_sensitive(obj_addcolumnwin.entry11, TRUE);
    }
  
    colname=gtk_entry_get_text(GTK_ENTRY(obj_addcolumnwin.entry10));

    query = g_string_new("ALTER TABLE ");
    g_string_sprintfa(query, " %s ADD (%s %s",
		      obj_cinfo.currentobject, colname, ctype);
    if (length && strlen(length) > 0) {
	g_string_sprintfa(query, "(%s)", length);
    }
    g_string_append(query, ")");

    clear_text(obj_addcolumnwin.text10);
    gtk_text_insert (GTK_TEXT (obj_addcolumnwin.text10), NULL, NULL, NULL,
		     query->str, query->len);
    g_string_free(query, 1);
}
