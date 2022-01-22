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
void
obj_on_clist10_select_row                  (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);
					
void
obj_on_entry10_changed                     (GtkEditable     *editable,
                                        gpointer         user_data);
					
void
obj_on_addcolumnwin_clicked                   (GtkButton       *button,
                                        gpointer         user_data);
					
void
obj_on_canceladd_clicked                   (GtkButton       *button,
                                        gpointer         user_data);
					
void
obj_on_entry11_changed                     (GtkEditable     *editable,
                                        gpointer         user_data);
					
void obj_buildsql(void);

