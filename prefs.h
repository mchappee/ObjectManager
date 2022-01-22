/* SQLWork - the Oracle Query tool
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

#define PREF_NAME      "name"
#define PREF_LOGIN     "login"
#define PREF_EXEPATH   "exepath"
#define PREF_ICON      "icon"
#define PREF_ICONPATH  "iconpath"
#define PREF_TBSTATE   "tbstate"
#define PREF_LOGGING   "logging"
#define PREF_SYNTAX    "syntax"
#define PREF_MODULAR   "modular"
#define PREF_ROWS      "rows"
#define PREF_PRINTCALL "printcall"

extern int   prefs_get_int_var(const char *varname);
extern char *prefs_get_char_var(const char *varname);

extern void  prefs_set_int_var(const char *varname, int value);
extern void  prefs_set_char_var(const char *varname, const char *value);

extern void  prefs_unset_char_var(const char *varname);

extern void  prefs_save_values(void);

