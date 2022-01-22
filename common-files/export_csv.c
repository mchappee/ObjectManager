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

/* Save as comma separated values */
void
export_as_csv(
    const char	 *fname,
    GtkCList	 *clist)
{
    char	 *value,*c;
    FILE	 *stream;
    int		 ncols=0;
    int		 nrows=0;

    assert(fname && *fname);

    /* fname is passed from the export function, 
       and is the full path to the export file */
    stream=fopen(fname,"w");

    if (stream) {
	while(nrows < clist->rows){
	    while(ncols < clist->columns){
		gtk_clist_get_text(clist, nrows, ncols, &value);
		ncols += 1;
	    
		/* This routine replaces all commas with semi-colons */
		while((c=strchr(value,','))!=NULL){
		    *c=';';
		}
	    
		/* We don't want a leading comma */
		if(ncols == 1)
		    fprintf(stream,"%s",value);
		else
		    fprintf(stream,",%s",value);
	    }
	    ncols = 0;
	    fprintf(stream,"\n");
	    nrows += 1;
	}
	fclose(stream);
    } else {
	file_open_error(fname, errno, "write");
    }
}

