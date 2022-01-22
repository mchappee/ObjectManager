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

static void
on_grid_view_click_column(
    GtkCList        *clist,
    gint             column,
    gpointer         user_data)
{
    if (column != clist->sort_column) {
	char	     *title;
	GtkWidget    *hbox;
	GtkWidget    *label;
	GtkWidget    *arrow;

	if (clist->sort_column != -1) {
	    title = g_strdup(gtk_clist_get_column_title (clist, clist->sort_column));
	    gtk_clist_set_column_title (clist, clist->sort_column, title);
	    g_free(title);
	}

	title = g_strdup(gtk_clist_get_column_title (clist, column));

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	label = gtk_label_new(title);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	arrow = gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_IN);
	gtk_widget_show(arrow);
	gtk_object_set_data(GTK_OBJECT(hbox), "arrow", arrow);
	gtk_box_pack_start(GTK_BOX(hbox), arrow, FALSE, FALSE, 2);

	/* There's no gtk_clist_get_column_justification function */
	switch (clist->column[column].justification) {
	case GTK_JUSTIFY_LEFT:
	    gtk_misc_set_alignment (GTK_MISC(label), 0.0, 0.5);
	    break;

	case GTK_JUSTIFY_RIGHT:
	    gtk_misc_set_alignment (GTK_MISC(label), 1.0, 0.5);
	    break;

	case GTK_JUSTIFY_CENTER:
	case GTK_JUSTIFY_FILL:
	    gtk_misc_set_alignment (GTK_MISC(label), 0.5, 0.5);
	}


	gtk_clist_set_column_widget(clist, column, hbox);
	gtk_clist_set_sort_type(clist, GTK_SORT_ASCENDING);
	/* Setting the widget sets the column title to null, however
	   the export functions expect the titles to be there.
	   Using set_column_title would remove the widget we just
	   set so we have to do this by fiddling with the data
	   in the widget directly (yes, this _is_ bad) */
	clist->column[column].title = title;
	gtk_clist_set_sort_column(clist, column);
    } else {
	GtkWidget *columnw = gtk_clist_get_column_widget (clist, column);
	GtkWidget *arrow = gtk_object_get_data(GTK_OBJECT(columnw), "arrow");
	
	/* re-sorting same column - toggle the sort order */
	if (clist->sort_type == GTK_SORT_ASCENDING) {
	    gtk_arrow_set(GTK_ARROW(arrow), GTK_ARROW_UP, GTK_SHADOW_IN);
	    gtk_clist_set_sort_type(clist, GTK_SORT_DESCENDING);
	} else {
	    gtk_arrow_set(GTK_ARROW(arrow), GTK_ARROW_DOWN, GTK_SHADOW_IN);
	    gtk_clist_set_sort_type(clist, GTK_SORT_ASCENDING);
	}
    }
    gtk_clist_sort(clist);
}

void
on_table_notebook_switch_page(
    GtkNotebook     *notebook,
    GtkNotebookPage *page,
    gint             page_num,
    gpointer         user_data)
{
    if (page_num == 3 && !GTK_OBJECT_DESTROYED(notebook)) {
	/* Should find clist some other way than global */
	GtkCList     *clist	 = GTK_CLIST(obj_mainwin.tree1);
	GList	     *selection	 = clist->selection;
	DirNode	     *dirnode;
	char	     *path_elements[4];
	int          num_elements;
	char         qtemplate[] = 
	    "SELECT * from %s.%s where rownum < %d";
	char         *query;

	if (!selection)
	    return;
	if (GTK_BIN(user_data)->child)
	    return;

	dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(clist), selection->data);
	num_elements = parse_path(dirnode->path, path_elements, 4);
	query = g_malloc(sizeof(qtemplate) + strlen(path_elements[DIRPATH_OWNER]) +
			 strlen(path_elements[DIRPATH_OBJECT]) + 10);
	sprintf(query, qtemplate, path_elements[DIRPATH_OWNER], 
		path_elements[DIRPATH_OBJECT], obj_ginfo.rows);
	{
	    GtkCList *l = clist_from_query(query, path_elements[DIRPATH_DBID], 
					   512, "<NULL>", active_step);
	    active_clear();
	    
	    if (l) {
		gtk_signal_connect (GTK_OBJECT (l), "click_column",
				    GTK_SIGNAL_FUNC (on_grid_view_click_column),
				    NULL);
		l->sort_column = -1;

		gtk_container_add(GTK_CONTAINER(user_data), GTK_WIDGET(l));
		gtk_widget_show(GTK_WIDGET(l));
	    }
	}
	free_path_elements(path_elements, num_elements);
    }
}

