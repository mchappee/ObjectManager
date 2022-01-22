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

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <gtk/gtk.h>

#include "common.h"

/* Add single character to buffer, extending if necessary */
static void
addchar(
    char **buf,
    char c,
    int  *size,
    int  *allocated)
{
    if ((*size) + 1 >= *allocated) {
	*allocated *= 2;
	*buf = g_realloc(*buf, *allocated);
    }
    (*buf)[*size] = c;
    (*size)++;
}

/* Add string to buffer, extending if necessary */
static void
addstr(
    char **buf,
    const char *s,
    int  *size,
    int  *allocated)
{
    if (s && *s) {
	int l = strlen(s);
	if ((*size) + l + 1 >= *allocated) {
	    *allocated = *allocated * 2 + l;
	    *buf = g_realloc(*buf, *allocated);
	}
	strcpy(*buf + *size, s);
	*size += l;
    }
}

/* Formatted printf routine. Similar to vsprintf but allocates the
   returned string (which should be freed with g_free) */

char *
vmsgfmt(
    const gchar *msg,
    va_list      args)
{
    int  allocated = strlen(msg) * 2;
    int  len = 0;
    const char *s = msg;
    char *r = g_malloc(allocated);
    char c;

    while (*s) {
	switch(c = *s++) {
	case '%': {
	    if ((c = *s++) != '\0') {
		switch(c) {
		case '%': {
		    addchar(&r, '%', &len, &allocated);
		    break;
		}
		case 's': {
		    char *str = va_arg(args, char *);
		    addstr(&r, str, &len, &allocated);
		    break;
		}
		case 'e': {
		    int e = va_arg(args, int);
		    addstr(&r, strerror(e), &len, &allocated);
		    break;
		}
		case 'd': {
		    int d = va_arg(args, int);
		    char buf[32];
		    snprintf(buf, sizeof(buf), "%d", d);
		    addstr(&r, buf, &len, &allocated);
		    break;
		}
		case 'c': {
		    int c = va_arg(args, int);
		    addchar(&r, c, &len, &allocated);
		    break;
		}
		default: g_error("Unsupported format character '%c'\n", c);
		}
	    }
	    break;
	}
	case '\\': {
	    if ((c = *s++) != '\0') {
		switch(c) {
		case 'n': { 
		    addchar(&r, '\n', &len, &allocated);
		    break;
		}
		case 'r': { 
		    addchar(&r, '\r', &len, &allocated);
		    break;
		}
		case 't': { 
		    addchar(&r, '\t', &len, &allocated);
		    break;
		}
		case 'b': { 
		    addchar(&r, '\b', &len, &allocated);
		    break;
		}
		case 'f': { 
		    addchar(&r, '\f', &len, &allocated);
		    break;
		}
		case '\'': { 
		    addchar(&r, '\'', &len, &allocated);
		    break;
		}
		case '0': { 
		    addchar(&r, '\0', &len, &allocated);
		    break;
		}
		case '\\': { 
		    addchar(&r, '\\', &len, &allocated);
		    break;
		}
		default: addchar(&r, c, &len, &allocated);
		}
	    }
	    break;
	}
	default: addchar(&r, c, &len, &allocated);
	}
    }
    addchar(&r, '\0', &len, &allocated);
    return r;
}

