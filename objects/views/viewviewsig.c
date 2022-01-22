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
#include "viewfuncs.h"
#include "objects/tables/tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void
on_view_notebook_switch_page(
    GtkNotebook     *notebook,
    GtkNotebookPage *page,
    gint             page_num,
    gpointer         user_data)
{
    if (page_num == 1 && !GTK_OBJECT_DESTROYED(notebook)) {
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
	query = g_malloc(sizeof(qtemplate) + strlen(path_elements[2]) +
			 strlen(path_elements[3]) + 1);
	sprintf(query, qtemplate, path_elements[2], path_elements[3], obj_ginfo.rows);
	{
	    GtkCList *l = clist_from_query(query, path_elements[0], 512, "<NULL>", active_step);
	    active_clear();

	    if (l) {
		gtk_container_add(GTK_CONTAINER(user_data), GTK_WIDGET(l));
		gtk_widget_show(GTK_WIDGET(l));
	    }
	}
    }
}
