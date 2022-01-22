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

#include <glib.h>
#include <gtk/gtk.h>

#include "internal.h"
#include "edit.h"
#include "highlight.h"

typedef struct _PropertyChange {
  int id;
  unsigned int offset;
  unsigned int length;
}PropertyChange;

/*
 * globals
 */
/* for recycling */
static GSList *g_free_text_props = NULL;

static void copy_gtk_text (char *buffer, GtkText *text, int pos, int length);

void
gtk_editor_set_pattern_property (GtkEditor *editor, char *pattern_name,
                                 GdkFont *font, GdkColor *fore, GdkColor *back)
{
  int i;

  if (strcmp("default", pattern_name) == 0) {
    editor->text_properties[0].font = gtk_editor_fontdup(font);
    editor->text_properties[0].fore = gtk_editor_coldup(fore);
    editor->text_properties[0].back = gtk_editor_coldup(back);
    return;
  }

  for (i = 0; i < editor->n_token_entries; i++) {
    if (strcmp(editor->token_entries[i].name, pattern_name) == 0) {
      if (editor->token_entries[i].prop_id == -1) {
        i = editor->token_entries[i].prop_id = editor->n_text_properties++;
        editor->text_properties = g_realloc(editor->text_properties,
          sizeof(GtkEditorTextProperty) * editor->n_text_properties);
      } else
        i = editor->token_entries[i].prop_id;
      editor->text_properties[i].font = gtk_editor_fontdup(font);
      editor->text_properties[i].fore = gtk_editor_coldup(fore);
      editor->text_properties[i].back = gtk_editor_coldup(back);
      return;
    }
  }

  for (i = 0; i < editor->n_block_entries; i++) {
    if (strcmp(editor->block_entries[i].name, pattern_name) == 0) {
      if (editor->block_entries[i].prop_id == -1) {
        i = editor->block_entries[i].prop_id = editor->n_text_properties++;
        editor->text_properties = g_realloc(editor->text_properties,
          sizeof(GtkEditorTextProperty) * editor->n_text_properties);
      } else
        i = editor->block_entries[i].prop_id;
      editor->text_properties[i].font = gtk_editor_fontdup(font);
      editor->text_properties[i].fore = gtk_editor_coldup(fore);
      editor->text_properties[i].back = gtk_editor_coldup(back);
      return;
    }
  }
}

GSList *
_gtk_editor_add_property_change (GSList *text_changes,
				 int id, unsigned int offset,
				 unsigned int length)
{
  GSList *cur;
  PropertyChange *prop;

  if(text_changes
     && ((prop = text_changes->data) != NULL)
     && offset == prop->offset + prop->length
     && id == prop->id
     && offset != g_insertion_point) {
    prop->length += length;
    return text_changes;
  }

  if (g_free_text_props) {
    cur = g_free_text_props;
    g_free_text_props = g_slist_remove_link (g_free_text_props, cur);
    prop = cur->data;
  } else {
    prop = g_new (PropertyChange, 1);
    cur = g_slist_alloc();
    cur->data = prop;
    cur->next = NULL;
  }

  prop->id = id;
  prop->offset = offset;
  prop->length = length;

  if(text_changes == NULL)
    return cur;
  return(g_slist_concat(cur, text_changes));
}

void
_gtk_editor_apply_property_changes (GtkEditor *editor,
				    char *text,
				    GSList *text_changes,
				    int offset)
{
  GSList *cur;
  PropertyChange *prop;
  int t_offset;

  if(text_changes == NULL)
    return;

  text_changes = g_slist_reverse(text_changes);

  for(cur = text_changes; cur != NULL;) {
    prop = cur->data;
    t_offset = (prop->offset >= g_insertion_point)?
               ((g_deletion_length > 0)? g_deletion_length : -g_insertion_length)
               : 0;
    gtk_text_set_point(GTK_TEXT(editor), prop->offset + t_offset);
    gtk_text_forward_delete (GTK_TEXT (editor), prop->length);
    gtk_text_insert(GTK_TEXT(editor), editor->text_properties[prop->id].font,
                    editor->text_properties[prop->id].fore,
                    editor->text_properties[prop->id].back,
                    (text + (prop->offset - offset)),
                    prop->length);
    text_changes = g_slist_remove_link(text_changes, cur);
    g_free_text_props = g_slist_concat(cur, g_free_text_props);
    cur = text_changes;
  }
}

void
_gtk_editor_insert_text_with_properties (GtkEditor *editor,
					 const char *text,
					 GSList *text_changes)
{
  GSList *cur;
  PropertyChange *prop;
  text_changes = g_slist_reverse(text_changes);
  for(cur = text_changes; cur != NULL;) {
    prop = cur->data;
    gtk_text_set_point(GTK_TEXT(editor), prop->offset);
    gtk_text_insert(GTK_TEXT(editor), editor->text_properties[prop->id].font,
                    editor->text_properties[prop->id].fore,
                    editor->text_properties[prop->id].back,
                    text + (prop->offset - g_insertion_point),
                    prop->length);
    text_changes = g_slist_remove_link(text_changes, cur);
    g_free_text_props = g_slist_concat(cur, g_free_text_props);
    cur = text_changes;
  }
}

#define MAX_BUFFER_LENGTH 1024

void
_gtk_editor_update_blocks (GtkEditor *editor,
			   GtkEditorToken *start,
			   int pos)
{
  GSList *cur;
  GtkEditorToken *token;
  GtkEditorBlockStack *stack;
  GSList *prop_changes = NULL;
  PropertyChange *prop_change;
  char *buffer;
  static char sbuffer[MAX_BUFFER_LENGTH];
  int prop_id, max_len = 0, check = 1;

  for (token = start; token != NULL; token = token->next) {
    prop_id = -1;

    /* Check for a previous end of block */
    if (check && token->prev && token->prev->block_id != -1
        && token->prev->begins_block == FALSE) {
      /* Here the previous token was the end of a block.
         First we have to see if it has any meaning (i.e is matches a previous
         begin block) If it does we must pop all the blocks contained in this
         block off of the highlight stack and push them onto this block.
      */
      if ((stack = _gtk_editor_pop_blocks(editor, token->prev->block_id))) {
        token->prev->block = stack->block;
        token->prev->block->end = token->prev;
        token->prev->block->stack = stack->next;
      } else
        token->prev->block = NULL;
    }

    check = TRUE;

    /* Check how the token should be highlighted. */
    if (token->block_id != -1 && token->begins_block &&
        ((editor->block_stack && editor->block_stack->block->begin->nest
         && token->precedence >= editor->block_stack->block->begin->precedence)
         || !editor->block_stack)) {
      /* Here we have encountered a legal begin block.
         We can push it on to the highlight stack and start highlighting
         as this type block.
      */
      if (!token->block) {
        token->block = _gtk_editor_get_new_block();
        token->block->begin = token;
        token->block->end = NULL;
        _gtk_editor_push_block (editor, token->block);
      } else {
        check = FALSE;
        if (token->block->end)
          token = token->block->end->next;
        else
          token = NULL;
      }

      if(token->id != -1 && editor->token_entries[token->id].prop_id != -1)
        prop_id = editor->token_entries[token->id].prop_id;
      else if (editor->block_entries[token->block_id].prop_id != -1)
        prop_id = editor->block_entries[token->block_id].prop_id;
      else
        prop_id = 0;
    } else if (token->block_id != -1 && token->begins_block == FALSE){
      for (stack = editor->block_stack; stack != NULL; stack = stack->next) {
        if(stack->block->begin->block_id == token->block_id) {
          stack->block->end = token;
          token->block = stack->block;
          if (token->id != -1 && editor->token_entries[token->id].prop_id != -1)
            prop_id = editor->token_entries[token->id].prop_id;
          else if (editor->block_entries[token->block_id].prop_id != -1)
            prop_id = editor->block_entries[token->block_id].prop_id;
          else
            prop_id = 0;
        }
      }
    } else {
      if (token->block && token->begins_block) {
        _gtk_editor_free_block (token->block);
      }

      token->block = NULL;
      if (editor->block_stack) {
        if (token->precedence > editor->block_stack->block->begin->precedence && token->id != -1
            && editor->token_entries[token->id].prop_id != -1)
          prop_id = editor->token_entries[token->id].prop_id;
        else
          prop_id = editor->block_entries[editor->block_stack->block->begin->block_id].prop_id;
      } else if (token->id != -1 && editor->token_entries[token->id].prop_id != -1)
        prop_id = editor->token_entries[token->id].prop_id;
      else
        prop_id = 0;

    }

    if (prop_id == -1)
      prop_id = 0;

    if (token->prop_id != prop_id) {
      token->prop_id = prop_id;
      prop_changes = _gtk_editor_add_property_change (prop_changes,
						      prop_id, pos,
						      token->length);
    } else if (prop_id == 0)
      break;

    pos += token->length;
  }

  for (cur = prop_changes; cur != NULL; cur = cur->next) {
    prop_change = cur->data;
    if (prop_change->length > max_len)
      max_len = prop_change->length;
  }

  if (max_len > MAX_BUFFER_LENGTH)
    buffer = g_new(char, max_len);
  else
    buffer = sbuffer;

  gtk_text_freeze(GTK_TEXT(editor));

  for (cur = prop_changes; cur != NULL;) {
    prop_change = cur->data;
    copy_gtk_text (buffer, GTK_TEXT(editor), prop_change->offset, prop_change->length);
    gtk_text_set_point(GTK_TEXT(editor), prop_change->offset);
    gtk_text_forward_delete (GTK_TEXT (editor), prop_change->length);
    gtk_text_insert(GTK_TEXT(editor), editor->text_properties[prop_change->id].font,
                    editor->text_properties[prop_change->id].fore,
                    editor->text_properties[prop_change->id].back,
                    buffer, prop_change->length);
    prop_changes = g_slist_remove_link(prop_changes, cur);
    g_free_text_props = g_slist_concat(cur, g_free_text_props);
    cur = prop_changes;
  }
  gtk_text_thaw(GTK_TEXT(editor));

  if (buffer != sbuffer)
    g_free (buffer);
}

static void
copy_gtk_text (char *buffer, GtkText *text, int pos, int length)
{
  if (pos < text->gap_position) {
    if (pos + length < text->gap_position) {
      /* before gap */
      memcpy(buffer, text->text.ch + pos, length);

    } else {
      /* before and intersecting gap */
      int l = text->gap_position - pos;
      memcpy (buffer, text->text.ch + pos, l);
      memcpy (buffer,
              text->text.ch + text->gap_position + text->gap_size,
              length - l);
    }
  } else {
    /* starts inside or after gap */
    memcpy(buffer, text->text.ch + text->gap_size + pos, length);
  }
}

