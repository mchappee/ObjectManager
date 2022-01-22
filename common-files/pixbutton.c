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
 
/* On a side note, the following code was borrowed from 
 * the gnome libs.  Specifically libgnomeui/gnome-stock.c
 */
  
 
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "include/objectman.h"
#include "objects/procedures/procfuncs.h"
#include "objects/tables/tablefuncs.h"
#include "xpm/core-cancel.xpm"
#include "xpm/core-ok.xpm"
#include "xpm/core-back.xpm"
#include "xpm/core-storage.xpm"
#include "xpm/core-add.xpm"
#include "xpm/core-remove.xpm"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <signal.h>


GtkWidget *
pixmap_button(GtkWidget *pixmap, const char *text)
{
	GtkWidget *button, *label, *hbox, *w;
	gboolean use_icon, use_label;

	g_return_val_if_fail(text != NULL, NULL);

	button = gtk_button_new();
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_container_add(GTK_CONTAINER(button), hbox);
	w = hbox;
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(w), hbox, TRUE, FALSE, 7);

	use_icon = TRUE;
	use_label = TRUE;

	if ((use_label) || (!use_icon) || (!pixmap)) {
		label = gtk_label_new(text);
		gtk_widget_show(label);
		gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, 7);
	}

		gtk_widget_show(pixmap);
		gtk_box_pack_start(GTK_BOX(hbox), pixmap,
				   FALSE, FALSE, 0);

	return button;
}

GtkWidget *
create_button_pixmap(
    int a)
{
    GdkPixmap    *gdk_pixmap;
    GdkBitmap    *pixmap_mask;
    GtkWidget    *pixmap;
    GtkWidget    *window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_widget_realize(window1);
    gdk_pixmap = NULL;
    switch(a){
        case BUTTON_PIX_OK: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, core_ok);
	    break;
	}
	case BUTTON_PIX_CANCEL:{
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, core_cancel);
            break;
	}
        case BUTTON_PIX_BACK: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, core_back);
	    break;
	}
	case BUTTON_PIX_STORAGE:{
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, core_storage);
            break;
	}
        case BUTTON_PIX_ADD: {
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, core_add);
	    break;
	}
	case BUTTON_PIX_REMOVE:{
            gdk_pixmap = 
                gdk_pixmap_create_from_xpm_d(window1->window, &pixmap_mask, NULL, core_remove);
            break;
	}

    }
    
    pixmap = gtk_pixmap_new (gdk_pixmap, pixmap_mask);
    gtk_widget_destroy(window1);
    
    return pixmap; 

}


