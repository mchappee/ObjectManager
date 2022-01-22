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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "include/objectman.h"
#include "procfuncs.h"

int          total_args;
GtkWidget    *entry[1024];
GtkWidget    *exec_text;
char         *objectid;

static void
on_proctext1_changed(
    GtkEditable     *editable,
    gpointer         user_data)
{
    gtk_label_set_text(GTK_LABEL (obj_mainwin.proclabelchanged),"Not Saved");
    gtk_widget_show(obj_mainwin.proclabelchanged);
}

void
obj_doprocview(void)
{
    GtkWidget *hbox1;
    GtkWidget *label13;
    GtkWidget *vscrollbar1;

    obj_mainwin.notebook1 = gtk_notebook_new ();
    gtk_widget_show (obj_mainwin.notebook1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
    gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_set_usize (hbox1, 800, 600);
    gtk_widget_show (hbox1);
    gtk_container_add (GTK_CONTAINER (obj_mainwin.notebook1), hbox1);

    if(obj_ginfo.highlighting){
        obj_mainwin.proctext1 = gtk_editor_new(NULL, NULL);
	install_pat(GTK_EDITOR(obj_mainwin.proctext1),obj_mainwin.window1);
    }else
        obj_mainwin.proctext1 = gtk_text_new(NULL, NULL);
	
    gtk_widget_show (obj_mainwin.proctext1);
    gtk_container_add (GTK_CONTAINER (hbox1), obj_mainwin.proctext1);
    gtk_text_set_editable (GTK_TEXT (obj_mainwin.proctext1), TRUE);

    vscrollbar1 = gtk_vscrollbar_new (GTK_TEXT (obj_mainwin.proctext1)->vadj);
    gtk_box_pack_start (GTK_BOX (hbox1), vscrollbar1, FALSE,FALSE, 0);
    gtk_widget_show (vscrollbar1);

    label13 = gtk_label_new ("Procedure definition");
    gtk_widget_show (label13);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), 
				label13);
			      
    gtk_signal_connect (GTK_OBJECT (obj_mainwin.proctext1), "changed",
			GTK_SIGNAL_FUNC (on_proctext1_changed),
			NULL);

    return;
}

static void
on_runproc_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(GTK_WIDGET(user_data));
    gtk_main_quit();
}

GtkWidget*
create_runproc_win (
    char    *procedure)
{
  GtkWidget *runproc_win;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *vbox2;
  GtkWidget *timelabel;
  GtkWidget *statuslabel;
  GtkWidget *pixmap1;
  GtkWidget *hbuttonbox1;
  GtkWidget *runproc_ok;

  runproc_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (runproc_win), "runproc_win", runproc_win);
  gtk_window_set_title (GTK_WINDOW (runproc_win), "Running Procedure");
  gtk_window_set_position (GTK_WINDOW (runproc_win), GTK_WIN_POS_CENTER);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (runproc_win), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (runproc_win), vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (runproc_win), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (runproc_win), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox2, TRUE, TRUE, 0);

  timelabel = gtk_label_new (_("0 seconds"));
  gtk_widget_ref (timelabel);
  gtk_object_set_data_full (GTK_OBJECT (runproc_win), "timelabel", timelabel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (timelabel);
  gtk_box_pack_start (GTK_BOX (vbox2), timelabel, FALSE, FALSE, 0);
  gtk_widget_set_usize (timelabel, -2, 47);

  statuslabel = gtk_label_new (_("Procedure running, \ndo not kill."));
  gtk_widget_ref (statuslabel);
  gtk_object_set_data_full (GTK_OBJECT (runproc_win), "statuslabel", statuslabel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statuslabel);
  gtk_box_pack_start (GTK_BOX (vbox2), statuslabel, FALSE, FALSE, 0);
  gtk_widget_set_usize (statuslabel, -2, 49);

  pixmap1 = create_misc_pixmap (ADVANTIO_PIXMAP);
  gtk_widget_ref (pixmap1);
  gtk_object_set_data_full (GTK_OBJECT (runproc_win), "pixmap1", pixmap1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pixmap1);
  gtk_box_pack_start (GTK_BOX (hbox1), pixmap1, TRUE, TRUE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (runproc_win), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, TRUE, TRUE, 0);

  runproc_ok = gtk_button_new_with_label (_("Exit"));
  gtk_widget_ref (runproc_ok);
  gtk_object_set_data_full (GTK_OBJECT (runproc_win), "runproc_ok", runproc_ok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (runproc_ok);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), runproc_ok);
  GTK_WIDGET_SET_FLAGS (runproc_ok, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (runproc_ok), "clicked",
                      GTK_SIGNAL_FUNC (on_runproc_ok_clicked),
                      runproc_win);

  gtk_widget_show(runproc_win);
  while (gtk_events_pending())
	            gtk_main_iteration();
		    
  execute_nonblocking_sql(obj_cinfo.login->dbid, procedure, timelabel);
  gtk_label_set_text(GTK_LABEL(statuslabel), "The procedure has finished.\n You may exit this window.");
  
  return runproc_win;

}


static void
on_exproc_execute_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    
    pid_t   pid;

    pid = fork();

    if (pid == 0) {
	char    *farg[5];
        char    *me;
	char    *cp;
	
	cp = gtk_editable_get_chars(GTK_EDITABLE (exec_text),(gint) 0,
				(gint) gtk_text_get_length (GTK_TEXT (exec_text)));
				
	me = g_get_prgname();
	farg[0] = me;
	farg[1] = obj_ginfo.globallogin;
	farg[2] = "-p";
	farg[3] = cp;
	farg[4] = NULL;
	execvp(me, farg);
	perror("exec error");
	g_free(cp);
	exit(0);
    }
    
    
    if (pid < 0) {
	perror("fork error");
	return;
    }
}


static void
on_exproc_cancel_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(user_data);
}

static void
on_argument_changed        (GtkEditable     *editable,
                            gpointer         user_data)
{
    int     i=0;
    char    *entry_text;
    GString *query = g_string_new("BEGIN ");

    g_string_sprintfa(query, "%s(",objectid);
    
    for(i = 0; i < total_args; i++){ 
        if(i)
	    g_string_append(query, ",");
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry[i]));
        if(strlen(entry_text))
            g_string_sprintfa(query, "'%s'", entry_text);
    }
    g_string_append(query, "); END;");
    
    clear_text(exec_text);
    gtk_text_insert (GTK_TEXT (exec_text), NULL, NULL,NULL, query->str,
		     query->len);
    g_string_free(query, 1);
}


GtkWidget *
create_execute_statement(GtkCList *list, char *object_id)
{
    GtkWidget *window1;
    GtkWidget *vbox1;
    GtkWidget *scrolledwindow2;
    GtkWidget *viewport1;
    GtkWidget *table1;
    GtkWidget *hbuttonbox1;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *scrolledwindow1;
    int       i=0;

    window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
    gtk_window_set_title (GTK_WINDOW (window1), ("Execute Procedure"));
    gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
    objectid = g_malloc(strlen(object_id) + 1);
    strcpy(objectid,object_id);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_widget_set_usize (vbox1, 450, 300);
    gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window1), vbox1);

    scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow2", scrolledwindow2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow2);
    gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow2, TRUE, TRUE, 0);
    
    viewport1 = gtk_viewport_new (NULL, NULL);
    gtk_widget_ref (viewport1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "viewport1", viewport1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (viewport1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow2), viewport1);

    table1 = gtk_table_new (1, 2, FALSE);
    gtk_widget_ref (table1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "table1", table1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (table1);
    gtk_container_add (GTK_CONTAINER (viewport1), table1);

    for (i = 0; i < GTK_CLIST(list)->rows; i++) {
	GtkWidget *typelabel;
	GtkWidget *namelabel;
	char      *argname;
	char      *datatype;
	char      *padded_argname;
	char      *padded_datatype;
	gtk_clist_get_text(GTK_CLIST(list), i, 0, &argname);
	gtk_clist_get_text(GTK_CLIST(list), i, 1, &datatype);
	
	padded_argname = g_malloc(strlen(argname) + 12);
	padded_datatype = g_malloc(strlen(datatype) + 8);
	
	sprintf(padded_argname,"   %s      ",argname);
	sprintf(padded_datatype,"%s      ",datatype);
	
	typelabel = gtk_label_new (padded_datatype);
	namelabel = gtk_label_new (padded_argname);
	
	g_free(padded_argname);
	g_free(padded_datatype);
	
	entry[i] = gtk_entry_new ();
	/*gtk_object_set_data (GTK_OBJECT (window1), objects[i].labeltext, menuitem);*/
	gtk_table_attach (GTK_TABLE (table1), namelabel, 0, 1, i, i+1,
		      (GtkAttachOptions) (0),
		      (GtkAttachOptions) (0), 0, 0);
	gtk_table_attach (GTK_TABLE (table1), typelabel, 1, 2, i, i+1,
		      (GtkAttachOptions) (0),
		      (GtkAttachOptions) (0), 0, 0);
        gtk_table_attach (GTK_TABLE (table1), entry[i], 2, 3, i, i+1,
		      (GtkAttachOptions) (0),
		      (GtkAttachOptions) (0), 0, 0);

	gtk_widget_show (namelabel);
	gtk_widget_show (typelabel);
	gtk_widget_show (entry[i]);

	gtk_signal_connect (GTK_OBJECT (entry[i]), "changed",
			GTK_SIGNAL_FUNC (on_argument_changed),
			(gpointer)i);
    }

    total_args = i;

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button1 = gtk_button_new_with_label (("Execute"));
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    button2 = gtk_button_new_with_label (("Cancel"));
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow1", scrolledwindow1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow1);
    gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    exec_text = gtk_text_new (NULL, NULL);
    gtk_widget_ref (exec_text);
    gtk_object_set_data_full (GTK_OBJECT (window1), "exec_text", exec_text,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (exec_text);
    gtk_container_add (GTK_CONTAINER (scrolledwindow1), exec_text);

    on_argument_changed (GTK_ENTRY(entry[i]), NULL);

    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (on_exproc_execute_clicked),
			window1);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_exproc_cancel_clicked),
			window1);

    return window1;
}



