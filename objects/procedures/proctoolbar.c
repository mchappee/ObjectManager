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
#include "procfuncs.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void 
obj_create_proc_toolbar(
    gpointer data,
    int      options)
{
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *button3;
    GtkWidget *button4;
    GtkWidget *button5;
    GtkWidget *button6;
    GtkWidget *vseparator1;
    GtkWidget *vseparator2;
    GtkWidget *tmp_toolbar_icon;
    int       tbstate;

    obj_mainwin.dynamictoolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    gtk_widget_ref (obj_mainwin.dynamictoolbar1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.dynamictoolbar1", obj_mainwin.dynamictoolbar1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_RELIEF_NONE);
    gtk_widget_show (obj_mainwin.dynamictoolbar1);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.toolbar1), obj_mainwin.dynamictoolbar1, NULL, NULL);

    if (options == 2)
    {
        tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_ENABLE);
        if (tmp_toolbar_icon == NULL)
  	    printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
        obj_mainwin.toggletrigger = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
							      GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
							      NULL,
							      ("Enable\nDisable"),
							      "alter trigger ... enable/disable", NULL,
							      tmp_toolbar_icon, NULL, NULL);
        gtk_widget_ref (obj_mainwin.toggletrigger);
        gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.toggletrigger", obj_mainwin.toggletrigger,
			          (GtkDestroyNotify) gtk_widget_unref);
        gtk_signal_connect (GTK_OBJECT (obj_mainwin.toggletrigger), "pressed",
			    GTK_SIGNAL_FUNC (obj_on_trigger_clicked),
			    data);
			
        gtk_widget_hide (obj_mainwin.toggletrigger);
    }

    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_OPEN);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button1 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  "Open",
					  "Open", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button1", button1);
    gtk_widget_show (button1);
    gtk_signal_connect (GTK_OBJECT (button1), "pressed",
			GTK_SIGNAL_FUNC (on_procbutton1_pressed),
			data);

    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_SAVE);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button2 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  "Save",
					  "Save", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button2", button2);
    gtk_widget_show (button2);
    gtk_signal_connect (GTK_OBJECT (button2), "pressed",
			GTK_SIGNAL_FUNC (on_procbutton2_pressed),
			data);

    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_PRINT);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button3 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  "Print",
					  "Print", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button3", button3);
    gtk_widget_show (button3);
    gtk_signal_connect (GTK_OBJECT (button3), "pressed",
			GTK_SIGNAL_FUNC (on_procbutton3_pressed),
			data);

    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_MESSAGE);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    obj_mainwin.procbutton5 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
							  GTK_TOOLBAR_CHILD_BUTTON,
							  NULL,
							  "Message",
							  "Show Error Message", NULL,
							  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.procbutton5", obj_mainwin.procbutton5);
    gtk_widget_hide (obj_mainwin.procbutton5);
    gtk_signal_connect (GTK_OBJECT (obj_mainwin.procbutton5), "pressed",
			GTK_SIGNAL_FUNC (on_procbutton5_pressed),
			data);

    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_POSITION);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    obj_mainwin.procbutton6 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
							  GTK_TOOLBAR_CHILD_BUTTON,
							  NULL,
							  "Position",
							  "Highlight the First Error", NULL,
							  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.procbutton6", obj_mainwin.procbutton6);
    gtk_widget_hide (obj_mainwin.procbutton6);
    gtk_signal_connect (GTK_OBJECT (obj_mainwin.procbutton6), "pressed",
			GTK_SIGNAL_FUNC (on_procbutton6_pressed),
			data);


    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_SUBMIT);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
    button4 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  "Submit",
					  "Submit", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button4", button4);
    gtk_widget_show (button4);
    gtk_signal_connect (GTK_OBJECT (button4), "pressed",
			GTK_SIGNAL_FUNC (on_procbutton4_pressed),
			data);

    if (options == 1)
    {
        tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_EXECUTE);
        if (tmp_toolbar_icon == NULL)
	    printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.\n");
        button5 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					      GTK_TOOLBAR_CHILD_BUTTON,
					      NULL,
					      "Execute",
					      "Execute Procedure", NULL,
					      tmp_toolbar_icon, NULL, NULL);
        gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button5", button5);
        gtk_widget_show (button5);
        gtk_signal_connect (GTK_OBJECT (button5), "pressed",
			    GTK_SIGNAL_FUNC (on_procbutton_execute_pressed),
			    data);
    }

    vseparator1 = gtk_vseparator_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "vseparator1", vseparator1);
    gtk_widget_set_usize (vseparator1, 30, 30);
    gtk_widget_show (vseparator1);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1), vseparator1, NULL, NULL);

/*    obj_mainwin.procentry3 = gtk_entry_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.procentry3", obj_mainwin.procentry3);
    gtk_widget_show (obj_mainwin.procentry3);
    gtk_widget_set_usize (obj_mainwin.procentry3, 100, 22);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1), obj_mainwin.procentry3, NULL, NULL);*/

    tmp_toolbar_icon = create_proc_pixmap (PROCTOOLBAR_SEARCH);
    if (tmp_toolbar_icon == NULL)
	printf ("\nCould not create pixmap, run 'make install' to create pixmaps and reg files.");
    button6 = gtk_toolbar_append_element (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1),
					  GTK_TOOLBAR_CHILD_BUTTON,
					  NULL,
					  "Search",
					  "Search through the code", NULL,
					  tmp_toolbar_icon, NULL, NULL);
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "button6", button6);
    gtk_widget_show (button6);
    gtk_signal_connect (GTK_OBJECT (button6), "clicked",
			GTK_SIGNAL_FUNC (on_procsearch_clicked),
			data);

    vseparator2 = gtk_vseparator_new ();
    gtk_object_set_data (GTK_OBJECT (obj_mainwin.window1), "vseparator2", vseparator2);
    gtk_widget_set_usize (vseparator2, 30, 30);
    gtk_widget_show (vseparator2);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1), vseparator2, NULL, NULL);

    obj_mainwin.proclabelvalid = gtk_label_new (("Valid"));
    gtk_widget_ref (obj_mainwin.proclabelvalid);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.proclabelvalid", obj_mainwin.proclabelvalid,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (obj_mainwin.proclabelvalid);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1), obj_mainwin.proclabelvalid, NULL, NULL);

    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.dynamictoolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.dynamictoolbar1));
    gtk_toolbar_append_space(GTK_TOOLBAR (obj_mainwin.dynamictoolbar1));

    obj_mainwin.proclabelchanged = gtk_label_new (("Changed"));
    gtk_widget_ref (obj_mainwin.proclabelchanged);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.proclabelchanged", obj_mainwin.proclabelchanged,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toolbar_append_widget (GTK_TOOLBAR (obj_mainwin.dynamictoolbar1), obj_mainwin.proclabelchanged, NULL, NULL);
    
    tbstate=prefs_get_int_var(PREF_TBSTATE);
    if(tbstate == 1)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_TEXT);

    if(tbstate == 2)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_ICONS);

    if(tbstate == 3)
        gtk_toolbar_set_style(GTK_TOOLBAR(obj_mainwin.dynamictoolbar1), GTK_TOOLBAR_BOTH);

    
}
