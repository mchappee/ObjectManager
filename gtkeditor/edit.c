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
#include <glib.h>
#include <stdio.h>

#include "gtkeditor.h"
#include "internal.h"
#include "scanner.h"
#include "highlight.h"
#include "edit.h"

#define TOKEN_LOOKBACK 5
#define BEGIN_BLOCK 0
#define END_BLOCK 1
#define FREEZE_LENGTH 1024

/*
 * globals
 */

/* pointer to current editor being analized */
static GtkEditor *g_editor;

static GtkEditorToken *g_start_token;
static int g_pos;

/* text changes associated with the newly inserted text */
static GSList *g_new_text_changes = NULL;
static int g_blocks_changed;
static GtkEditorBlockStack *g_block_stack;

/* used for recyclying */
static GtkEditorToken *g_free_tokens = NULL;
static GtkEditorBlockStack *g_free_block_stacks = NULL;


/*
 * prototypes
 */
static void            move_position_near    (GtkEditor *editor,
                                              int pos);
static void            look_back             (GtkEditor *editor,
                                              int count);
static void            split_token           (GtkEditorToken *token,
					      int              pos);
static GtkEditorToken *get_new_token         (void);
static void            recycle_token         (GtkEditorToken *token);
static GtkEditorBlockStack *
                       get_new_block_stack   ();
static void            recycle_block_stack   (GtkEditorBlockStack *block_stack);
static void            push_blocks           (GtkEditor *editor, GtkEditorBlockStack *block_stack);
static GtkEditorBlock *pop_block             (GtkEditor *editor);
static GtkEditorBlockStack *
                       duplicate_block_stack (GtkEditorBlockStack *block_stack);
static void            restore_block_stack   (GtkEditor *editor, GtkEditorBlockStack *block_stack);


#if DEBUG
static void print_tokens (GtkEditor *editor);
#endif

/* gtk_editor_insert_text

   This function is called when new text is inserted by the user. It
   scans the new text with the effected areas, and updates the editors
   token list.
*/
void
gtk_editor_insert_text (GtkEditable *editable, const gchar *new_text,
			gint new_text_length, gint *position)
{
  GtkEditor *editor = GTK_EDITOR (editable);
  GtkText *text = GTK_TEXT (editable);

  g_editor = GTK_EDITOR (editable);
  if (!g_editor->scanner) {
    /* No scanner installed insert with the default font properties */
    gtk_text_set_point (text, *position);
    gtk_text_insert (text, GTK_WIDGET (editable)->style->font,
		     &GTK_WIDGET (editable)->style->text[GTK_STATE_NORMAL],
		     &GTK_WIDGET (editable)->style->base[GTK_STATE_NORMAL],
		     new_text, new_text_length);
    *position += new_text_length;
    gtk_signal_emit_stop_by_name (GTK_OBJECT(editable), "insert_text");
    return;
  }

  if (!g_editor->tokens && gtk_text_get_length (text)) {
    /* Uninitialized editor, highlight the entire buffer before inserting
       anything.
    */
    gtk_editor_hilite_buffer (g_editor);
  }

  /*
     Create a new token and insert it into the token list. This token
     represents the inserted text. A prop_id of -1 represents a token
     with no associated text in the editor. This fact is used in the
     merge routine below.
  */
  g_token = get_new_token();
  g_token->length = new_text_length;
  g_blocks_changed = FALSE;

  if (!g_editor->tokens) {
    /* empty editor */
    g_editor->tokens = g_token;
    g_start_token = NULL;
  } else {
    /* find insertion point */
    move_position_near(g_editor, *position);

    /* insert the token into the token list */
    if (*position == 0) {
      g_token->next = g_editor->tokens;
      g_editor->tokens->prev = g_token;
      g_editor->tokens = g_token;
      editor->cur_token_pos += new_text_length;
    } else if (*position == gtk_text_get_length(text)) {
      g_token->prev = editor->cur_token;
      editor->cur_token->next = g_token;
    } else {
      if (editor->cur_token_pos != *position) {
        if (editor->cur_token->block) {
          if (editor->cur_token->begins_block) {
            /* token needs to be split, destroy the block */
            editor->cur_token->block = NULL;
            _gtk_editor_free_block (pop_block (editor));
          } else {
            editor->cur_token->block->end = NULL;
            editor->cur_token->block = NULL;
          }
          g_blocks_changed = 1;
        }
        split_token(editor->cur_token, *position - editor->cur_token_pos);
        editor->cur_token = editor->cur_token->next;
        editor->cur_token_pos = *position + new_text_length;
      } else
        editor->cur_token_pos += new_text_length;
      g_token->next = editor->cur_token;
      g_token->prev = editor->cur_token->prev;
      editor->cur_token->prev->next = g_token;
      editor->cur_token->prev = g_token;
    }

    /* look back a certain ammount */
    look_back(editor, TOKEN_LOOKBACK);

    g_start_token = g_token = editor->cur_token;

    if (g_token->block && g_token->begins_block)
      pop_block (editor);
  }

  /* scan the text */
  g_new_text_changes = NULL;
  _gtk_editor_scan (g_editor, editor->cur_token_pos, new_text, new_text_length, *position);
  /* Insert the new text with the new text properties. The reason I do this
     after I scan the text is because of a bug in the text widget.
  */
  if(new_text_length > FREEZE_LENGTH) {
    gtk_text_freeze(text);
    _gtk_editor_insert_text_with_properties (g_editor, new_text,
					     g_new_text_changes);
    gtk_text_thaw(text);
  } else {
    _gtk_editor_insert_text_with_properties (g_editor, new_text,
					     g_new_text_changes);
  }

  /* apply block changes */
  if(g_blocks_changed)
    _gtk_editor_update_blocks (g_editor, g_token, g_pos);

  restore_block_stack (editor, g_block_stack);

  /* set the new position*/
  *position += new_text_length;
/*  gtk_text_set_point (text, *position);
  gtk_editable_set_position (editable, *position);*/

  /* terminate this signal */
  gtk_signal_emit_stop_by_name (GTK_OBJECT(editable), "insert_text");
}

/* gtk_editor_delete_text

   This function is called when the user deletes text. It deletes the
   asociated tokens and block and rescans the affected area.

*/
void
gtk_editor_delete_text (GtkEditable *editable,
			gint start_pos, gint end_pos)
{
  GtkEditor *editor = GTK_EDITOR (editable);
  GtkEditorToken *prev = NULL, *temp, *cur;
  int pos;
  int deletion_length;

  /* calculate the length of the deleted area */
  deletion_length = end_pos - start_pos;

  if (!editor->scanner) {
    /* No scanner, just delete the text and return. */
    gtk_text_set_point(GTK_TEXT(editor), start_pos);
    gtk_text_forward_delete(GTK_TEXT(editor), deletion_length);
    gtk_signal_emit_stop_by_name (GTK_OBJECT(editable), "delete_text");
    return;
  }

  /* find nearest token */
  move_position_near(editor, start_pos);

  if (start_pos > editor->cur_token_pos) {
    /* starts in the middle of a token */
    if (editor->cur_token->block) {
      if (editor->cur_token->begins_block) {
        /* token needs to be split, destroy the block */
        _gtk_editor_free_block (pop_block (editor));
      } else
        editor->cur_token->block->end = NULL;
      editor->cur_token->block = NULL;
      g_blocks_changed = 1;
    }

    /* split the token */
    split_token (editor->cur_token, start_pos - editor->cur_token_pos);
    editor->cur_token = editor->cur_token->next;
    editor->cur_token_pos = start_pos;
  }

  cur = editor->cur_token;

  /* set prev to the token prior to the deleted area */
  if (cur->prev)
    prev = cur->prev;

  /* get rid of tokens and blocks in the deleted area */
  pos = start_pos;
  while (pos < end_pos) {
    if (cur->block && cur->begins_block) {
      if (cur->block->end)
        cur->block->end->block = NULL;
      if(cur == editor->cur_token)
        _gtk_editor_free_block (pop_block (editor));
      else
        _gtk_editor_free_block (cur->block);
      cur->block = NULL;
    } else if (cur->block && cur->begins_block == FALSE)
      cur->block->end = NULL;

    if (pos + cur->length <= end_pos) {
      /* Token is entirely inside the deleted area, recycle it. */
      pos += cur->length;
      temp = cur;
      cur = cur->next;
      recycle_token(temp);
    } else {
      /* Token is covers more than the deleted are, shorten it. */
      cur->length -= end_pos - pos;
      pos = end_pos;
    }
  }

  if((gtk_text_get_length(GTK_TEXT(g_editor)) - deletion_length) == 0) {
    /* cleared editor, get rid of everything */
    editor->tokens = NULL;
    editor->cur_token = NULL;
    editor->block_stack = NULL;
    gtk_text_set_point (GTK_TEXT (editor), start_pos);
    gtk_text_forward_delete (GTK_TEXT (editor), deletion_length);
    gtk_signal_emit_stop_by_name (GTK_OBJECT(editable), "delete_text");
    return;
  }

  if (prev) {
    prev->next = cur;
    if (cur)
      cur->prev = prev;
    editor->cur_token = prev;
    editor->cur_token_pos -= prev->length;

    /* look back a certain ammount */
    look_back(editor, TOKEN_LOOKBACK);
  } else {
    cur->prev = NULL;
    editor->tokens = cur;
    editor->cur_token = cur;
    editor->cur_token_pos = start_pos;
  }

  g_start_token = g_token = editor->cur_token;

  if (g_token->block && g_token->begins_block)
    pop_block (editor);

  /* rescan affected text */
  g_editor = editor;
  _gtk_editor_scan (editor, editor->cur_token_pos, NULL,
		    -deletion_length, start_pos);
  /* delete the deleted text */
  gtk_text_set_point (GTK_TEXT (editor), start_pos);
  gtk_text_forward_delete (GTK_TEXT (editor), deletion_length);
  /* apply block changes */
  if(g_blocks_changed)
    _gtk_editor_update_blocks (editor, g_token, g_pos);

  restore_block_stack (editor, g_block_stack);
  /* set the cursor position */
  gtk_text_set_point (GTK_TEXT(editor), start_pos);
  gtk_editable_set_position (editable, start_pos);
  /* terminate this signal */
  gtk_signal_emit_stop_by_name (GTK_OBJECT(editable), "delete_text");
}

/* gtk_editor_hilite_buffer

   Rescans the entire buffer and highlights acordingly.
*/
void
gtk_editor_hilite_buffer (GtkEditor *editor)
{
  int point, length;
  GtkEditable *editable;
  GtkText *text;
  char *buffer;
  GtkEditorToken *cur, *temp;

  if (editor->scanner == NULL)
    /* nothing to do here */
    return;

  editable = GTK_EDITABLE (editor);
  text = GTK_TEXT (editor);

  if (!(length = gtk_text_get_length (text)))
    return;
  if (editor->tokens) {
    for(cur = editor->tokens; cur != NULL;) {
      temp = cur;
      cur = cur->next;
      recycle_token (temp);
    }
  }

  /*
     Create a new token an insert it into the token list. This token
     represents the entire buffer. A prop_id of -1 represents a token
     with no associated text in the editor. This fact is used in the
     merge routine below.
  */
  g_token = editor->tokens = get_new_token();
  g_token->length = length;
  g_start_token = NULL;
  g_blocks_changed = 0;

  gtk_text_freeze (text);

  buffer = gtk_editable_get_chars (editable, 0, length);
  gtk_text_set_point (text, 0);
  gtk_text_forward_delete (text, length);
  point = gtk_text_get_point (text);
  g_new_text_changes = NULL;
  g_editor = editor;
  _gtk_editor_scan (editor, 0, buffer, length, 0);
  _gtk_editor_insert_text_with_properties (editor, buffer,
					   g_new_text_changes);
  gtk_text_set_point (text, 0);
  gtk_editable_set_position (editable, 0);
  g_free(buffer);

  gtk_text_thaw (text);

  restore_block_stack (editor, g_block_stack);
}

/* find_nearest_token

   Finds the nearest token and its coresponding blocks

*/
static void
move_position_near(GtkEditor *editor, int pos)
{
  GtkEditorToken *token = editor->cur_token;
  GtkEditorBlockStack *stack;
  int cur_pos = editor->cur_token_pos;

  if (pos >= cur_pos + token->length) {
    /* Position is greater than the current position, search forward. */
    while (cur_pos + token->length <= pos && token->next != NULL) {
      /* increment the token */
      cur_pos += token->length;
      token = token->next;
      /* check for crossing block boundries */
      if (token->prev && token->prev->block
          && token->prev->begins_block == FALSE) {
        stack = _gtk_editor_pop_blocks(editor, token->prev->block_id);
        token->prev->block->stack = stack->next;
      }
      if (token->block && token->begins_block) {
        _gtk_editor_push_block(editor, token->block);
      }
    }
  } else if(pos < cur_pos) {
    /* Position is less than the current position, search back */
    while (cur_pos > pos) {
      /* decrement the token */
      token = token->prev;
      cur_pos -= token->length;
      /* check for crossing block boundries */
      if (token->next->block && token->next->begins_block) {
        pop_block(editor);
      }
      if (token->block && token->begins_block == FALSE) {
        _gtk_editor_push_block(editor, token->block);
        if (token->block->stack)
          push_blocks(editor, token->block->stack);
      }
    }
  }

  editor->cur_token = token;
  editor->cur_token_pos = cur_pos;
}

static void
look_back (GtkEditor *editor, int count)
{
  GtkEditorToken *token = editor->cur_token;
  int pos = editor->cur_token_pos;

  /* look back */
  while (token->prev != NULL && count > 0) {
    count--;
    /* decrement the token */
    token = token->prev;
    pos -= token->length;
    /* check for crossing block boundries */
    if (token->next->block && token->next->begins_block) {
      pop_block(editor);
    }
    if (token->block && token->begins_block == FALSE) {
      _gtk_editor_push_block(editor, token->block);
      if (token->block->stack)
        push_blocks(editor, token->block->stack);
    }
  }
  editor->cur_token = token;
  editor->cur_token_pos = pos;
}

/* merge_token

   This function merges a token into the token list. The merged token is
   returned.
*/
void
_gtk_editor_merge_token (int id, int precedence, char *text,
                         int length, int position, int block_id,
                         int nesting, int begins_block, int state)
{
  GtkEditorToken *temp, *cur = NULL;

  GSList *text_changes = NULL;
  GtkEditorBlockStack *stack;
  unsigned int pos = 0, len, cur_pos = 0;
  int prop_id = -1;

  g_pos = position;
  /* Check for a previous end of block */
  if (g_token != g_start_token
      && g_token->prev && g_token->prev->block_id != -1
      && g_token->prev->begins_block == FALSE) {
    /* Here the previous token was the end of a block.
       First we have to see if it has any meaning (i.e is matches a previous
       begin block) If it does we must pop all the blocks contained in this
       block off of the highlight stack and push them onto this block.
    */
    if ((stack = _gtk_editor_pop_blocks(g_editor, g_token->prev->block_id))) {
      if (!g_token->prev->block)
        g_blocks_changed = 1;
      g_token->prev->block = stack->block;
      g_token->prev->block->end = g_token->prev;
      g_token->prev->block->stack = stack->next;
    }
  }

  /* Check how the token should be highlighted. */

  if (block_id != -1 && begins_block &&
      ((g_editor->block_stack && g_editor->block_stack->block->begin->nest
         && precedence >= g_editor->block_stack->block->begin->precedence)
         || !g_editor->block_stack)) {
    /* Here we have encountered a legal begin block.
       We can push it on to the highlight stack and start highlighting
       as this type block.
    */
    if (!g_token->block) {
      g_blocks_changed = 1;
      g_token->block = _gtk_editor_get_new_block();
      g_token->block->end = NULL;
    }

    g_token->block->begin = g_token;
    _gtk_editor_push_block (g_editor, g_token->block);

    if(id != -1 && g_editor->token_entries[id].prop_id != -1)
      prop_id = g_editor->token_entries[id].prop_id;
    else if (g_editor->block_entries[block_id].prop_id != -1)
      prop_id = g_editor->block_entries[block_id].prop_id;
    else
      prop_id = 0;
  } else if (block_id != -1 && begins_block == FALSE){
    for (stack = g_editor->block_stack; stack != NULL; stack = stack->next) {
      if(stack->block->begin->block_id == block_id) {
        if (stack->block->end != g_token) {
          g_blocks_changed = 1;
          stack->block->end = g_token;
        }
        g_token->block = stack->block;
        if (id != -1 && g_editor->token_entries[id].prop_id != -1)
          prop_id = g_editor->token_entries[id].prop_id;
        else if (g_editor->block_entries[block_id].prop_id != -1)
          prop_id = g_editor->block_entries[block_id].prop_id;
        else
          prop_id = 0;
      }
    }
  } else {
    if (g_token->block && g_token->begins_block) {
      _gtk_editor_free_block (g_token->block);
    }
    g_token->block = NULL;
    if (g_editor->block_stack) {
      if (precedence > g_editor->block_stack->block->begin->precedence && id != -1
          && g_editor->token_entries[id].prop_id != -1)
        prop_id = g_editor->token_entries[id].prop_id;
      else
        prop_id = g_editor->block_entries[g_editor->block_stack->block->begin->block_id].prop_id;
    } else if (id != -1 && g_editor->token_entries[id].prop_id != -1)
      prop_id = g_editor->token_entries[id].prop_id;
    else
      prop_id = 0;
  }

  if (prop_id == -1)
    prop_id = 0;


  /* Loop through the tokens that intersect the merged token. */
  while (pos < length) {
    if (length - pos >= g_token->length) {
      /* new token overlaps the current token */
      len = g_token->length;
      if(g_token->prop_id == -1) {
        /* This token represents new text, we must add it's property to the
           global change list. This is later applied by hilite_buffer
           and insert_text.
        */
        g_new_text_changes = _gtk_editor_add_property_change (g_new_text_changes,
							      prop_id,
							      g_pos+pos,
							      len);
      } else if (g_token->prop_id != prop_id) {
        text_changes = _gtk_editor_add_property_change (text_changes,
							prop_id,
							g_pos+pos,
							len);
      }

      if (cur == NULL) {
        /* Then merged token doesn't belong to the list yet. Since we would be
           getting rid of this token normally, we can let the merged
           token take over it's spot.
        */
        cur = g_token;
	cur_pos = g_pos;
        g_token->id = id;
        g_token->block_id = block_id;
        g_token->begins_block = begins_block;
        g_token->precedence = precedence;
        g_token->prop_id = prop_id;
        g_token->length = length;
        g_token->nest = nesting;
        g_token->state = state;
	g_token = g_token->next;
      } else {
        /* The token no longer exist, recycle it. */
        temp = g_token;
	if (g_token->prev)
	  g_token->prev->next = g_token->next;
	if (g_token->next)
	  g_token->next->prev = g_token->prev;
	g_token = g_token->next;
	recycle_token (temp);
      }

    } else {
      /* The merged token covers less than the current token. */
      len = length - pos;
      if(g_token->prop_id == -1) {
        g_new_text_changes = _gtk_editor_add_property_change (g_new_text_changes,
							      prop_id,
							      g_pos+pos,
							      len);
      } else if (g_token->prop_id != prop_id) {
        text_changes = _gtk_editor_add_property_change (text_changes,
							prop_id,
							g_pos+pos,
							len);
      }
			
      if (cur == NULL) {
        /* Then merged token doesn't belong to the list yet. Since the merged
           token covers less than this token we have to split it.
        */
	cur = g_token;
	cur_pos = g_pos;
	split_token(g_token, len);
	g_token->id = id;
        g_token->block_id = block_id;
        g_token->begins_block = begins_block;
        g_token->precedence = precedence;
        g_token->prop_id = prop_id;
        g_token->length = length;
        g_token->nest = nesting;
        g_token->state = state;
	g_token = g_token->next;
	
      } else {
	/* shorten token */
	g_token->length -= len;
      }
    }
    /* increment the ammount merged */
    pos += len;
  }

  /* update the cur token */
  if ((g_insertion_point >= g_pos
      && g_insertion_point < g_pos + length) ||
      (cur->next == NULL && g_insertion_point == g_pos + length)) {
    g_editor->cur_token = cur;
    g_editor->cur_token_pos = g_pos;
    g_block_stack = duplicate_block_stack(g_editor->block_stack);
  }

  /* apply property changes */
  if (text_changes) {
    _gtk_editor_apply_property_changes(g_editor, text, text_changes, g_pos);
  }

  g_pos += length;
}

static void
split_token (GtkEditorToken *token, int pos)
{
  GtkEditorToken *new_token;
  
  new_token = get_new_token();
  new_token->next = token->next;
  new_token->prev = token;
  if (token->next)
    token->next->prev = new_token;
  token->next = new_token;
  new_token->length = token->length - pos;
  new_token->prop_id = token->prop_id;
  new_token->state = token->state;
  token->length = pos;
}

static GtkEditorToken*
get_new_token (void)
{
  static GtkEditorToken *temp;
  
  if (g_free_tokens) {
    temp = g_free_tokens;
    g_free_tokens = g_free_tokens->next;
    if (g_free_tokens)
      g_free_tokens->prev = NULL;
  } else {
    temp = g_new (GtkEditorToken, 1);
  }
  temp->id = -1;
  temp->block_id = -1;
  temp->precedence = 0;
  temp->prop_id = -1;
  temp->length = 0;
  temp->nest = 0;
  temp->state = 0;
  temp->block = NULL;
  temp->next = NULL;
  temp->prev = NULL;

  return temp;
}

static void
recycle_token(GtkEditorToken *token)
{
  token->prev = NULL;
  token->next = g_free_tokens;
  if (g_free_tokens)
    g_free_tokens->prev = token;
  g_free_tokens = token;
}

void
_gtk_editor_push_block (GtkEditor *editor, GtkEditorBlock *block)
{
  GtkEditorBlockStack *block_stack;

  block_stack = get_new_block_stack ();
  block_stack->block = block;
  block_stack->next = editor->block_stack;
  editor->block_stack = block_stack;
}

static void
push_blocks(GtkEditor *editor, GtkEditorBlockStack *block_stack)
{
  GtkEditorBlockStack *stack;

  for(stack = block_stack; stack->next != NULL; stack = stack->next);

  stack->next = editor->block_stack;
  editor->block_stack = block_stack;
}

static GtkEditorBlock *
pop_block (GtkEditor *editor)
{
  GtkEditorBlock *block = NULL;
  GtkEditorBlockStack *temp;

  if (editor->block_stack) {
    block = editor->block_stack->block;
    temp = editor->block_stack;
    editor->block_stack = editor->block_stack->next;
    recycle_block_stack(temp);
  }

  return block;
}

GtkEditorBlockStack *
_gtk_editor_pop_blocks (GtkEditor *editor, int id)
{
  GtkEditorBlockStack *node, *block_stack = NULL;

  /* first we check if if this token ends any of the blocks on the stack */
  for (node = editor->block_stack; node != NULL; node = node->next) {
    if (node->block->begin->block_id == id) {
      block_stack = editor->block_stack;
      editor->block_stack = node->next;
      node->next = NULL;
      break;
    }
  }

  return block_stack;
}

static GtkEditorBlockStack *
get_new_block_stack ()
{
  GtkEditorBlockStack *block_stack;

  if (g_free_block_stacks) {
    block_stack = g_free_block_stacks;
    g_free_block_stacks = g_free_block_stacks->next;
  } else {
    block_stack = g_new (GtkEditorBlockStack, 1);
  }

  block_stack->block = NULL;
  block_stack->next = NULL;

  return block_stack;
}

static void
recycle_block_stack (GtkEditorBlockStack *block_stack)
{
  block_stack->next = g_free_block_stacks;
  g_free_block_stacks = block_stack;
}

static GtkEditorBlockStack *
duplicate_block_stack (GtkEditorBlockStack *block_stack)
{
  GtkEditorBlockStack *new_block_stack = NULL, *stack = NULL;

  for(; block_stack != NULL; block_stack = block_stack->next) {
    if(new_block_stack == NULL)
      stack = new_block_stack = get_new_block_stack ();
    else
      stack = stack->next = get_new_block_stack ();
    stack->block = block_stack->block;
  }

  return new_block_stack;
}

static void
restore_block_stack (GtkEditor *editor, GtkEditorBlockStack *block_stack)
{
  if (editor->block_stack)
    recycle_block_stack(editor->block_stack);
  editor->block_stack = block_stack;
}

GtkEditorBlock *
_gtk_editor_get_new_block (void)
{
  GtkEditorBlock *block;

  block = g_new(GtkEditorBlock, 1);
  block->begin = NULL;
  block->end = NULL;
  block->stack = NULL;

  return block;
}

void
_gtk_editor_free_block (GtkEditorBlock *block)
{
  g_free(block);
}

/*
  for debugging
*/
#if DEBUG
static void
print_tokens (GtkEditor *editor)
{
  GtkEditorToken *token = editor->tokens;
  GtkEditorBlockStack *stack;
  int i, pos = 0, level = 0;

  g_print("--------------------------------------------\n");
  g_print("Stack state:\n");
  for (stack = editor->block_stack; stack != NULL; stack = stack->next) {
    g_print("%d\n", stack->block->begin->block_id);
  }

  g_print("\n");
  for (; token != NULL; token = token->next) {
    g_print("|");
    if(token == editor->cur_token)
      g_print("*->|");

    if (token->block && token->begins_block) {
      g_print("%d>|", level++);
    }
    for(i = 0; i < token->length; i++) {
      g_print("%c", GTK_TEXT_INDEX(GTK_TEXT(editor), pos + i));
    }
    if(token->block && token->begins_block == FALSE) {
      g_print("|<%d", --level);
    }
    pos += token->length;
  }
  g_print("|\n");
  g_print("--------------------------------------------\n");
}

#endif
