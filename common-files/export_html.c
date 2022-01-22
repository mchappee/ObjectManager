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
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "common.h"

/* Quote any html meta chars */
static char *
html_quote_string(
    const char	     *str)
{
    char	     *html_str;
    char	     *s;
    const char	     *p;

    html_str = g_malloc(strlen(str) * 6 + 1);
    for (p = str, s = html_str; p && *p ; p++) {
	switch(*p) {
	case '<': strcpy(s, "&lt ");   s += 4; break;
	case '>': strcpy(s, "&gt ");   s += 4; break;
	case '&': strcpy(s, "&amp ");  s += 5; break;
	case '"': strcpy(s, "&quot "); s += 6; break;
	default: *s++ = *p; break;
	}
    }
    *s = '\0';
    return html_str;
}

/* Save a clist as a html table definition */
void
export_as_html(
    const char	*fname, 
    GtkCList	*clist)
{
    char	*value;
    FILE	*stream;
    int		ncol;
    int		nrow;
    char	*tablename = gtk_object_get_data(GTK_OBJECT(clist), "tablename");
    char	*owner	   = gtk_object_get_data(GTK_OBJECT(clist), "owner");

    assert(fname && *fname);
    assert(tablename);
    assert(owner);
    stream=fopen(fname,"w");

    if (stream) {
	fprintf(stream,"<HTML>\n<HEAD>\n<TITLE>%s.%s</TITLE>\n",
		owner, tablename);
	fprintf(stream,"</HEAD>\n<BODY BGCOLOR=\"#ffffff\">\n\n");

	fprintf(stream,"<table BORDER=\"1\">\n<tr BGCOLOR=\"#C0C0C0\">");
	for (ncol = 0; ncol < clist->columns; ncol++) {
	    char *s = html_quote_string(gtk_clist_get_column_title (clist, ncol));
	    fprintf(stream,"<th>%s</th>", s);
	    g_free(s);
	}

	for(nrow = 0;nrow < clist->rows; nrow++){
	    fprintf(stream,"</tr>\n<tr>");
	    for(ncol = 0; ncol < clist->columns; ncol++){
		char *s;

		gtk_clist_get_text(clist, nrow, ncol, &value);
		s = html_quote_string(value);
		fprintf(stream,"<td>%s</td>\n", *s ? s : "&nbsp");
		g_free(s);
	    }
	    fprintf(stream,"</tr>\n");
	}
	fprintf(stream,"</table>\n");
	fprintf(stream,"</BODY>\n");

	fclose(stream);
    } else {
	file_open_error(fname, errno, "write");
    }
}
