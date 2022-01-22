/* GtkEditor - a source editor widget for GTK
 * Copyright (C) 1998, 2000 Thomas Mailund.
 *
 * The editor widget was written by Thomas Mailund, so bugs should be
 * reported to <mailund@daimi.au.dk>, not the gtk ppl.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "gtkeditor.h"
#include "internal.h"
#include "edit.h"
#include "scanner.h"

#define NOT_DONE 1
#define DONE 1 << 1

/* pointer to current editor being analized */
static GtkEditor *g_editor;

static const char *g_insertion_text;

/* tells if we are done yet */
static int g_lex_flag;

/* points to character position in the new text */
static int g_char_pos;

/* points to current position in the new text */
static int g_pos, g_offset;

static int g_state, g_unfinished;

/* length of the buffer plus inserted text */
static unsigned int g_length;

static GList *g_prev_token;

/*
 * prototypes
 */
static int            next_char            ();
static void           new_token            (int id,
                                            int precedence,
                                            char *text,
                                            int length,
                                            int pos,
                                            int block_id,
                                            int nesting,
                                            int begins_block);
static void copy_global_text (char *buffer, int pos, int length);
					
/* gtk_editor_compile_scanner

   A flex scanner is generated to search for the highlighted
   entries. The scanner is compiled into a shared library which can be
   loaded at a later time.
*/

/* gtk_editor_load_scanner

   Loads a previously compiled flex scanner.
*/
GtkEditorScanner *
gtk_editor_load_scanner (const char *scanner_name)
{
  GtkEditorScanner *scanner;
  GModule *scanner_lib;

  char *home_path;
  char buf[256];

  /* we search a number of places, and after a number of names.  The
   * scanners we create with the distribution has suffix .so, so we
   * try to load both 'scanner_name' and 'scanner_name.so'.  We always
   * try 'scanner_name' first and 'scanner_name.so' last.
   *
   * we search in ~/.gtkeditor/scanners, <libpath>gtkeditor, and
   * <libpath>, in that order.
   */

  /* first look in ~/.gtkeditor/scanners */
  home_path = getenv("HOME");
  if (home_path) {
    strncpy (buf, home_path, 256);
    strncat (buf, "/.gtkeditor/scanners/", 256);
    strncat (buf, scanner_name, 256);
    scanner_lib = g_module_open (buf, G_MODULE_BIND_LAZY);
    if (scanner_lib) goto get_symbols; /* got it */

    /* nope, didn't get it.  We try to append suffix */
    strncat (buf, ".so", 256);
    scanner_lib = g_module_open (buf, G_MODULE_BIND_LAZY);
    if (scanner_lib) goto get_symbols; /* got it */
  }

  /* ok, then try in <libpath>/gtkeditor */
  strncpy (buf, "gtkeditor/", 256);
  strncat (buf, scanner_name, 256);
  scanner_lib = g_module_open (buf, G_MODULE_BIND_LAZY);
  if (scanner_lib) goto get_symbols; /* got it */
  strncat (buf, ".so", 256);
  scanner_lib = g_module_open (buf, G_MODULE_BIND_LAZY);
  if (scanner_lib) goto get_symbols; /* got it */

  /* finaly, we just try in <libpath> */

  strncpy (buf, scanner_name, 256);
  scanner_lib = g_module_open (buf, G_MODULE_BIND_LAZY);
  if (scanner_lib) goto get_symbols; /* got it */
  strncat (buf, ".so", 256);
  scanner_lib = g_module_open (buf, G_MODULE_BIND_LAZY);
  if (scanner_lib) goto get_symbols; /* got it */

  /* if we reach this point, we couldn't find the scanner */
  g_warning ("error loading scanner!\n");
  return NULL;

 get_symbols:
  scanner = g_new(GtkEditorScanner, 1);
  if(!g_module_symbol(scanner_lib, "get_name", (gpointer)&scanner->get_name)
     || !g_module_symbol(scanner_lib, "get_token_names", (gpointer)&scanner->get_token_names)
     || !g_module_symbol(scanner_lib, "get_block_names", (gpointer)&scanner->get_block_names)
     || !g_module_symbol(scanner_lib, "get_token_no", (gpointer)&scanner->get_token_no)
     || !g_module_symbol(scanner_lib, "get_block_no", (gpointer)&scanner->get_block_no)
     || !g_module_symbol(scanner_lib, "token_func", (gpointer)&scanner->token_func)
     || !g_module_symbol(scanner_lib, "char_func", (gpointer)&scanner->char_func)
     || !g_module_symbol(scanner_lib, "get_state", (gpointer)&scanner->get_state)
     || !g_module_symbol(scanner_lib, "set_state", (gpointer)&scanner->set_state)
     || !g_module_symbol(scanner_lib, "lex", (gpointer)&scanner->lex)) {
    g_free(scanner);
    g_warning ("error loading scanner!");
    return NULL;
  }

  return scanner;
}

GtkEditorScanner *
gtk_editor_static_scanner (const char *(*get_name)(),
                           const char **(*get_token_names)(),
                           const char **(*get_block_names)(),
                           int (*get_token_no)(), int (*get_block_no)(),
                           void (*token_func)(void (*func)(int id, int precedence, char *text,
                           int length, int pos, int block_id, int nesting, int block_type)),
                           void (*char_func)(int (*func)()),
                           int (*get_state)(), void (*set_state)(int state),
                           int (*lex)())
{
  GtkEditorScanner *scanner;

  scanner = g_new(GtkEditorScanner, 1);

  scanner->get_name = get_name;
  scanner->get_token_names = get_token_names;
  scanner->get_block_names = get_block_names;
  scanner->get_token_no = get_token_no;
  scanner->get_block_no = get_block_no;
  scanner->token_func = token_func;
  scanner->char_func = char_func;
  scanner->get_state = get_state;
  scanner->set_state = set_state;
  scanner->lex = lex;

  return scanner;
}

void
gtk_editor_install_scanner (GtkEditor *editor,
			    GtkEditorScanner *scanner)
{
  const char **token_names;
  const char **block_names;
  int i;

  if (scanner == NULL)
    return;

  editor->scanner = scanner;

  block_names = scanner->get_block_names();

  if (block_names) {
    for(i = 0; block_names[i] != NULL; i++);

    editor->n_block_entries = i;
    editor->block_entries = g_new (GtkEditorBlockEntry, editor->n_block_entries);

    for (i = 0; i < editor->n_block_entries; i++) {
      editor->block_entries[i].name = g_strdup (block_names[i]);
      editor->block_entries[i].prop_id = -1;
    }
  } else {
    editor->block_entries = NULL;
    editor->n_block_entries = 0;
  }

  token_names = scanner->get_token_names();

  if (token_names) {
    for(i = 0; token_names[i] != NULL; i++);

    editor->n_token_entries = i;
    editor->token_entries = g_new (GtkEditorTokenEntry, editor->n_token_entries);

    for (i = 0; i < editor->n_token_entries; i++) {
      editor->token_entries[i].name = g_strdup (token_names[i]);
      editor->token_entries[i].prop_id = -1;
    }
  } else {
    editor->token_entries = NULL;
    editor->n_token_entries = 0;
  }

  scanner->token_func(new_token);
  scanner->char_func(next_char);
}

void gtk_editor_uninstall_scanner (GtkEditor *editor)
{
  GtkEditorToken *token, *temp_token;

  editor->n_text_properties = 0;
  editor->n_token_entries = 0;
  editor->n_block_entries = 0;

  if(editor->text_properties)
    g_free(editor->text_properties);

  if(editor->token_entries)
    g_free(editor->token_entries);

  if(editor->block_entries)
    g_free(editor->block_entries);

  editor->scanner = NULL;

  for (token = editor->tokens; token != NULL;) {
    temp_token = token;
    token = token->next;
    g_free (token);
  }

  editor->tokens = NULL;
  editor->cur_token = NULL;
  editor->cur_token_pos = 0;
  editor->block_stack = NULL;
}

void
_gtk_editor_scan (GtkEditor *editor, int pos,
      const char *insertion_text,
      int length, int insertion_point)
{
  int text_length;

  g_prev_token = NULL;
  g_editor = editor;
  g_lex_flag = NOT_DONE;
  g_insertion_point = insertion_point;
  g_length = text_length = gtk_text_get_length (GTK_TEXT(editor));
  if(length >= 0) {
    g_insertion_text = insertion_text;
    g_insertion_length = length;
    g_deletion_length = 0;
  } else {
    g_insertion_text = NULL;
    g_insertion_length = 0;
    g_deletion_length = -length;
  }

  g_length += length;
  g_offset = g_pos = g_char_pos = pos;
  g_state = g_token->state;
  editor->scanner->set_state(g_state);
  g_unfinished = FALSE;
  editor->scanner->lex();

  if (g_unfinished && g_pos < g_length && (text_length == 0 || g_pos <= insertion_point)) {
    /* unmatched text */
    char *buffer;
    char s_buffer[1024];
    int l = g_length - g_pos;

    if (l > 1024)
      buffer = g_new(char, l);
    else
      buffer = s_buffer;

    copy_global_text (buffer, g_pos, l);
    _gtk_editor_merge_token(-1, 0, buffer, l, g_pos, -1, 0, 0, g_state);

    if (buffer != s_buffer)
      g_free (buffer);
  }
}

static int
next_char()
{
  int c;

  if (g_lex_flag == DONE)
    c = EOF;
  else if (g_char_pos >= g_length) {
    c = EOF;
    g_unfinished = TRUE;
  } else if (g_insertion_text
           && (g_char_pos >= g_insertion_point)
           && (g_char_pos < g_insertion_point + g_insertion_length))
    c = g_insertion_text[g_char_pos - g_insertion_point];
  else if (g_char_pos >= g_insertion_point)
    c = GTK_TEXT_INDEX(GTK_TEXT(g_editor), g_char_pos - g_insertion_length + g_deletion_length);
  else
    c = GTK_TEXT_INDEX(GTK_TEXT(g_editor), g_char_pos);

  g_char_pos++;
  return c;
}

/* new_token

  This function is called by flex when flex completes a token. It inserts the
  tokens into the global editor with the properties specified. The algorithm
  trys to resuse existing tokens in the editor, and only deletes and reinserts
  text when it has changed properties.

*/
static void
new_token(int id, int precedence, char *text, int length,
          int pos, int block_id, int nesting, int begins_block)
{
  char *buffer;
  char s_buffer[1024];

  if (g_lex_flag == DONE)
    /* nothing to do here */
    return;

  g_state = g_editor->scanner->get_state();

  /* check if no more lexing is needed */
  if (g_pos >= g_insertion_point + g_insertion_length && g_token->state == g_state) {
    if (((g_offset + pos) > g_pos)
        && g_token->length == ((g_offset + pos) - g_pos)
        && g_token->id == -1)
      g_lex_flag = DONE;
    else if (g_token->id == id && g_token->length == length)
      g_lex_flag = DONE;
  }

  if ((g_offset + pos) > g_pos) {
    /* unmatched text */
    int l = (g_offset + pos) - g_pos;

    if (l > 1024)
      buffer = g_new(char, l);
    else
      buffer = s_buffer;
    copy_global_text (buffer, g_pos, l);
    _gtk_editor_merge_token(-1, 0, buffer, l, g_pos, -1, 0, 0, g_state);

    if (buffer != s_buffer)
      g_free (buffer);

    g_pos += l;
  }

  /* merge token into the token list */
  _gtk_editor_merge_token(id, precedence, text, length, g_pos,
                          block_id, nesting, begins_block, g_state);

  g_pos += length;
}

static void
copy_global_text (char *buffer, int pos, int length)
{
  int len = 0, i_len = 0;
  GtkText *text = GTK_TEXT (g_editor);

  buffer[length] = '\0';
  if (pos < g_insertion_point) {
    if(pos +length > g_insertion_point)
      len = g_insertion_point - pos;
    else
      len = length;
    if (pos < text->gap_position) {
      if (pos + len < text->gap_position) {
        /* before gap */
        memcpy(buffer, text->text.ch + pos, len);

      } else {
        /* before and intersecting gap */
        int l = text->gap_position - pos;
        memcpy (buffer, text->text.ch + pos, l);
        memcpy (buffer+l,
                text->text.ch + text->gap_position + text->gap_size,
                len - l);
      }
    } else {
      /* starts inside or after gap */
      memcpy(buffer, text->text.ch + text->gap_size + pos, len);
    }
    pos += len;
  }

  if (pos >= g_insertion_point && pos < g_insertion_point + g_insertion_length) {
    if (pos+(length-len) > (g_insertion_point+g_insertion_length))
      i_len = g_insertion_point+g_insertion_length - pos;
    else
      i_len = length - len;

    memcpy(buffer+len, g_insertion_text+(pos-g_insertion_point), i_len);
    pos += i_len;
  }

  pos -= g_insertion_length;
  length -= len + i_len;
  pos += g_deletion_length;

  if (length) {
    if (pos < text->gap_position) {
      if (pos + length < text->gap_position) {
        /* before gap */
        memcpy(buffer+len+i_len, text->text.ch + pos, length);

      } else {
        /* before and intersecting gap */
        int l = text->gap_position - pos;
        memcpy (buffer+len+i_len, text->text.ch + pos, l);
        memcpy (buffer+len+i_len+l,
                text->text.ch + text->gap_position + text->gap_size,
                length - l);
      }
    } else {
      /* starts inside or after gap */
      memcpy(buffer+len+i_len, text->text.ch + text->gap_size + pos, length);
    }
  }
}

