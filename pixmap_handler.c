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
#include "include/objectman.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>

/* Code Object Icons */
#include "xpm/disconnect.xpm"
#include "xpm/open.xpm"
#include "xpm/save.xpm"
#include "xpm/print.xpm"
#include "xpm/error.xpm"
#include "xpm/highlight_error.xpm"
#include "xpm/search.xpm"
#include "xpm/submit.xpm"
#include "xpm/run.xpm"

/* Table Object Icons */
#include "xpm/analyze.xpm"
#include "xpm/rowcount.xpm"
#include "xpm/cache.xpm"
#include "xpm/addcolumn.xpm"
#include "xpm/rename.xpm"
#include "xpm/truncate.xpm"
#include "xpm/index.xpm"
#include "xpm/deallocate.xpm"
#include "xpm/rebuild.xpm"

/* Main Toolbar Icons */
#include "xpm/quit.xpm"
#include "xpm/users.xpm"

/* Login Window Buttons */
#include "xpm/quit-mini.xpm"
#include "xpm/submit-mini.xpm"
#include "xpm/login.xpm"

/* Logfile Toolbar Icons */
#include "xpm/drop.xpm"

/* Session Toolbar Icons */
#include "xpm/om_kill.xpm"

/* Misc Pixmaps */
#include "xpm/oratop.xpm"
#include "xpm/tablewitch.xpm"
#include "xpm/tuxlogo.xpm"
#include "xpm/printer.xpm"

/* Rollback Toolbar Icons */
#include "xpm/rollback_toggle.xpm"
#include "xpm/rollback_shrink.xpm"

/* Tablespace Toolbar Icons */
#include "xpm/coalesce.xpm"

GtkWidget*
create_tablespace_pixmap(
    int    pixmap_id)

{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    
    gdk_pixmap = NULL;
    switch(pixmap_id){
        case TABLESPACETOOLBAR_COALESCE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, tablespace_coalesce);
	    break;
	}
    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
	
    return pixmap;
}


GtkWidget*
create_rollback_pixmap(
    int    pixmap_id)

{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    
    gdk_pixmap = NULL;
    switch(pixmap_id){
        case ROLLBACKTOOLBAR_TOGGLE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, rollback_toggle);
	    break;
	}
	case ROLLBACKTOOLBAR_SHRINK: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, rollback_shrink);
	    break;
	}
    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
	
    return pixmap;
}


GtkWidget*
create_misc_pixmap(
    int    pixmap_id)

{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    GtkWidget    *window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_widget_realize(window1);
    gdk_pixmap = NULL;
    
    switch(pixmap_id){
        case ADVANTIO_PIXMAP: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, advantio);
	    break;
	}
	case TABLEWITCH_PIXMAP: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, tablewitch);
	    break;
	}
	case TUXLOGO_PIXMAP: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, tuxlogo);
	    break;
	}
	case PRINTER_PIXMAP: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, printer);
	    break;
	}
    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
    gtk_widget_destroy(window1);
	
    return pixmap;
}


GtkWidget*
create_session_pixmap(
    int    pixmap_id)

{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    
    gdk_pixmap = NULL;
    switch(pixmap_id){
        case SESSIONTOOLBAR_KILL: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, session_kill);
	    break;
	}
    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
	
    return pixmap;
}


GtkWidget*
create_logfile_pixmap(
    int    pixmap_id)

{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    
    gdk_pixmap = NULL;
    switch(pixmap_id){
        case LOGFILETOOLBAR_DROP: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, logfile_drop);
	    break;
	}
    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
	
    return pixmap;
}
	

GtkWidget*
create_login_pixmap(
    int    pixmap_id)

{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    GtkWidget    *window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_widget_realize(window1);
    gdk_pixmap = NULL;
    switch(pixmap_id){
        case LOGINBUTTON_QUIT: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, login_quit);
	    break;
	}
	case LOGINBUTTON_LOGIN: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, login_login);
	    break;
	}
	case LOGINBUTTON_PIXMAP: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, login_pixmap);
	    break;
	}
    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
    gtk_widget_destroy(window1);
	
    return pixmap;
}


GtkWidget*
create_main_pixmap(
    int    pixmap_id)

{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    GtkWidget    *window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_widget_realize(window1);
    gdk_pixmap = NULL;
    switch(pixmap_id){
        case MAINTOOLBAR_QUIT: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, main_quit);
	    break;
	}
	case MAINTOOLBAR_LOGIN: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, main_login);
	    break;
	}
    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
    gtk_widget_destroy(window1);
	
    return pixmap;
}


GtkWidget*
create_table_pixmap(
    int    pixmap_id)

{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    
    gdk_pixmap = NULL;
    switch(pixmap_id){
        case TABLETOOLBAR_ANALYZE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, table_analyze);
	    break;
	}
	case TABLETOOLBAR_COUNT: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, table_rowcount);
	    break;
	}
	case TABLETOOLBAR_CACHE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, table_cache);
	    break;
	}
	case TABLETOOLBAR_ADD_COLUMN: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, table_addcolumn);
	    break;
	}
	case TABLETOOLBAR_RENAME: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, table_rename);
	    break;
	}
	case TABLETOOLBAR_TRUNCATE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, table_truncate);
	    break;
	}
	case TABLETOOLBAR_INDEX: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, table_index);
	    break;
	}
	case TABLETOOLBAR_DEALLOCATE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, table_deallocate);
	    break;
	}
	case INDEXTOOLBAR_REBUILD: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, index_rebuild);
	    break;
	}
    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
	
    return pixmap;

}


GtkWidget*
create_proc_pixmap(
    int    pixmap_id)
{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    
    gdk_pixmap = NULL;
    switch(pixmap_id){
        case PROCTOOLBAR_ENABLE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_enable);
	    break;
	}
	case PROCTOOLBAR_OPEN: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_open);
	    break;
	}
	case PROCTOOLBAR_SAVE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_save);
	    break;
	}
	case PROCTOOLBAR_PRINT: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_print);
	    break;
	}
	case PROCTOOLBAR_MESSAGE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_message);
	    break;
	}
	case PROCTOOLBAR_POSITION: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_position);
	    break;
	}
	case PROCTOOLBAR_SUBMIT: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_submit);
	    break;
	}
	case PROCTOOLBAR_EXECUTE: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_execute);
	    break;
	}
	case PROCTOOLBAR_SEARCH: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(obj_mainwin.window1->window, &pixmap_mask, NULL, proc_search);
	    break;
	}
	
    }
	pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
	
	return pixmap;

}

