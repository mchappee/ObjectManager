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
#include "tablefuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>

int         response;

void
on_analyze_compute_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

    response = 0;
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}


void
on_analyze_estimate_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    response = 1;
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}


void
on_analyze_delete_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    response = 2;
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}


void
on_analyze_validate_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    response = 3;
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}


GtkWidget*
create_analyzewin (int *response)
{
  GtkWidget *window1;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
  gtk_window_set_title (GTK_WINDOW (window1), ("Analyze Object"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_object_set_data (GTK_OBJECT (window1), "response", response);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_container_add (GTK_CONTAINER (window1), hbuttonbox1);

  button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Compute\nStatistics");
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Estimate\nStatistics");
  gtk_widget_ref (button2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  button3 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Delete\nStatistics");
  gtk_widget_ref (button3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button3", button3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button3);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button3);
  GTK_WIDGET_SET_FLAGS (button3, GTK_CAN_DEFAULT);
/*
  button4 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Validate\nRef Update");
  gtk_widget_ref (button4);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button4", button4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button4);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button4);
  GTK_WIDGET_SET_FLAGS (button4, GTK_CAN_DEFAULT);
*/
  gtk_signal_connect (GTK_OBJECT (button1), "clicked",
                      GTK_SIGNAL_FUNC (on_analyze_compute_clicked),
                      window1);
  gtk_signal_connect (GTK_OBJECT (button2), "clicked",
                      GTK_SIGNAL_FUNC (on_analyze_estimate_clicked),
                      window1);
  gtk_signal_connect (GTK_OBJECT (button3), "clicked",
                      GTK_SIGNAL_FUNC (on_analyze_delete_clicked),
                      window1);
/*  gtk_signal_connect (GTK_OBJECT (button4), "clicked",
                      GTK_SIGNAL_FUNC (on_analyze_validate_clicked),
                      window1);
*/
  return window1;
}

int 
analyzewin()
{
    GtkWidget   *dialog;

    dialog = create_analyzewin(&response);
    gtk_widget_show(dialog);
    gtk_main();

    return response;
}

