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
#ifndef EDIT_H
#define EDIT_H

#include <glib.h>
#include "gtkeditor.h"

void _gtk_editor_merge_token  (int id,
                                          int precedence,
                                          char *text,
                                          int length,
                                          int pos,
                                          int block_id,
                                          int nesting,
                                          int block_type,
                                          int state);


GtkEditorBlock *     _gtk_editor_get_new_block (void);
void                 _gtk_editor_free_block (GtkEditorBlock *block);
void                 _gtk_editor_push_block (GtkEditor *editor, GtkEditorBlock *block);
GtkEditorBlockStack *_gtk_editor_pop_blocks (GtkEditor *editor, int id);

#endif
