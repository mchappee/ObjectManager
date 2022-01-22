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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "ps_header.h"

char    *
get_tooltip()
{
    return "Export table as PostScript";
}

char    *
get_menuitem()
{
    return "PostScript";
}

char    *
get_description()
{
    return "ObjectManager PostScript Export Module";
}

char    *
get_author()
{
    return "Matthew Chappee (matthew@orasoft.org)";
}

char    *
get_extension()
{
    return "*.ps";
}

void
export_procedure(
    const char	 *fname,
    GtkCList	 *clist)
{
    char	 *value,*c;
    FILE	 *stream;
    int		 ncols=0;
    int		 nrows=0;
    int          placeholder = 715;
    int          pagenum = 2;
    char         *buildpage;

    assert(fname && *fname);

    /* fname is passed from the export function, 
       and is the full path to the export file */
    stream=fopen(fname,"w");

    if (stream) {
        fprintf(stream, "%s\n", ps_header);
	while(nrows < clist->rows){
            if( placeholder <= 9 ){
                buildpage = g_malloc(strlen(ps_newpage) + 100);
                sprintf(buildpage, ps_newpage, pagenum, pagenum, pagenum, pagenum);
                pagenum++;
                fprintf(stream, "%s\n%s", ps_footer, buildpage);
                g_free(buildpage);
                placeholder = 715;
            }
            fprintf(stream, "5 %d M\n(", placeholder);
            placeholder -= 8;
	    while(ncols < clist->columns){
		gtk_clist_get_text(clist, nrows, ncols, &value);
		ncols += 1;
	    
		/* This routine replaces all tabs with a space */
		while((c=strchr(value,'	'))!=NULL){
		    *c=';';
		}
	    
		/* We don't want a leading tab */
		if(ncols == 1)
		    fprintf(stream,"%s",value);
		else
		    fprintf(stream,"	%s",value);
	    }
	    ncols = 0;
	    fprintf(stream,") s\n");
	    nrows += 1;
	}
        fprintf(stream, "%s\n", ps_footer);
	fclose(stream);
    } else {
	file_open_error(fname, errno, "write");
    }
}

