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
#include <gdk/gdkkeysyms.h>

#include <include/objectman.h>

#include <xpm/anim1.xpm>
#include <xpm/anim2.xpm>
#include <xpm/anim3.xpm>
#include <xpm/anim4.xpm>
#include <xpm/anim5.xpm>
#include <xpm/anim6.xpm>
#include <xpm/anim7.xpm>
#include <xpm/anim8.xpm>
#include <xpm/anim9.xpm>
#include <xpm/anim10.xpm>
#include <xpm/anim11.xpm>
#include <xpm/anim12.xpm>
#include <xpm/blank.xpm>

static GdkPixmap *pixmaps[13];
static GdkBitmap *masks[13];
static GtkWidget *pixmap;

GtkWidget *
active_init(
    GtkWidget *mainwin)
{
    if (!pixmap) {
	pixmaps[0] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[0], NULL, blank);
	pixmaps[1] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[1], NULL, diamond_frame1);
	pixmaps[2] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[2], NULL, diamond_frame2);
	pixmaps[3] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[3], NULL, diamond_frame3);
	pixmaps[4] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[4], NULL, diamond_frame4);
	pixmaps[5] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[5], NULL, diamond_frame5);
	pixmaps[6] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[6], NULL, diamond_frame6);
	pixmaps[7] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[7], NULL, diamond_frame7);
	pixmaps[8] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[8], NULL, diamond_frame8);
	pixmaps[9] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[9], NULL, diamond_frame9);
	pixmaps[10] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[10], NULL, diamond_frame10);
	pixmaps[11] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[11], NULL, diamond_frame11);
	pixmaps[12] = gdk_pixmap_create_from_xpm_d(mainwin->window, &masks[12], NULL, diamond_frame12);
	pixmap = gtk_pixmap_new(pixmaps[0], masks[0]);
	gtk_widget_show(pixmap);
	return pixmap;
    }
    return 0;
}

int
active_step(
    int counter)
{
    counter /= 12;

    if (counter) {
	gtk_pixmap_set(GTK_PIXMAP(pixmap), pixmaps[(counter%12)+1], masks[(counter%12)+1]);
	while(gtk_events_pending())
	    gtk_main_iteration();
    }
    return 1;
}

void
active_clear(void)
{
    gtk_pixmap_set(GTK_PIXMAP(pixmap), pixmaps[0], masks[0]);
}
