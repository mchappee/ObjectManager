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

GtkWidget *obj_create_window1 (void);
GtkWidget *obj_create_progressbar(void);

typedef struct obj_mainwin_str{
  GtkWidget *window1;
  GtkWidget *hbox1;
  GtkWidget *hbox2;
  GtkWidget *tree1;
  GtkWidget *statusbar1;
  GtkWidget *toolbar1;
  GtkWidget *scrolledwindow1;
  GtkWidget *notebook1;
  GtkWidget *dynamictoolbar1;
  GtkWidget *togglecache;
  GtkWidget *sessionclist2;
  GtkWidget *sessiontext1;
  GtkWidget *proctext1;
  GtkWidget *proctext2;
  GtkWidget *proctext3;
  GtkWidget *procentry3;
  GtkWidget *procbutton5;
  GtkWidget *procbutton6;
  GtkWidget *proclabelvalid;
  GtkWidget *proclabelchanged;
  GtkWidget *toggletrigger;
  GtkWidget *toolbarcombo1;
}obj_mainwin_struct;

typedef struct obj_progwin_str{
  GtkWidget *window4;
  GtkWidget *progressbar1;
  GtkWidget *label1;
}obj_progwin_struct;

extern obj_progwin_struct obj_progwin;
extern obj_mainwin_struct obj_mainwin;


