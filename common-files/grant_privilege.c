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
#include <gdk/gdkkeysyms.h>

#include "include/objectman.h"
#include "common-files/common.h"

#include <stdio.h>
#include <assert.h>

char         *dbid;
char         *owner_object;
GtkWidget    *text1;
GtkCList     *clist2;
GtkWidget    *clist1;

typedef struct priv_str{
    char	 *privuser;
    int		 grantoption;
}priv_struct;

priv_struct priv;

static void 
build_priv_sql()
{
    GString *query = g_string_new("GRANT ");
    GString *comma_string = g_string_new(NULL);
    GList   *sel;
    
    for (sel = GTK_CLIST(clist1)->selection; sel != NULL; sel = sel->next ) {
        char    *privitem;
	
	gtk_clist_get_text(GTK_CLIST(clist1),GPOINTER_TO_INT ( sel->data ), 1, &privitem);
	    
	g_string_append(comma_string, privitem);
	
	if(sel->next)
            g_string_append(comma_string, ", ");
	
    }
    
    g_string_append(query, comma_string->str);
    g_string_sprintfa(query, " ON %s",owner_object);
    
    g_string_append(query, " TO ");
  
    if(priv.privuser!=NULL)
	g_string_sprintfa(query, "%s",priv.privuser);
    
    if(priv.grantoption)
	g_string_append(query, " WITH GRANT OPTION");
  
    clear_text(text1);
    gtk_text_insert (GTK_TEXT (text1), NULL, NULL,NULL, query->str, query->len);
    
    g_list_free (sel);
    g_string_free(query, 1);
}


static void
on_clist1_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    build_priv_sql();
}


static void
on_clist2_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    gtk_clist_get_text(clist, row, column, &priv.privuser);
    build_priv_sql();
}


static void
on_checkbutton1_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	priv.grantoption=1;
    else
        priv.grantoption=0;
    
    build_priv_sql();
}


static void
on_execute_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{

    char *cp = gtk_editable_get_chars(GTK_EDITABLE (text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (text1)));
    
    if (execute_immediate_sql(dbid, cp)) {
	msgbox("Grant Succeeded");
    }
    
    g_free(cp);

}


static void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(user_data);
}

GtkWidget*
grant_privilege_win(
    char    *owner,
    char    *object,
    int     object_type,
    char    *cdbid)
{
  GtkWidget *window1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *scrolledwindow1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *scrolledwindow2;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *checkbutton1;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *scrolledwindow3;
  char      *display_text;
  char      *tmppriv[2];

  dbid = cdbid;

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
  gtk_window_set_title (GTK_WINDOW (window1), ("Grant Privilege"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_widget_set_usize(vbox1, 350, 500);
  gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  label1 = gtk_label_new (("Grant:"));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);

  clist1 = gtk_clist_new (2);
  gtk_widget_ref (clist1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "clist1", clist1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (clist1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), clist1);
  gtk_clist_set_column_width (GTK_CLIST (clist1), 0, 1);
  gtk_clist_set_column_width (GTK_CLIST (clist1), 1, 80);
  gtk_clist_set_selection_mode (GTK_CLIST (clist1), GTK_SELECTION_MULTIPLE);
  gtk_clist_column_titles_hide (GTK_CLIST (clist1));

  label2 = gtk_label_new (("label2"));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_clist_set_column_widget (GTK_CLIST (clist1), 0, label2);

  label3 = gtk_label_new (("label3"));
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_clist_set_column_widget (GTK_CLIST (clist1), 1, label3);

  owner_object = g_malloc(strlen(owner) + strlen(object) + 2);
  sprintf(owner_object, "%s.%s", owner, object);
  
  display_text = g_malloc(strlen(owner_object) + 10);
  sprintf(display_text, "On Object %s", owner_object);

  label4 = gtk_label_new (display_text);
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (vbox1), label4, FALSE, FALSE, 0);

  g_free(display_text);

  label5 = gtk_label_new (("To"));
  gtk_widget_ref (label5);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (vbox1), label5, FALSE, FALSE, 0);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow2, TRUE, TRUE, 0);

  clist2 = clist_from_query("SELECT username FROM all_users order by username", cdbid, 0, "", NULL);
  gtk_widget_ref (GTK_WIDGET(clist2));
  gtk_object_set_data_full (GTK_OBJECT (window1), "clist2", clist2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (GTK_WIDGET(clist2));
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), GTK_WIDGET(clist2));
  gtk_clist_column_titles_hide (GTK_CLIST (clist2));

  label6 = gtk_label_new (("label6"));
  gtk_widget_ref (label6);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label6", label6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label6);
  gtk_clist_set_column_widget (GTK_CLIST (clist2), 0, label6);

  label7 = gtk_label_new (("label7"));
  gtk_widget_ref (label7);
  gtk_object_set_data_full (GTK_OBJECT (window1), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_clist_set_column_widget (GTK_CLIST (clist2), 1, label7);

  checkbutton1 = gtk_check_button_new_with_label (("With grant privilege"));
  gtk_widget_ref (checkbutton1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton1", checkbutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (checkbutton1);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbutton1, FALSE, FALSE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
  gtk_widget_ref (button2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow3);
  gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow3", scrolledwindow3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow3);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow3, TRUE, TRUE, 0);

  text1 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "text1", text1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), text1);
  
  tmppriv[0]=NULL;
  switch (object_type) {
      case OMDB_Table:
          tmppriv[1]="ALTER";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="DELETE";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="INDEX";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="INSERT";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="REFERENCES";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="SELECT";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="UPDATE";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);	  
          break;
      case OMDB_Procedure:
          tmppriv[1]="EXECUTE";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
          break;
      case OMDB_Function:
          tmppriv[1]="EXECUTE";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
          break;
      case OMDB_Package:
          tmppriv[1]="EXECUTE";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
          break;
      case OMDB_Sequence:
          tmppriv[1]="ALTER";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="SELECT";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
          break;
      case OMDB_View:
	  tmppriv[1]="DELETE";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="INSERT";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="SELECT";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
	  tmppriv[1]="UPDATE";
          gtk_clist_append(GTK_CLIST(clist1), tmppriv);
  }

  gtk_signal_connect (GTK_OBJECT (clist1), "select_row",
                      GTK_SIGNAL_FUNC (on_clist1_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (clist2), "select_row",
                      GTK_SIGNAL_FUNC (on_clist2_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (clist1), "unselect_row",
                        GTK_SIGNAL_FUNC (on_clist1_select_row),
                        NULL);
  gtk_signal_connect (GTK_OBJECT (checkbutton1), "toggled",
                      GTK_SIGNAL_FUNC (on_checkbutton1_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button1), "clicked",
                      GTK_SIGNAL_FUNC (on_execute_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button2), "clicked",
                      GTK_SIGNAL_FUNC (on_cancel_clicked),
                      window1);

  return window1;
}

