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

#include <stdlib.h>

#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>

#include "gtkeditor.h"
#include "internal.h"

/* widget stuff */
static void   gtk_editor_class_init          (GtkEditorClass   *klass);
static void   gtk_editor_init                (GtkEditor        *editor);
static void   gtk_editor_destroy             (GtkObject        *object);


/* --<local data>--------------------------------------------------------- */
static GtkWidgetClass *parent_class = NULL;

/* --<widget initialization, constructor and destructor>------------------ */
guint
gtk_editor_get_type ()
{
  static guint editor_type = 0;

  if (!editor_type)
    {
      GtkTypeInfo editor_info =
      {
	"GtkEditor",
	sizeof (GtkEditor),
	sizeof (GtkEditorClass),
	(GtkClassInitFunc) gtk_editor_class_init,
	(GtkObjectInitFunc) gtk_editor_init,
	NULL,
        NULL,
      };

      editor_type = gtk_type_unique (gtk_text_get_type (), &editor_info);
    }

  return editor_type;
}

static void
gtk_editor_class_init (GtkEditorClass *class)
{
  GtkObjectClass *object_class;
  GtkEditableClass *editable_class;

  object_class = (GtkObjectClass*) class;
  editable_class = (GtkEditableClass *) class;

  /* init local data */
  parent_class = gtk_type_class (gtk_text_get_type ());

  /* setup signals */
  object_class->destroy = gtk_editor_destroy;

  editable_class->insert_text = gtk_editor_insert_text;
  editable_class->delete_text = gtk_editor_delete_text;
}

static void
gtk_editor_init (GtkEditor *editor)
{
  /* hilite */
  editor->scanner = NULL;
  editor->n_token_entries = 0;
  editor->token_entries = NULL;
  editor->n_block_entries = 0;
  editor->block_entries = NULL;
  editor->tokens = NULL;
  editor->cur_token = NULL;
  editor->cur_token_pos = 0;
  editor->block_stack = NULL;

  /* default text property */
  editor->n_text_properties = 1;
  editor->text_properties = g_malloc(sizeof(GtkEditorTextProperty));
  editor->text_properties[0].font = GTK_WIDGET (editor)->style->font;
  editor->text_properties[0].fore = &GTK_WIDGET (editor)->style->text[GTK_STATE_NORMAL];
  editor->text_properties[0].back = &GTK_WIDGET (editor)->style->base[GTK_STATE_NORMAL];
}

GtkWidget*
gtk_editor_new (GtkAdjustment *hadj,
		GtkAdjustment *vadj)
{
  GtkWidget *editor;

  if (hadj)
    g_return_val_if_fail (GTK_IS_ADJUSTMENT (hadj), NULL);
  if (vadj)
    g_return_val_if_fail (GTK_IS_ADJUSTMENT (vadj), NULL);
  
  editor = gtk_widget_new (GTK_TYPE_EDITOR,
			   "hadjustment", hadj,
			   "vadjustment", vadj,
			   NULL);

  return GTK_WIDGET (editor);
}

static void
gtk_editor_destroy (GtkObject *object)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (GTK_IS_EDITOR (object));

  GTK_OBJECT_CLASS(parent_class)->destroy (object);
}

/* --<hilite stuff>------------------------------------------------------- */
GdkFont*
gtk_editor_fontdup (GdkFont *font)
{
  if (font) {
    gdk_font_ref (font);
  }
  return font;
}

GdkColor*
gtk_editor_coldup (const GdkColor *color)
{
  GdkColor *new;
  if (color) {
    new = g_new (GdkColor, 1);
    memcpy (new, color, sizeof (GdkColor));
  } else {
    new = NULL;
  }
  return new;
}
