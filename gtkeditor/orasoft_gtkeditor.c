#include <stdlib.h>

#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkmain.h>
#include <gtk/gtksignal.h>

#include "gtkeditor.h"
#include "internal.h"
#include "highlight.h"

static GtkEditorScanner *g_cscanner = NULL;
extern const char *get_name (void);
extern const char **get_token_names (void);
extern const char **get_block_names (void);
extern int get_token_no (const char *);
extern int get_block_no (const char *);
extern void token_func (void (*func)(int id, int precedence, char *text,
                                     int length, int pos, int block_id,
                                     int nesting, int block_type));

void
install_pat (GtkEditor *cbuff, GtkWidget *window)
{
  static GdkColor blue, greenish, blueish, red, brownish, green, purple;
  static GdkColormap *cmap = NULL;
  static GdkFont *keywordsfont = NULL, *commentfont = NULL, *regularfont = NULL;

  if(!g_cscanner) {    		
    g_cscanner = gtk_editor_static_scanner (get_name,
					    get_token_names,
					    get_block_names,
					    get_token_no,
					    get_block_no,
					    token_func,
					    char_func,
					    get_state,
					    set_state,
					    lex);
    if(!g_cscanner)
      return;
  }

  gtk_editor_install_scanner (cbuff, g_cscanner);

  if (!cmap) {
    cmap = gtk_widget_get_colormap (window);
    commentfont = gdk_font_load ("-adobe-courier-medium-i-normal-*-12-*-*-*-*-*-*-*");
    keywordsfont = gdk_font_load ("-adobe-courier-bold-r-normal-*-12-*-*-*-*-*-*-*");
    regularfont = gdk_font_load ("-adobe-courier-medium-r-normal-*-12-*-*-*-*-*-*-*");
    blue.pixel = 0;
    blue.red = 0;
    blue.green = 0;
    blue.blue = 65535;

    greenish.pixel = 0;
    greenish.red = 30000;
    greenish.green = 45000;
    greenish.blue = 0;

    blueish.pixel = 0;
    blueish.red = 40000;
    blueish.green = 0;
    blueish.blue = 55000;

    purple.pixel = 0;
    purple.red = 65535;
    purple.green = 0;
    purple.blue = 65535;

    red.pixel = 0;
    red.red = 65535;
    red.green = 0;
    red.blue = 0;

    brownish.pixel = 0;
    brownish.red = 65535;
    brownish.green = 43908;
    brownish.blue = 16383;
    
    green.pixel = 0;
    green.red = 30000;
    green.green = 20000;
    green.blue = 10000;

    gdk_color_alloc (cmap, &blue);
    gdk_color_alloc (cmap, &greenish);
    gdk_color_alloc (cmap, &blueish);
    gdk_color_alloc (cmap, &red);
    gdk_color_alloc (cmap, &brownish);
    gdk_color_alloc (cmap, &green);
    gdk_color_alloc (cmap, &purple);
    
  }

  /* pattern properties */
  gtk_editor_set_pattern_property(cbuff, "default", regularfont, NULL, NULL);
  gtk_editor_set_pattern_property(cbuff, "area", regularfont, &green, NULL);
  gtk_editor_set_pattern_property(cbuff, "keyword", keywordsfont, &blue, NULL);
  gtk_editor_set_pattern_property(cbuff, "type", regularfont, &greenish, NULL);
  gtk_editor_set_pattern_property(cbuff, "include", regularfont, &blueish, NULL);
  gtk_editor_set_pattern_property(cbuff, "sql", regularfont, &blueish, NULL);
  gtk_editor_set_pattern_property(cbuff, "functions", regularfont, &blueish, NULL);
  gtk_editor_set_pattern_property(cbuff, "comment", commentfont, &red, NULL);
  gtk_editor_set_pattern_property(cbuff, "objects", regularfont, &purple, NULL);
  gtk_editor_set_pattern_property(cbuff, "string", regularfont, &brownish, NULL);
}


