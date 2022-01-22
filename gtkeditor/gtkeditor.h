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
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GTK_EDITOR_H__
#define __GTK_EDITOR_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GTK_TYPE_EDITOR (gtk_editor_get_type ())
#define GTK_EDITOR(obj) (GTK_CHECK_CAST (obj, gtk_editor_get_type (), GtkEditor))
#define GTK_EDITOR_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST (klass, gtk_editor_get_type (), GtkEditorClass))
#define GTK_IS_EDITOR(obj) (GTK_CHECK_TYPE (obj, gtk_editor_get_type ()))
#define GTK_IS_EDITOR_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_EDITOR))

  void char_func (int (*func)());
  int get_state (void);
  void set_state (int state);
  int lex ();

  typedef struct _GtkEditor            GtkEditor;
  typedef struct _GtkEditorClass       GtkEditorClass;
  typedef struct _GtkEditorBlockStack  GtkEditorBlockStack;
  typedef struct _GtkEditorBlock       GtkEditorBlock;
  typedef struct _GtkEditorToken       GtkEditorToken;

  struct _GtkEditorBlock {
    GtkEditorToken *begin;
    GtkEditorToken *end;
    GtkEditorBlockStack *stack;
  };

  struct _GtkEditorBlockStack {
    GtkEditorBlock *block;
    GtkEditorBlockStack *next;
  };

  struct _GtkEditorToken {
    signed id : 8;
    signed block_id : 8;
    unsigned begins_block : 1;
    unsigned precedence :7;
    signed prop_id : 8;
    unsigned nest : 1;
    unsigned length : 31;
    int state;
    GtkEditorBlock *block;
    struct _GtkEditorToken *prev;
    struct _GtkEditorToken *next;
  };

  typedef struct _GtkEditorTextProperty {
    GdkFont *font;
    GdkColor *fore;
    GdkColor *back;
  }GtkEditorTextProperty;

  typedef struct _GtkEditorTokenEntry {
    char *name;

    /* the text properties associated with the entry*/
    int prop_id;
  } GtkEditorTokenEntry;

  typedef struct _GtkEditorBlockEntry {
    char *name;

    /* the text properties associated with the entry*/
    int prop_id;
  } GtkEditorBlockEntry;
  
  typedef struct {
    const char *(*get_name)();
    const char **(*get_token_names)();
    const char **(*get_block_names)();
    int (*get_token_no)();
    int (*get_block_no)();
    void (*token_func)(void (*func)(int id, int precedence, char *text,
      int length, int pos, int block_id, int nesting, int block_type));
    void (*char_func)(int (*func)());
    int (*get_state)();
    void (*set_state)(int state);
    int (*lex)();
  } GtkEditorScanner;
       
  /* --<the widget>------------------------------------------------------- */
  struct _GtkEditor {
    GtkText text;

    /* --<hilite stuff>--------------------------------------------------- */
    int n_text_properties;
    GtkEditorTextProperty *text_properties;
    int n_token_entries;
    GtkEditorTokenEntry *token_entries;
    int n_block_entries;
    GtkEditorBlockEntry *block_entries;
    /* scanner */
    GtkEditorScanner *scanner;

    /* tokens */
    GtkEditorToken *tokens;
    GtkEditorToken *cur_token;
    int cur_token_pos;

    /* block stack */
    GtkEditorBlockStack *block_stack;
  };

  struct _GtkEditorClass {
    GtkTextClass parent_class;
  };


  /* --<prototypes>------------------------------------------------------- */
  /* widget stuff */
  guint          gtk_editor_get_type               (void);
  GtkWidget*     gtk_editor_new                    (GtkAdjustment *hadj,
						    GtkAdjustment *vadj);

  /* syntax highlighting */
  GtkEditorScanner
                *gtk_editor_load_scanner           (const char    *name);
  GtkEditorScanner
                *gtk_editor_static_scanner     (const char *(*get_name)(),
                                                const char **(*get_token_names)(),
                                                const char **(*get_block_names)(),
                                                int (*get_token_no)(),
                                                int (*get_block_no)(),
                                                void (*token_func)(void (*func)(int id, int precedence, char *text,
                                                int length, int pos, int block_id, int nesting, int block_type)),
                                                void (*char_func)(int (*func)()),
                                                int (*get_state)(),
                                                void (*set_state)(int state),
                                                int (*lex)());
  void           gtk_editor_install_scanner        (GtkEditor     *editor,
					            GtkEditorScanner *scanner);
  void           gtk_editor_uninstall_scanner       (GtkEditor     *editor);
  void           gtk_editor_set_pattern_property   (GtkEditor     *editor,
  						    char          *patten_name,
						    GdkFont       *font,
						    GdkColor      *fore,
						    GdkColor      *back);
  void           gtk_editor_hilite_buffer          (GtkEditor      *editor);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_EDITOR_H__ */
