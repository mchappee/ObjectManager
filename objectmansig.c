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
#include "objects/tablespaces/tablespacefuncs.h"
#include "oracle_oci.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <malloc.h>

/* Global variables */
obj_logininfo    *obj_logins;
obj_currentinfo  obj_cinfo;
obj_globalinfo   obj_ginfo;

/*static gboolean
pmemusage(
    gpointer	     data)
{
    struct mallinfo m = mallinfo();
    fprintf(stderr, "memusage = %dk\n", m.uordblks/1024);
    return TRUE;
}*/

static void
start_config()
{

}

static void
display_usage()
{

    printf("\nObjectManager 2.0 by Orasoft http://www.orasoft.org\n");
    printf("\n-h	This message.");
    printf("\n-d	No dynamic module loading");
    printf("\n-s	No syntax highlighting");
    printf("\n");

}


int 
main (
    int    argc, 
    char   *argv[])
{
  
    int           sig_handle;
    int           c;
    GtkWidget     *loginwin;
    char          *login;
    char          *procedure=NULL;

    gtk_set_locale ();
    gtk_init (&argc, &argv);

    /* gtk_timeout_add(2000, pmemusage, 0); */
    
    osnsui(&sig_handle, gtk_main_quit, (char *) 0);

    login = prefs_get_char_var(PREF_LOGIN);
    if(login != NULL){
	obj_ginfo.globallogin = g_strdup(login);
    } else {
	obj_ginfo.globallogin = g_strdup("none");
    }

    obj_ginfo.highlighting = prefs_get_int_var(PREF_SYNTAX);
    obj_ginfo.dynamicexport = prefs_get_int_var(PREF_MODULAR);
    obj_ginfo.runprocmode = 0;

    while ((c = getopt(argc, argv, "dhscp:")) != -1)
    {
        switch (c)
	{
	    case 'h':
                display_usage();
		exit(0);
                break;
            case 'd':
                obj_ginfo.dynamicexport = 0;
                break;
	    case 's':
	        obj_ginfo.highlighting = 0;
		break;
	    case 'c':
	        start_config();
		exit(0);
		break;
	    case 'p':
	        obj_ginfo.runprocmode = 1;
		procedure = g_strdup(optarg);
		break;
        }
    }

    obj_initglobals();

    obj_mainwin.window1 = obj_create_window1();  
    loginwin = obj_create_login();
    
    validate_tnsnames();
    oracle_initialise();

    if(argc - optind > 0){
        char *dbid = getdbid(argv[optind]);
	int l = strlen(argv[optind]);
	g_free(obj_ginfo.globallogin);
	obj_ginfo.globallogin = g_strdup(argv[optind]);
	memset(argv[optind], '\0', l);
	snprintf(argv[optind], l, "%s", dbid);
	obj_on_login_ok_clicked(NULL, NULL);

	if(!obj_ginfo.runprocmode)
	    gtk_widget_show (obj_mainwin.window1);
	else
	    gtk_widget_show (create_runproc_win(procedure));
	    
	g_free(dbid);
    }else
	gtk_widget_show (loginwin);

    gtk_main ();
    prefs_save_values();
    
    return 0;
}


