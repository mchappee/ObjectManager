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

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "include/objectman.h"
#include "objects/tables/tablefuncs.h"
#include "securityfuncs.h"

GtkWidget *createuser_clist1;
GtkWidget *createuser_text1;
GtkWidget *createuser_clist2;
GtkWidget *quota_window;

int         response = 0;

typedef struct createuser_struct{
    char    *username;
    char    *password;
    int     external_password;
    char    *dtablespace;
    char    *ttablespace;
    char    *profile;
    char    *quotaval;
    int     quotavalM;
    int     quotaval_unlimited;
    int     userflag;
    int     alterflag;
}createuser;

createuser user;

static void
build_createuser(void)
{
    GString *query = g_string_new(NULL);
    int    a=0;

    if(user.alterflag)
        g_string_append(query, "ALTER ");
    else
        g_string_append(query, "CREATE ");

    if(user.userflag)
        g_string_append(query, "USER ");
    else
        g_string_append(query, "ROLE ");

    if(user.username)
        g_string_append(query, user.username);
	
    if(!user.external_password){
        if(user.password){
            g_string_sprintfa(query, " IDENTIFIED BY %s",user.password);
	}else{
	     if(!user.userflag)
		g_string_append(query, " NOT IDENTIFIED");
        }
    }else{
        g_string_append(query, " IDENTIFIED EXTERNALLY");
    }

    if(user.dtablespace)
        g_string_sprintfa(query, " DEFAULT TABLESPACE %s",user.dtablespace);

    if(user.ttablespace)
        g_string_sprintfa(query, " TEMPORARY TABLESPACE %s",user.ttablespace);
	
    if(user.profile)
        g_string_sprintfa(query, " PROFILE %s",user.profile);

    if(user.quotaval_unlimited && user.dtablespace)
        g_string_sprintfa(query, " QUOTA UNLIMITED ON %s", user.dtablespace);
	
    if(!user.quotaval_unlimited && user.dtablespace){
        if(user.quotavalM){
            if(user.quotaval)
	        g_string_sprintfa(query, " QUOTA %s M ON %s", user.quotaval, user.dtablespace);
        }else{
	    if(user.quotaval)
	        g_string_sprintfa(query, " QUOTA %s K ON %s", user.quotaval, user.dtablespace);
	}
    }
    
    gtk_clist_clear(GTK_CLIST(createuser_clist2));
    while(a < GTK_CLIST(createuser_clist1)->rows){
        char    *rowlist[2];
	GString *permission_string = g_string_new(NULL);
        char    *permission;
        char    *adminflag;
        gtk_clist_get_text(GTK_CLIST(createuser_clist1), a, 1, &permission);
	gtk_clist_get_text(GTK_CLIST(createuser_clist1), a, 0, &adminflag);
	
	if(!strncmp(adminflag,"Quota",3)){
	    g_string_append(permission_string, "ALTER ");
	    
	    if(user.userflag)
                g_string_append(permission_string, "USER ");
            else
                g_string_append(permission_string, "ROLE ");
	    
	    if(user.username)
	        g_string_sprintfa(permission_string, " %s QUOTA %s ",user.username, permission);
	    else
	        g_string_sprintfa(permission_string, " <NULL> QUOTA %s ",permission);
	
	    rowlist[0] = NULL;
	    rowlist[1] = permission_string->str;
	    gtk_clist_append(GTK_CLIST(createuser_clist2),rowlist);
	
	}else{
	    g_string_append(permission_string, "GRANT ");
	
	    if(user.username)
	        g_string_sprintfa(permission_string, " %s TO %s ",permission, user.username);
	    else
	        g_string_sprintfa(permission_string, " %s TO <NULL> ",permission);
	    
	    if(!strncmp(adminflag,"Yes",3))
	        g_string_append(permission_string, "WITH ADMIN OPTION");
	
	    rowlist[0] = NULL;
	    rowlist[1] = permission_string->str;
	    gtk_clist_append(GTK_CLIST(createuser_clist2),rowlist); 
	}
        a++;
    }

    clear_text(createuser_text1);
    gtk_text_insert (GTK_TEXT (createuser_text1), NULL, NULL,NULL, query->str,query->len);
    
    if(obj_ginfo.highlighting)
        gtk_editor_hilite_buffer (GTK_EDITOR(createuser_text1));
    
    g_string_free(query, 1);
    
}


void
on_create_user_password_changed        (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	user.password=entry;
    }else{
	user.password=NULL;
    }

    build_createuser();
}


void
on_create_user_ttablespace_changed     (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	user.ttablespace=entry;
    }else{
	user.ttablespace=NULL;
    }

    build_createuser();
}


void
on_create_user_username_changed        (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	user.username=entry;
    }else{
	user.username=NULL;
    }

    build_createuser();
}


void
on_create_user_quotaval_changed        (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	user.quotaval=entry;
    }else{
	user.quotaval=NULL;
    }

    build_createuser();
}


void
on_create_user_dtablespace_changed     (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	user.dtablespace=entry;
    }else{
	user.dtablespace=NULL;
    }

    build_createuser();
}


void
on_create_user_os_password_toggled (GtkToggleButton *togglebutton,
                                    gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	user.external_password=1;
    else
	user.external_password=0;
    
    build_createuser();
}


void
on_create_user_quota_unlimited_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	user.quotaval_unlimited=1;
    else
	user.quotaval_unlimited=0;
    
    build_createuser();
}


void
on_create_user_quoteM_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	user.quotavalM=1;
    else
	user.quotavalM=0;
    
    build_createuser();
}


void
on_create_user_quotaK_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (GTK_TOGGLE_BUTTON (togglebutton)->active==1)
	user.quotavalM=0;
    else
	user.quotavalM=1;
    
    build_createuser();
}


void
on_create_user_profile_changed         (GtkEditable     *editable,
                                        gpointer         user_data)
{
    char *entry=NULL;

    entry=gtk_entry_get_text(GTK_ENTRY(editable));
    if(strlen(entry)>0){
	user.profile=entry;
    }else{
	user.profile=NULL;
    }

    build_createuser();
}


void
on_create_user_adminflag_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    GList   *sel;
    char    *adminflag;

    for (sel = GTK_CLIST(createuser_clist1)->selection; sel != NULL; sel = sel->next ) {
	int row_index = GPOINTER_TO_INT ( sel->data );
	gtk_clist_get_text(GTK_CLIST(createuser_clist1), row_index, 0, &adminflag);
	if(!strncmp(adminflag,"No",2))
	    gtk_clist_set_text(GTK_CLIST(createuser_clist1), row_index, 0, "Yes");
	if(!strncmp(adminflag,"Yes",3))
	    gtk_clist_set_text(GTK_CLIST(createuser_clist1), row_index, 0, "No");
    }

    build_createuser();

}


void
on_create_user_removeperm_clicked       (GtkButton       *button,
                                         gpointer         user_data)
{
    GList   *sel;
    int row_index;

    sel = GTK_CLIST(createuser_clist1)->selection;
    if(sel){
        row_index = GPOINTER_TO_INT ( sel->data );
        gtk_clist_remove (GTK_CLIST(createuser_clist1), row_index);
        build_createuser();
    }
    
}

void
on_create_user_added_click_column      (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}

void
on_quota_click_column       (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    int     quotanum;
    char    *rowtext;
    char    *userperm[2];
    
    quotanum = get_quota();
    gtk_clist_get_text(GTK_CLIST(clist), row, 0, &rowtext);
    
    userperm[0]="Quota";
    userperm[1] = g_malloc(strlen(rowtext) + 24);

    switch (quotanum) {
        case -2:
	    break;
	case -1:
	    sprintf(userperm[1], "UNLIMITED ON %s",rowtext);
            gtk_clist_append(GTK_CLIST(createuser_clist1),userperm);
            break;
	case 0:
	    sprintf(userperm[1], "NONE ON %s",rowtext);
	    gtk_clist_append(GTK_CLIST(createuser_clist1),userperm);
	    break;
	default:
	    sprintf(userperm[1], "%d K ON %s",quotanum,rowtext);
	    gtk_clist_append(GTK_CLIST(createuser_clist1),userperm);
    }	

    g_free(userperm[1]);
    build_createuser();
    
}

void
on_create_user_priv_click_column       (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    char    *rowtext;
    char    *userperm[2];

    gtk_clist_get_text(GTK_CLIST(clist), row, 0, &rowtext);
    
    userperm[0]="No";
    userperm[1]=rowtext;
    gtk_clist_append(GTK_CLIST(createuser_clist1),userperm);
    build_createuser();

}


void
on_create_user_roles_click_column      (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    char    *rowtext;
    char    *userperm[2];

    gtk_clist_get_text(GTK_CLIST(clist), row, 0, &rowtext);

    userperm[0]="No";
    userperm[1]=rowtext;
    gtk_clist_append(GTK_CLIST(createuser_clist1),userperm);
    build_createuser();
}


void
create_user_execute_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    char    *cp = gtk_editable_get_chars(GTK_EDITABLE (createuser_text1),(gint) 0,
				      (gint) gtk_text_get_length (GTK_TEXT (createuser_text1)));
    int     a=0;
    char    *permission_statement;
    GString *perm_success = g_string_new("User Created Successfully With:\n\n ");

    if (execute_immediate_sql(obj_cinfo.login->dbid, cp)) {
        while(a < GTK_CLIST(createuser_clist2)->rows){
	    gtk_clist_get_text(GTK_CLIST(createuser_clist2), a, 1, &permission_statement);
	    if(execute_immediate_sql(obj_cinfo.login->dbid, permission_statement))
	        g_string_sprintfa(perm_success, "%s Successfull\n",permission_statement);
	    else
	        g_string_sprintfa(perm_success, "%s Failed\n",permission_statement);
		
	    a++;
	}
	msgbox(perm_success->str);
	g_string_free(perm_success, 1);
    }
    g_free(cp);
}


void
on_create_user_cancel_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(user_data);
}


GtkWidget *
create_db_user (
                int userflag, 
                GtkCList *userlist, 
		GtkCList *privlist, 
		GtkCList *rolelist, 
		GtkCList *quotalist)
{
    GtkWidget *window1;
    GtkWidget *vbox1;
    GtkWidget *notebook1;
    GtkWidget *table1;
    GtkWidget *fixed4;
    GtkWidget *entry2;
    GtkWidget *label4;
    GtkWidget *checkbutton1;
    GtkWidget *fixed6;
    GtkWidget *frame2;
    GtkWidget *fixed8;
    GtkWidget *combo2;
    GtkWidget *combo_entry2;
    GtkWidget *fixed1;
    GtkWidget *entry1;
    GtkWidget *label3;
    GtkWidget *frame1;
    GtkWidget *fixed7;
    GtkWidget *entry4;
    GtkWidget *label5;
    GtkWidget *combo1;
    GtkWidget *combo_entry1;
    GtkWidget *checkbutton2;
    GSList *fixed7_group = NULL;
    GtkWidget *radiobutton1;
    GtkWidget *radiobutton2;
    GtkWidget *fixed9;
    GtkWidget *combo3;
    GtkWidget *combo_entry3;
    GtkWidget *label6;
    GtkWidget *label1;
    GtkWidget *hbox1;
    GtkWidget *vbox2;
    GtkWidget *vbox3;
    GtkWidget *hbuttonbox3;
    GtkWidget *button3;
    GtkWidget *button4;
    GtkWidget *scrolledwindow2;
    GtkWidget *label11;
    GtkWidget *label12;
    GtkWidget *notebook2;
    GtkWidget *scrolledwindow3;
    GtkWidget *clist2;
    GtkWidget *label13;
    GtkWidget *label9;
    GtkWidget *scrolledwindow4;
    GtkWidget *scrolledwindow5;
    GtkWidget *scrolledwindow6;
    GtkWidget *clist3;
    GtkWidget *clist4;
    GtkWidget *label15;
    GtkWidget *label10;
    GtkWidget *label2;
    GtkWidget *hbuttonbox2;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *notelabel1;
    char    *query;
    char    *dbid;
    char    privtemplate[]  = "SELECT UNIQUE privilege FROM sys.dba_sys_privs order by privilege";
    char    roletemplate[]  = "SELECT role FROM sys.dba_roles order by role";
    char    quotatemplate[] = "SELECT tablespace_name, status FROM user_tablespaces "
                              "WHERE status != 'INVALID' order by tablespace_name";

    user.userflag = userflag;
    dbid = obj_cinfo.login->dbid;

    if(userlist==NULL){
        window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
        gtk_window_set_title (GTK_WINDOW (window1), ("Create User"));
        gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
    }else{
        obj_mainwin.notebook1 = gtk_notebook_new ();
        gtk_widget_ref (obj_mainwin.notebook1);
        gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "obj_mainwin.notebook1", obj_mainwin.notebook1,
			      (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show (obj_mainwin.notebook1);
        gtk_container_add (GTK_CONTAINER (obj_mainwin.hbox1), obj_mainwin.notebook1);
        gtk_widget_set_usize (obj_mainwin.notebook1, 500, 600);
        window1 = obj_mainwin.notebook1;
    }

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_widget_set_usize (vbox1, 597, 490);
    gtk_object_set_data_full (GTK_OBJECT (window1), "vbox1", vbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window1), vbox1);

    notebook1 = gtk_notebook_new ();
    gtk_widget_ref (notebook1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "notebook1", notebook1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notebook1);
    gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);

    notelabel1 = gtk_label_new (("Security Info"));
    gtk_widget_ref (notelabel1);
    gtk_object_set_data_full (GTK_OBJECT (obj_mainwin.window1), "notelabel1", notelabel1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notelabel1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (obj_mainwin.notebook1), 
				gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj_mainwin.notebook1), 0), notelabel1);

    table1 = gtk_table_new (3, 2, FALSE);
    gtk_widget_ref (table1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "table1", table1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (table1);
    gtk_container_add (GTK_CONTAINER (notebook1), table1);

    fixed4 = gtk_fixed_new ();
    gtk_widget_ref (fixed4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed4", fixed4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed4);
    gtk_table_attach (GTK_TABLE (table1), fixed4, 1, 2, 0, 1,
		      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
		      (GtkAttachOptions) (GTK_FILL), 0, 0);

    entry2 = gtk_entry_new ();
    gtk_widget_ref (entry2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry2", entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry2);
    gtk_fixed_put (GTK_FIXED (fixed4), entry2, 16, 24);
    gtk_widget_set_uposition (entry2, 16, 24);
    gtk_widget_set_usize (entry2, 192, 24);
    gtk_entry_set_visibility (GTK_ENTRY (entry2), FALSE);

    label4 = gtk_label_new (("Password:"));
    gtk_widget_ref (label4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label4", label4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label4);
    gtk_fixed_put (GTK_FIXED (fixed4), label4, 16, 8);
    gtk_widget_set_uposition (label4, 16, 8);
    gtk_widget_set_usize (label4, 72, 16);

    checkbutton1 = gtk_check_button_new_with_label (("OS Authentication"));
    gtk_widget_ref (checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton1", checkbutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed4), checkbutton1, 16, 48);
    gtk_widget_set_uposition (checkbutton1, 16, 48);
    gtk_widget_set_usize (checkbutton1, 192, 24);

    fixed6 = gtk_fixed_new ();
    gtk_widget_ref (fixed6);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed6", fixed6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed6);
    gtk_table_attach (GTK_TABLE (table1), fixed6, 1, 2, 2, 3,
		      (GtkAttachOptions) (GTK_FILL),
		      (GtkAttachOptions) (GTK_FILL), 0, 0);

    frame2 = gtk_frame_new (("Temporary Tablespace"));
    gtk_widget_ref (frame2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame2", frame2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame2);
    gtk_table_attach (GTK_TABLE (table1), frame2, 1, 2, 1, 2,
		      (GtkAttachOptions) (GTK_FILL),
		      (GtkAttachOptions) (GTK_FILL), 0, 0);

    fixed8 = gtk_fixed_new ();
    gtk_widget_ref (fixed8);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed8", fixed8,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed8);
    gtk_container_add (GTK_CONTAINER (frame2), fixed8);

    combo2 = gtk_combo_new ();
    gtk_widget_ref (combo2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo2", combo2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo2);
    gtk_fixed_put (GTK_FIXED (fixed8), combo2, 16, 16);
    gtk_widget_set_uposition (combo2, 16, 16);
    gtk_widget_set_usize (combo2, 256, 24);

    combo_entry2 = GTK_COMBO (combo2)->entry;
    gtk_widget_ref (combo_entry2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry2", combo_entry2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry2);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed1", fixed1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_table_attach (GTK_TABLE (table1), fixed1, 0, 1, 0, 1,
		      (GtkAttachOptions) (GTK_FILL),
		      (GtkAttachOptions) (GTK_FILL), 0, 0);

    entry1 = gtk_entry_new ();
    gtk_widget_ref (entry1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry1", entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry1);
    gtk_fixed_put (GTK_FIXED (fixed1), entry1, 16, 24);
    gtk_widget_set_uposition (entry1, 16, 24);
    gtk_widget_set_usize (entry1, 192, 24);

    label3 = gtk_label_new (("User Name:"));
    gtk_widget_ref (label3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label3", label3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label3);
    gtk_fixed_put (GTK_FIXED (fixed1), label3, 16, 8);
    gtk_widget_set_uposition (label3, 16, 8);
    gtk_widget_set_usize (label3, 80, 16);

    frame1 = gtk_frame_new (("Default Tablespace"));
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame1", frame1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_table_attach (GTK_TABLE (table1), frame1, 0, 1, 1, 2,
		      (GtkAttachOptions) (GTK_FILL),
		      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    fixed7 = gtk_fixed_new ();
    gtk_widget_ref (fixed7);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed7", fixed7,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed7);
    gtk_container_add (GTK_CONTAINER (frame1), fixed7);

    entry4 = gtk_entry_new ();
    gtk_widget_ref (entry4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "entry4", entry4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (entry4);
    gtk_fixed_put (GTK_FIXED (fixed7), entry4, 96, 72);
    gtk_widget_set_uposition (entry4, 96, 72);
    gtk_widget_set_usize (entry4, 72, 24);

    label5 = gtk_label_new (("Quota:"));
    gtk_widget_ref (label5);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label5", label5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label5);
    gtk_fixed_put (GTK_FIXED (fixed7), label5, 16, 48);
    gtk_widget_set_uposition (label5, 16, 48);
    gtk_widget_set_usize (label5, 80, 24);

    combo1 = gtk_combo_new ();
    gtk_widget_ref (combo1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo1", combo1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo1);
    gtk_fixed_put (GTK_FIXED (fixed7), combo1, 16, 16);
    gtk_widget_set_uposition (combo1, 16, 16);
    gtk_widget_set_usize (combo1, 272, 24);

    combo_entry1 = GTK_COMBO (combo1)->entry;
    gtk_widget_ref (combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry1", combo_entry1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry1);

    checkbutton2 = gtk_check_button_new_with_label (("Unlimited"));
    gtk_widget_ref (checkbutton2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton2", checkbutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton2);
    gtk_fixed_put (GTK_FIXED (fixed7), checkbutton2, 96, 48);
    gtk_widget_set_uposition (checkbutton2, 96, 48);
    gtk_widget_set_usize (checkbutton2, 80, 24);

    radiobutton1 = gtk_radio_button_new_with_label (fixed7_group, ("M"));
    fixed7_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
    gtk_widget_ref (radiobutton1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton1", radiobutton1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (radiobutton1);
    gtk_fixed_put (GTK_FIXED (fixed7), radiobutton1, 168, 72);
    gtk_widget_set_uposition (radiobutton1, 168, 72);
    gtk_widget_set_usize (radiobutton1, 40, 24);

    radiobutton2 = gtk_radio_button_new_with_label (fixed7_group, ("K"));
    fixed7_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
    gtk_widget_ref (radiobutton2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "radiobutton2", radiobutton2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton2), TRUE);
    gtk_widget_show (radiobutton2);
    gtk_fixed_put (GTK_FIXED (fixed7), radiobutton2, 208, 72);
    gtk_widget_set_uposition (radiobutton2, 208, 72);
    gtk_widget_set_usize (radiobutton2, 40, 24);

    fixed9 = gtk_fixed_new ();
    gtk_widget_ref (fixed9);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed9", fixed9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed9);
    gtk_table_attach (GTK_TABLE (table1), fixed9, 0, 1, 2, 3,
		      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
		      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    combo3 = gtk_combo_new ();
    gtk_widget_ref (combo3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo3", combo3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo3);
    gtk_fixed_put (GTK_FIXED (fixed9), combo3, 16, 24);
    gtk_widget_set_uposition (combo3, 16, 24);
    gtk_widget_set_usize (combo3, 280, 24);

    combo_entry3 = GTK_COMBO (combo3)->entry;
    gtk_widget_ref (combo_entry3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry3", combo_entry3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry3);

    label6 = gtk_label_new (("Profile:"));
    gtk_widget_ref (label6);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label6", label6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label6);
    gtk_fixed_put (GTK_FIXED (fixed9), label6, 16, 8);
    gtk_widget_set_uposition (label6, 16, 8);
    gtk_widget_set_usize (label6, 104, 16);
    gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);

    label1 = gtk_label_new (("General"));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label1", label1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_ref (hbox1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbox1", hbox1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox1);
    gtk_container_add (GTK_CONTAINER (notebook1), hbox1);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "vbox2", vbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox2);
    gtk_box_pack_start (GTK_BOX (hbox1), vbox2, TRUE, TRUE, 0);

    hbuttonbox3 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox3", hbuttonbox3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox3);
    gtk_box_pack_start (GTK_BOX (vbox2), hbuttonbox3, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox3), GTK_BUTTONBOX_START);

    button3 = pixmap_button (create_button_pixmap(BUTTON_PIX_STORAGE),"Admin Flag");
    gtk_widget_ref (button3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button3", button3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button3);
    gtk_container_add (GTK_CONTAINER (hbuttonbox3), button3);

    button4 = pixmap_button (create_button_pixmap(BUTTON_PIX_REMOVE),"Remove");
    gtk_widget_ref (button4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button4", button4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button4);
    gtk_container_add (GTK_CONTAINER (hbuttonbox3), button4);

    scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow2", scrolledwindow2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow2);
    gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow2, TRUE, TRUE, 0);

    createuser_clist1 = gtk_clist_new (2);
    gtk_widget_ref (createuser_clist1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "createuser_clist1", createuser_clist1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (createuser_clist1);
    gtk_container_add (GTK_CONTAINER (scrolledwindow2), createuser_clist1);
    gtk_clist_set_column_width (GTK_CLIST (createuser_clist1), 0, 80);
    gtk_clist_set_column_width (GTK_CLIST (createuser_clist1), 1, 80);
    gtk_clist_column_titles_show (GTK_CLIST (createuser_clist1));

    label11 = gtk_label_new (("Admin"));
    gtk_widget_ref (label11);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label11", label11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label11);
    gtk_clist_set_column_widget (GTK_CLIST (createuser_clist1), 0, label11);

    label12 = gtk_label_new (("Permission"));
    gtk_widget_ref (label12);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label12", label12,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label12);
    gtk_clist_set_column_widget (GTK_CLIST (createuser_clist1), 1, label12);

    notebook2 = gtk_notebook_new ();
    gtk_widget_ref (notebook2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "notebook2", notebook2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notebook2);
    gtk_box_pack_start (GTK_BOX (hbox1), notebook2, TRUE, TRUE, 0);

    scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow3", scrolledwindow3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow3);
    gtk_container_add (GTK_CONTAINER (notebook2), scrolledwindow3);

    clist2 = gtk_clist_new (1);
    gtk_widget_ref (clist2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "clist2", clist2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist2);
    gtk_container_add (GTK_CONTAINER (scrolledwindow3), clist2);
    gtk_clist_set_column_width (GTK_CLIST (clist2), 0, 200);
    gtk_clist_column_titles_hide (GTK_CLIST (clist2));
    
    query = g_malloc(strlen(privtemplate) + 1);
    sprintf(query, privtemplate);
    /* GTK WARNING and FATAL */
    set_clist_from_query(GTK_CLIST(clist2), query, dbid, 0, NULL);
    g_free(query);

    label13 = gtk_label_new (("label13"));
    gtk_widget_ref (label13);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label13", label13,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label13);
    gtk_clist_set_column_widget (GTK_CLIST (clist2), 0, label13);

    label9 = gtk_label_new (("Privileges"));
    gtk_widget_ref (label9);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label9", label9,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label9);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 0), label9);

    scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow4", scrolledwindow4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow4);
    gtk_container_add (GTK_CONTAINER (notebook2), scrolledwindow4);

    clist3 = gtk_clist_new (1);
    gtk_widget_ref (clist3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "clist3", clist3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist3);
    gtk_container_add (GTK_CONTAINER (scrolledwindow4), clist3);
    gtk_clist_set_column_width (GTK_CLIST (clist3), 0, 200);
    gtk_clist_column_titles_hide (GTK_CLIST (clist3));
    
    query = g_malloc(strlen(roletemplate) + 1);
    sprintf(query, roletemplate);
    /* GTK WARNING and FATAL */
    set_clist_from_query(GTK_CLIST(clist3), query, dbid, 0, NULL);
    g_free(query);

    label15 = gtk_label_new (("label15"));
    gtk_widget_ref (label15);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label15", label15,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label15);
    gtk_clist_set_column_widget (GTK_CLIST (clist3), 0, label15);

    vbox3 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox3);
    gtk_object_set_data_full (GTK_OBJECT (window1), "vbox3", vbox3,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox3);
    gtk_container_add (GTK_CONTAINER (notebook2), vbox3);

    scrolledwindow6 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow6);
    gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow6", scrolledwindow6,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow6);
    gtk_container_add (GTK_CONTAINER (vbox3), scrolledwindow6);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow6), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    label10 = gtk_label_new (("Roles"));
    gtk_widget_ref (label10);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label10", label10,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label10);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 1), label10);

    label2 = gtk_label_new (("Permissions"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

    label11 = gtk_label_new (("Quotas"));
    gtk_widget_ref (label11);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label11", label11,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label11);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 2), label11);

    hbuttonbox2 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox2", hbuttonbox2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox2);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox2, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_SPREAD);

    button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Execute");
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_container_add (GTK_CONTAINER (hbuttonbox2), button1);
    GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

    button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_CANCEL),"Cancel");
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_container_add (GTK_CONTAINER (hbuttonbox2), button2);
    GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

    createuser_text1 = add_sql_window(vbox1);

    scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow5);
    gtk_object_set_data_full (GTK_OBJECT (window1), "scrolledwindow5", scrolledwindow5,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow5);
    gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow5, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    createuser_clist2 = gtk_clist_new (2);
    gtk_widget_ref (createuser_clist2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "createuser_clist2", createuser_clist2,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (createuser_clist2);
    gtk_container_add (GTK_CONTAINER (scrolledwindow5), createuser_clist2);
    gtk_clist_set_column_width (GTK_CLIST (createuser_clist2), 0, 5);
    gtk_clist_set_column_width (GTK_CLIST (createuser_clist2), 1, 200);
    gtk_clist_column_titles_show (GTK_CLIST (createuser_clist2));

    clist4 = gtk_clist_new (2);
    gtk_widget_ref (clist4);
    gtk_object_set_data_full (GTK_OBJECT (window1), "clist4", clist4,
			      (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (clist4);
    gtk_container_add (GTK_CONTAINER (scrolledwindow6), clist4);
    gtk_clist_set_column_width (GTK_CLIST (clist4), 0, 100);
    gtk_clist_set_column_width (GTK_CLIST (clist4), 1, 100);
    gtk_clist_column_titles_show (GTK_CLIST (clist4));

    set_clist_from_query(GTK_CLIST(clist4), quotatemplate, dbid, 0, NULL);

    gtk_signal_connect (GTK_OBJECT (entry2), "changed",
			GTK_SIGNAL_FUNC (on_create_user_password_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry2), "changed",
			GTK_SIGNAL_FUNC (on_create_user_ttablespace_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (entry1), "changed",
			GTK_SIGNAL_FUNC (on_create_user_username_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (entry4), "changed",
			GTK_SIGNAL_FUNC (on_create_user_quotaval_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry1), "changed",
			GTK_SIGNAL_FUNC (on_create_user_dtablespace_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton2), "toggled",
			GTK_SIGNAL_FUNC (on_create_user_quota_unlimited_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (checkbutton1), "toggled",
			GTK_SIGNAL_FUNC (on_create_user_os_password_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
			GTK_SIGNAL_FUNC (on_create_user_quoteM_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
			GTK_SIGNAL_FUNC (on_create_user_quotaK_toggled),
			NULL);
    gtk_signal_connect (GTK_OBJECT (combo_entry3), "changed",
			GTK_SIGNAL_FUNC (on_create_user_profile_changed),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button3), "clicked",
			GTK_SIGNAL_FUNC (on_create_user_adminflag_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button4), "clicked",
			GTK_SIGNAL_FUNC (on_create_user_removeperm_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (createuser_clist1), "select_row",
			GTK_SIGNAL_FUNC (on_create_user_added_click_column),
			NULL);
    gtk_signal_connect (GTK_OBJECT (clist2), "select_row",
			GTK_SIGNAL_FUNC (on_create_user_priv_click_column),
			NULL);
    gtk_signal_connect (GTK_OBJECT (clist3), "select_row",
			GTK_SIGNAL_FUNC (on_create_user_roles_click_column),
			NULL);
    gtk_signal_connect (GTK_OBJECT (clist4), "select_row",
			GTK_SIGNAL_FUNC (on_quota_click_column),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked",
			GTK_SIGNAL_FUNC (create_user_execute_clicked),
			NULL);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked",
			GTK_SIGNAL_FUNC (on_create_user_cancel_clicked),
			window1);

    user.username            =NULL;
    user.password            =NULL;
    user.external_password   =0;
    user.dtablespace         =NULL;
    user.ttablespace         =NULL;
    user.profile             =NULL;
    user.quotaval            =NULL;
    user.quotavalM           =0;
    user.quotaval_unlimited  =1;

    if(userflag){
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton2), TRUE);
	/* Dupe Call */
        generic_populate_tablespace(combo1,combo_entry1, obj_cinfo.login->dbid);
        generic_populate_tablespace(combo2,combo_entry2, obj_cinfo.login->dbid);
        generic_populate_profile(combo3,combo_entry3, obj_cinfo.login->dbid);
    }else{
        gtk_widget_hide(frame2);
	gtk_widget_hide(frame1);
	gtk_widget_hide(label6);
	gtk_widget_hide(combo3);
	gtk_widget_hide(clist4);
    }

    if(userlist!=NULL){
        char    *xfervar;
        gtk_clist_get_text(GTK_CLIST(userlist), 0, 0, &xfervar);
	gtk_entry_set_text(GTK_ENTRY(entry1), xfervar);
	gtk_clist_get_text(GTK_CLIST(userlist), 0, 1, &xfervar);
	if(!strncmp(xfervar,"EXTERNAL",8))
	    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton1), TRUE);
	if(userflag){
	    gtk_clist_get_text(GTK_CLIST(userlist), 0, 2, &xfervar);
	    gtk_entry_set_text(GTK_ENTRY(combo_entry1), xfervar);
	    gtk_clist_get_text(GTK_CLIST(userlist), 0, 3, &xfervar);
	    gtk_entry_set_text(GTK_ENTRY(combo_entry2), xfervar);
	    gtk_clist_get_text(GTK_CLIST(userlist), 0, 4, &xfervar);
	    gtk_entry_set_text(GTK_ENTRY(combo_entry3), xfervar);
	}
	gtk_widget_destroy(GTK_WIDGET(userlist));
	user.alterflag = 1;
    }else
        user.alterflag = 0;
    
    if(privlist!=NULL){
        char    *privilege;
	char    *adminflag;
	char    *xfervar[2];
	int     a=0;
	
	while(a < GTK_CLIST(privlist)->rows){
            gtk_clist_get_text(GTK_CLIST(privlist), a, 0, &privilege);
	    gtk_clist_get_text(GTK_CLIST(privlist), a, 1, &adminflag);
	    if(!strncmp(adminflag,"NO",2))
	        xfervar[0]="No";
	    else
	        xfervar[0]="Yes";
	    xfervar[1]=privilege;
	    gtk_clist_append(GTK_CLIST(createuser_clist1),xfervar);
	    a++;
        }
	gtk_widget_destroy(GTK_WIDGET(privlist));
    }
	
    if(rolelist!=NULL){
        char    *privilege;
	char    *adminflag;
	char    *xfervar[2];
	int     a=0;
	
	while(a < GTK_CLIST(rolelist)->rows){
            gtk_clist_get_text(GTK_CLIST(rolelist), a, 0, &privilege);
	    gtk_clist_get_text(GTK_CLIST(rolelist), a, 1, &adminflag);
	    if(!strncmp(adminflag,"NO",2))
	        xfervar[0]="No";
	    else
	        xfervar[0]="Yes";
	    xfervar[1]=privilege;
	    gtk_clist_append(GTK_CLIST(createuser_clist1),xfervar);
	    a++;
        }
	gtk_widget_destroy(GTK_WIDGET(rolelist));
    }

    if(quotalist!=NULL){
        char    *tablespace;
	char    *quotasize;
	char    *userperm[2];
	int     a=0, quotanum=0;
	
	while(a < GTK_CLIST(quotalist)->rows){
            gtk_clist_get_text(GTK_CLIST(quotalist), a, 0, &tablespace);
	    gtk_clist_get_text(GTK_CLIST(quotalist), a, 1, &quotasize);
	    
	    userperm[0]="Quota";
            userperm[1] = g_malloc(strlen(tablespace) + 24);
            quotanum = atoi(quotasize);

            switch (quotanum) {
	        case -1:
	            sprintf(userperm[1], "UNLIMITED ON %s",tablespace);
                    gtk_clist_append(GTK_CLIST(createuser_clist1),userperm);
                    break;
	        case 0:
	            sprintf(userperm[1], "NONE ON %s",tablespace);
	            gtk_clist_append(GTK_CLIST(createuser_clist1),userperm);
	            break;
	        default:
		    quotanum /= 1024;
	            sprintf(userperm[1], "%d K ON %s",quotanum,tablespace);
	            gtk_clist_append(GTK_CLIST(createuser_clist1),userperm);
            }	

            g_free(userperm[1]);
	    a++;
        }
	
	gtk_widget_destroy(GTK_WIDGET(quotalist));
    }

    build_createuser();

    return window1;
}




static void
on_createuser_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_show(create_db_user (1,NULL,NULL,NULL,NULL));
    gtk_widget_destroy(user_data);
}


static void
on_createrole_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_show(create_db_user (0,NULL,NULL,NULL,NULL));
    gtk_widget_destroy(user_data);
}



GtkWidget*
create_security_object (void)
{
  GtkWidget *window1;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkWidget *button2;

  window1 = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
  gtk_window_set_title (GTK_WINDOW (window1), "Create Security Object");
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (window1), TRUE);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_container_add (GTK_CONTAINER (window1), hbuttonbox1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Create User");
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  button2 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK),"Create Role");
  gtk_widget_ref (button2);
  gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (button1), "clicked",
                      GTK_SIGNAL_FUNC (on_createuser_clicked),
                      window1);
  gtk_signal_connect (GTK_OBJECT (button2), "clicked",
                      GTK_SIGNAL_FUNC (on_createrole_clicked),
                      window1);

  return window1;
}



void
on_quota_none_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    response = 0;
    gtk_widget_set_sensitive(user_data,FALSE);
}


void
on_quota_unlimited_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    response = -1;
    gtk_widget_set_sensitive(user_data,FALSE);
}


void
on_quota_limited_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    response = 1;
    gtk_widget_set_sensitive(user_data,TRUE);
}


void
on_quota_add_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

    if(response == 1){
        char *entry=NULL;
        entry=gtk_entry_get_text(GTK_ENTRY(user_data));
        response = atoi(entry);
    }
    gtk_widget_destroy(GTK_WIDGET(quota_window));
    gtk_main_quit();
}

static void
on_dialog_destroyed(
    GtkButton       *button,
    gpointer         user_data)
{

    gtk_widget_destroy(GTK_WIDGET(quota_window));
    gtk_main_quit();
}


GtkWidget*
create_quota_selection ()
{
  GtkWidget *hbox2;
  GSList *g1_group = NULL;
  GtkWidget *radiobutton1;
  GtkWidget *radiobutton2;
  GtkWidget *hbox3;
  GtkWidget *radiobutton3;
  GtkWidget *entry1;
  GtkWidget *button1;

  quota_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (quota_window), "quota_window", quota_window);
  gtk_window_set_title (GTK_WINDOW (quota_window), ("Quota Selection"));
  gtk_window_set_position (GTK_WINDOW (quota_window), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (quota_window), TRUE);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox2);
  gtk_object_set_data_full (GTK_OBJECT (quota_window), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (quota_window), hbox2);

  radiobutton1 = gtk_radio_button_new_with_label (g1_group, ("None"));
  g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
  gtk_widget_ref (radiobutton1);
  gtk_object_set_data_full (GTK_OBJECT (quota_window), "radiobutton1", radiobutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (hbox2), radiobutton1, FALSE, FALSE, 0);

  radiobutton2 = gtk_radio_button_new_with_label (g1_group, ("Unlimited"));
  g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
  gtk_widget_ref (radiobutton2);
  gtk_object_set_data_full (GTK_OBJECT (quota_window), "radiobutton2", radiobutton2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton2);
  gtk_box_pack_start (GTK_BOX (hbox2), radiobutton2, FALSE, FALSE, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (quota_window), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (hbox2), hbox3, TRUE, TRUE, 0);

  radiobutton3 = gtk_radio_button_new_with_label (g1_group, ("Limited (in K):"));
  g1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
  gtk_widget_ref (radiobutton3);
  gtk_object_set_data_full (GTK_OBJECT (quota_window), "radiobutton3", radiobutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton3);
  gtk_box_pack_start (GTK_BOX (hbox3), radiobutton3, FALSE, FALSE, 0);

  entry1 = gtk_entry_new ();
  gtk_widget_ref (entry1);
  gtk_object_set_data_full (GTK_OBJECT (quota_window), "entry1", entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry1);
  gtk_box_pack_start (GTK_BOX (hbox3), entry1, TRUE, TRUE, 0);
  gtk_widget_set_sensitive(entry1, FALSE);

  button1 = pixmap_button (create_button_pixmap(BUTTON_PIX_OK)," Add ");
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (quota_window), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_box_pack_start (GTK_BOX (hbox2), button1, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (radiobutton1), "toggled",
                      GTK_SIGNAL_FUNC (on_quota_none_toggled),
                      entry1);
  gtk_signal_connect (GTK_OBJECT (radiobutton2), "toggled",
                      GTK_SIGNAL_FUNC (on_quota_unlimited_toggled),
                      entry1);
  gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
                      GTK_SIGNAL_FUNC (on_quota_limited_toggled),
                      entry1);
  gtk_signal_connect (GTK_OBJECT (button1), "clicked",
                      GTK_SIGNAL_FUNC (on_quota_add_clicked),
                      entry1);
  gtk_signal_connect(GTK_OBJECT(quota_window), "destroy",
                       GTK_SIGNAL_FUNC(on_dialog_destroyed),
                       (gpointer)quota_window);

  return quota_window;
}


int
get_quota()
{
    GtkWidget   *dialog;

    response = 0;
    dialog = create_quota_selection();
    gtk_widget_show(dialog);
    gtk_main();
    return response;
}

