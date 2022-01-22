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

/* Definitions for common utility functions */

extern char *vmsgfmt(const gchar *msg, va_list args);

extern void export_as_csv(const char *fname, GtkCList *clist);
extern void export_as_sql(const char *fname, GtkCList *clist);
extern void export_as_html(const char *fname, GtkCList *clist);

/* errors.c */
extern void file_open_error(const char *fname, int e, char *rw);
extern void show_warning_msg(const char *msg, ...);
extern void fatal_error(const char *msg, ...); 

/* confirmwin.c */
/* pop up a dialog with a yes/no question, return true iff user says yes */
int confirm(const char *msg, ...);

/* filewin.c */
char *filewin(char *msg,...);
char *filewin_x(char *msg,char *extension,...);

/* dirbrowser */
char *obj_show_dirbrowser(void);

/* pixmap_button */
GtkWidget *pixmap_button(GtkWidget *pixmap, const char *text);
GtkWidget *create_button_pixmap(int a);

/* grant privilege */
GtkWidget    *
grant_privilege_win(
    char    *owner,
    char    *object,
    int     object_type,
    char    *dbid);

/* Search */
GtkWidget*
create_search_window (GtkText *textbox);

#define BUTTON_PIX_CANCEL   0
#define BUTTON_PIX_OK       1
#define BUTTON_PIX_BACK     2
#define BUTTON_PIX_STORAGE  3
#define BUTTON_PIX_ADD      4
#define BUTTON_PIX_REMOVE   5

