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
#include "objects/tables/tablefuncs.h"
#include "dblinkfuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void
obj_on_dblink_clicked(
    GtkButton	 *button,
    gpointer	 user_data)
{
    char	 *query;
    char	 *newdbname;
    char	 *qtemplate  = "SELECT count (*) from dual@%s";
    char         *tmpobject;
    GList        *owners;
    int          num_elements=0;
    char         *dbid, *owner,*object,*path_elements[4];
    DirNode      *dir  = (DirNode *)user_data;
    
    num_elements=parse_path(dir->path, path_elements, 4);
    dbid = path_elements[DIRPATH_DBID];
    owner = path_elements[DIRPATH_OWNER];
    object = path_elements[DIRPATH_OBJECT];

    /* dblinks are special.  We have to change their global name */

    tmpobject = g_malloc(strlen(obj_cinfo.currentobject) + 1);
    sprintf(tmpobject, "%s", obj_cinfo.currentobject);

    query=strtok(tmpobject,".");
    newdbname=strtok(NULL,"^");

    query = g_malloc(strlen(qtemplate) + strlen(newdbname) + 1);
    sprintf(query, qtemplate, newdbname);

    owners = glist_from_query(query, dbid, 0, TRUE, NULL);

    if(owners != NULL)
        msgbox("DBLink verified, everything's cool");
 
    g_free(query);
    g_free(tmpobject);
    g_list_free (owners);
    free_path_elements(path_elements, num_elements);
   
}


