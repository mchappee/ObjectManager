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
#include <errno.h>
#include <assert.h>
#include <time.h>

/* Make sure we get the cuserid prototype - the automake stuff should
   probably be fixed to take care of this */
#define __USE_XOPEN
#include <unistd.h>

#include "common.h"
#include "oracle_oci.h"

/* Save out the contents of a clist as a set of insert statements */
/* FIXME - check error code from writes */
void
export_as_sql(
    const char *fname, 
    GtkCList *clist)
{
    char      *value;
    FILE      *stream;
    int	      ncol=0;
    int	      nrow=0;
    char      *tablename = gtk_object_get_data(GTK_OBJECT(clist), "tablename");
    char      *owner	 = gtk_object_get_data(GTK_OBJECT(clist), "owner");
    char      *sep;
    int	      coltype;
    time_t    tnow = time(0);

    assert(fname && *fname);
    assert(tablename);
    assert(owner);
    stream=fopen(fname,"w");

    if (stream) {
	fprintf(stream, "/* %s.%s data exported by %s on %s */\n\n",
		owner, tablename, cuserid(0), ctime(&tnow));
	while(nrow < clist->rows){
	    fprintf(stream, "insert into %s.%s values (", owner, tablename);
	    sep = "";
	    for(ncol = 0; ncol < clist->columns; ncol++){
		gtk_clist_get_text(clist, nrow, ncol, &value);
		coltype = 
		    (int) gtk_object_get_data(GTK_OBJECT(gtk_clist_get_column_widget (clist, ncol)), 
					      "coltype");
		/* I hope no-one actually sets a text column to
		   '<NULL>' because they're going to get a surprise if
		   they do.  Unfortunately we can't attach meta data
		   to the individual cells in a clist so we don't have
		   anywhere to say whether the value was NULL or not */
		if (strcmp(value, "<NULL>") == 0) {
		    fprintf(stream,"%sNULL", sep);
		} else {
		    switch (coltype) {
		    case ORA_TINT_VARCHAR2: 
		    case ORA_TINT_LONG:
		    case ORA_TINT_CHAR:
		    {
			char *s = g_malloc(strlen(value)*2+3);
			char *p = s;
			int c;

			*p++ = '\'';
			while ((c = *value++) != '\0') {
			    if (c == '\'') {
				*p++ = '\'';
			    }
			    *p++ = c;
			}
			*p++ = '\'';
			*p++ = '\0';
			fprintf(stream,"%s%s", sep, s);
			g_free(s);
			break;
		    }
		    default:
			fprintf(stream,"%s%s", sep, value);
		    }
		}
		sep = ", ";
	    }
	    fprintf(stream,");\n");
	    nrow += 1;
	}

	fclose(stream);
    } else {
	file_open_error(fname, errno, "write");
    }
}

