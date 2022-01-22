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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "include/objectman.h"
#include "common.h"
#include "prefs.h"

static int	  prefs_initialised   = 0;
static char	  *prefs_file;
static GHashTable * value_hash_table  = 0;

typedef enum {
    PSRC_FILE = 1,
    PSRC_SET,
    PSRC_UNSET,
    PSRC_DEF} PefsSource;

typedef struct {
    char	 *val;
    PefsSource	 source;
} PrefsVarValue;

#define USER_CONFIG_DIR ".orasoft"
#define PREFS_FILE      "objectmanrc"
#define BOOKMARKS_DIR   "bookmarks"

static void
validate_rcfile(
    const char   *config_dir,
    const char   *config_file)
{
    struct stat s;
    FILE        *outf;
    int         status;
    
    if (stat(config_dir, &s) < 0) {
        if (errno == ENOENT) {
            mkdir(config_dir, 0700);
            outf = fopen(config_file, "w");
	    fprintf(outf,"syntax=1\nmodular=1\nrows=500\ntbstate=3\n");
	    fchmod(fileno(outf), 0600);
	    fclose(outf);
	    return;
        }
    }
    
    status = stat (config_file, &s);
    if (status == 0 && S_ISREG (s.st_mode)){
 	return;
    }else{
        outf = fopen(config_file, "w");
	fprintf(outf,"syntax=1\nmodular=1\nrows=500\ntbstate=3\n");
	fchmod(fileno(outf), 0600);
	fclose(outf);
    }

}


static void
prefs_set_char_var_internal (
    const char *varname,
    const char *value,
    PefsSource source)
{
    PrefsVarValue *oldval;
    
    oldval = g_hash_table_lookup (value_hash_table, varname);
    if (oldval) {
	if (strcmp(oldval->val, value) != 0) {
	    g_free(oldval->val);
	    oldval->val = g_strdup(value);
	    oldval->source = source;
	}
    } else {
	PrefsVarValue *val = g_malloc(sizeof(PrefsVarValue));

	val->val = g_strdup(value);
	val->source = source;
	g_hash_table_insert (value_hash_table, g_strdup(varname), val);
    }
}

static void
prefs_read_file(
    const char	 *filename,
    PefsSource	 source)
{
    FILE	 *inf	  = fopen(filename, "r");
    if (inf) {
	struct stat  st;
	char	     *filebuf;
	char	     *line;
	char	     *s;

	if (fstat(fileno(inf), &st) < 0) {
	    show_warning_msg("Can't stat open file %s, *highly* wierd!", 
			     filename);
	}
	if (st.st_size > 0) {
	    filebuf = g_malloc(st.st_size);
	    fread(filebuf, 1, st.st_size, inf);
	    for (line = filebuf; (line = strtok(line, "\n")) != 0; line = 0) {
		s = strchr(line, '=');
		if (s) {
		    *s = '\0';
		    prefs_set_char_var_internal(line, s+1, source);
		}		
	    }
	    g_free(filebuf);
	}
    }
}

static void
initialize_prefs()
{
    if (!prefs_initialised++) {
        char    *valdir;
	GString *prefsfile = g_string_new(user_home_dir());
	GString *bmkdir = g_string_new(user_home_dir());

	g_string_sprintfa(prefsfile, "%s%s%s%s", 
			  G_DIR_SEPARATOR_S, USER_CONFIG_DIR, 
			  G_DIR_SEPARATOR_S, PREFS_FILE);
	prefs_file = prefsfile->str;

	g_string_sprintfa(bmkdir, "%s%s%s%s", 
			  G_DIR_SEPARATOR_S, USER_CONFIG_DIR, 
			  G_DIR_SEPARATOR_S, BOOKMARKS_DIR);

        /* Matt Hack */
	valdir=g_malloc(strlen(user_home_dir()) + strlen(G_DIR_SEPARATOR_S) + strlen(USER_CONFIG_DIR) + 1);
	sprintf(valdir, "%s%s%s",user_home_dir(),G_DIR_SEPARATOR_S, USER_CONFIG_DIR);
	validate_rcfile(valdir, prefsfile->str);
	g_free(valdir);

	value_hash_table =  g_hash_table_new(g_str_hash, g_str_equal);
	
	prefs_read_file(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S PREFS_FILE, PSRC_DEF);
	prefs_read_file(prefsfile->str, PSRC_FILE);
	g_string_free(prefsfile, 0);
	g_string_free(bmkdir, 1);
    }
}


int
prefs_get_int_var(
    const char *varname)
{
    PrefsVarValue *val;
    initialize_prefs();

    val = g_hash_table_lookup (value_hash_table, varname);
    if (val)
	return atoi(val->val);
    else
	return 0;

}

char *
prefs_get_char_var(
    const char *varname)
{
    PrefsVarValue *val;

    initialize_prefs();
    val = g_hash_table_lookup (value_hash_table, varname);
    if (val)
	return val->val;
    else
	return 0;
}

void
prefs_unset_char_var(
    const char *varname)
{
    PrefsVarValue *val;

    initialize_prefs();
    val = g_hash_table_lookup (value_hash_table, varname);
}

void
prefs_set_int_var(
    const char *varname,
    int value)
{
    char val_as_str[16];
    initialize_prefs();
    snprintf(val_as_str, 16, "%d", value);
    prefs_set_char_var_internal(varname, val_as_str, PSRC_SET);
}

void
prefs_set_char_var(
    const char *varname,
    const char *value)
{
    initialize_prefs();
    prefs_set_char_var_internal(varname, value, PSRC_SET);
}

static void
write_prefs_val(
    gpointer	key,
    gpointer	value,
    gpointer	user_data)
{
    GString *str = (GString *)user_data;
    PrefsVarValue *v = (PrefsVarValue *)value;

    if (v->source != PSRC_DEF && v->source != PSRC_UNSET) {
	g_string_sprintfa(str, "%s=%s\n", (char *)key, v->val);
    }
}

/* Save the preferences out. Only those set by the user will
   be saved. Since the preferences file may contain password
   data make sure that it is only readable by the user */
void
prefs_save_values(void)
{
    if (prefs_initialised) {
	GString *str = g_string_new("");
	g_hash_table_foreach(value_hash_table,
			     write_prefs_val,
			     str);
	if (str->str[0]) {
	    FILE *outf = fopen(prefs_file, "w");

	    if (!outf) {
		/* If we can't open the file check whether that's
		   because the config dir doesn't exist and create
		   it */
		struct stat  st;
		GString	     *prefsdir	= g_string_new(user_home_dir());

		g_string_sprintfa(prefsdir, "%s%s", 
				  G_DIR_SEPARATOR_S, USER_CONFIG_DIR);
		if (stat(prefsdir->str, &st) < 0) {
		    if (errno == ENOENT) {
			mkdir(prefsdir->str, 0700);
			outf = fopen(prefs_file, "w");
		    }
		}
		g_string_free(prefsdir, 1);
	    }
	    if (outf) {
		fchmod(fileno(outf), 0600);
		fwrite(str->str, 1, str->len, outf);
		fclose(outf);
	    }
	}
	g_string_free(str, 1);
    }
}
