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

#ifndef INTERNAL_H
#define INTERNAL_H

#include "gtkeditor.h"

/* point in the old text where text is being inserted */
unsigned int g_insertion_point;

/* length of the inserted text */
unsigned int g_insertion_length;

/* length of the deleted text */
unsigned int g_deletion_length;

/* when text is reanalized by flex this should point to the base of
 * tokens from which start at the begining of the text */
GtkEditorToken *g_token;

GdkFont         *gtk_editor_fontdup         (GdkFont          *font);
GdkColor        *gtk_editor_coldup          (const GdkColor   *color);

void             gtk_editor_insert_text     (GtkEditable      *editable,
					     const gchar      *new_text,
					     gint              new_text_length,
					     gint             *position);

void             gtk_editor_delete_text     (GtkEditable      *editable,
					     gint              start_pos,
					     gint              end_pos);


#endif /* INTERNAL_H */
