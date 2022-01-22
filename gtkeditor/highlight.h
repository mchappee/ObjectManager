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

#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <glib.h>

#include "gtkeditor.h"

GSList *_gtk_editor_add_property_change         (GSList      *text_changes,
						 int          id,
						 unsigned int offset,
						 unsigned int length);

void    _gtk_editor_apply_property_changes      (GtkEditor   *editor,
						 char        *text,
						 GSList      *text_changes,
						 int          offset);

void    _gtk_editor_insert_text_with_properties (GtkEditor   *editor,
						 const char  *text,
						 GSList      *text_changes);
void    _gtk_editor_update_blocks               (GtkEditor *editor,
						 GtkEditorToken *start,
						 int pos);

#endif

