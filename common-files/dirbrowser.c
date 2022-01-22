/* This file was borrowed from xmms (http://www.xmms.org),
   the best multimedia program in the world */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include "common-files/common.h"

GtkWidget *playlistwin_dir_browser,playlistwin;
void playlistwin_add_dir_handler(gchar *);
GtkWidget *create_dir_browser(char *current_path,int *response);

static char *obj_dirpath;
GtkWidget *window;

static char *folder[] =
{
	"16 16 8 1",
	" 	c None",
	".	c #909000",
	"+	c #000000",
	"@	c #EFE8EF",
	"#	c #FFF8CF",
	"$	c #FFF890",
	"%	c #CFC860",
	"&	c #FFC890",
	"                ",
	"  .....+        ",
	" .@##$$.+       ",
	".%%%%%%%......  ",
	".###########$%+ ",
	".#$$$$$$$$$$&%+ ",
	".#$$$$$$$&$&$%+ ",
	".#$$$$$$$$&$&%+ ",
	".#$$$$$&$&$&$%+ ",
	".#$$$$$$&$&$&%+ ",
	".#$$$&$&$&$&&%+ ",
	".#&$&$&$&$&&&%+ ",
	".%%%%%%%%%%%%%+ ",
	" ++++++++++++++ ",
	"                ",
	"                "};

/* XPM */
static char *ofolder[] =
{
	"16 16 12 1",
	" 	c None",
	".	c #808080",
	"+	c #E0E0D0",
	"@	c #4F484F",
	"#	c #909000",
	"$	c #FFF8EF",
	"%	c #CFC860",
	"&	c #003090",
	"*	c #7F7800",
	"=	c #FFC890",
	"-	c #FFF890",
	";	c #2F3000",
	"        .       ",
	"       .+@      ",
	"   ###.$$+@     ",
	"  #%%.$$$$+@    ",
	"  #%.$$$&$$+@** ",
	"  #.+++&+&+++@* ",
	"############++@ ",
	"#$$$$$$$$$=%#++@",
	"#$-------=-=#+; ",
	" #---=--=-==%#; ",
	" #-----=-=-==#; ",
	" #-=--=-=-=-=#; ",
	"  #=-=-=-=-==#; ",
	"  ############; ",
	"   ;;;;;;;;;;;  ",
	"                "};

static GdkPixmap *folder_pixmap = NULL, *ofolder_pixmap;
static GdkBitmap *folder_mask, *ofolder_mask;

typedef struct
{
	gboolean scanned;
	gchar *path;
}
DirNode;



static gboolean check_for_subdir(gchar * path)
{
	DIR *dir;
	struct dirent *dirent;
	struct stat statbuf;
	gchar *npath;

	if ((dir = opendir(path)))
	{
		while ((dirent = readdir(dir)))
		{
			if (dirent->d_name[0] != '.')
			{
				npath = g_strconcat(path, dirent->d_name, "/", NULL);
				stat(npath, &statbuf);
				g_free(npath);
				if (S_ISDIR(statbuf.st_mode))
				{
					closedir(dir);
					return TRUE;
				}
			}
		}
		closedir(dir);
	}
	return FALSE;
}

static void destroy_cb(gpointer data)
{
	DirNode *node = data;

	g_free(node->path);
	g_free(node);
}

static void expand_cb(GtkWidget * widget, GtkCTreeNode * parent_node)
{
	DIR *dir;
	struct dirent *dirent;
	gchar *path, *text, *dummy = "dummy";
	struct stat statbuf;
	GtkCTreeNode *node, *sub_node;
	DirNode *parent_dirnode, *dirnode;
	gboolean has_subdir = FALSE;

	parent_dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(widget), parent_node);
	if (!parent_dirnode->scanned)
	{
		gtk_clist_freeze(GTK_CLIST(widget));
		node = gtk_ctree_find_by_row_data(GTK_CTREE(widget), parent_node, NULL);
		gtk_ctree_remove_node(GTK_CTREE(widget), node);
		if ((dir = opendir(parent_dirnode->path)))
		{
			while ((dirent = readdir(dir)))
			{
				path = g_strconcat(parent_dirnode->path, dirent->d_name, NULL);
				stat(path, &statbuf);
				if (S_ISDIR(statbuf.st_mode) && dirent->d_name[0] != '.')
				{
					dirnode = g_malloc0(sizeof (DirNode));
					dirnode->path = g_strconcat(path, "/", NULL);
					text = dirent->d_name;
					if (check_for_subdir(dirnode->path))
						has_subdir = TRUE;
					else
						has_subdir = FALSE;
					node = gtk_ctree_insert_node(GTK_CTREE(widget), parent_node, NULL, &text, 4, folder_pixmap, folder_mask, ofolder_pixmap, ofolder_mask, !has_subdir, FALSE);
					gtk_ctree_node_set_row_data_full(GTK_CTREE(widget), node, dirnode, destroy_cb);
					if (has_subdir)
						sub_node = gtk_ctree_insert_node(GTK_CTREE(widget), node, NULL, &dummy, 4, NULL, NULL, NULL, NULL, FALSE, FALSE);
				}
				g_free(path);
			}
			closedir(dir);
			gtk_ctree_sort_node(GTK_CTREE(widget), parent_node);
		}
		gtk_clist_thaw(GTK_CLIST(widget));
		parent_dirnode->scanned = TRUE;
	}
}


static void ok_clicked(GtkWidget * widget, GtkWidget *tree)
{
        char *response = (char *)gtk_object_get_data (GTK_OBJECT (widget), "response");
	GtkCTreeNode *node;
	DirNode *dirnode;
	GList *list_node;

	list_node = GTK_CLIST(tree)->selection;
	while (list_node)
	{
		node = list_node->data;
		dirnode = gtk_ctree_node_get_row_data(GTK_CTREE(tree), node);
                obj_dirpath=g_strdup(dirnode->path);
		list_node = g_list_next(list_node);
	}

        if(strlen(obj_dirpath)<1)
	  *response = 0;
	else
          *response = 1;
        gtk_widget_destroy(GTK_WIDGET(window));
        gtk_main_quit();
}

static void cancel_clicked(GtkWidget * widget, GtkWidget *tree)
{
    gtk_widget_destroy(GTK_WIDGET(window));
    gtk_main_quit();
}


static int filetreeent_compare_func(const void *a, const void *b)
{
	if (!a || !b || !((DirNode *) a)->path)
		return -1;
	return strcmp(((DirNode *) a)->path, (gchar *) b);
}

GtkWidget *create_dir_browser(char *current_path,int *response)
{
	GtkWidget *scroll_win, *tree, *vbox, *bbox, *ok, *cancel,
	         *sep;
	gchar *root_text = "/", *node_text = "dummy";
	gchar *currentdir, *pos, *tpath, *tpathnew;
	GtkCTreeNode *root_node, *node, *nextnode;
	DirNode *dirnode;
	gboolean leaf;

	window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(window), "Select target directory");
	gtk_container_border_width(GTK_CONTAINER(window), 10);
        gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
        gtk_signal_connect(GTK_OBJECT(window), "destroy",
                       GTK_SIGNAL_FUNC(cancel_clicked),
                       NULL);

	vbox = gtk_vbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	scroll_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(scroll_win, 250, 200);
	gtk_box_pack_start(GTK_BOX(vbox), scroll_win, TRUE, TRUE, 0);
	gtk_widget_show(scroll_win);

	gtk_widget_realize(window);
	if (!folder_pixmap)
	{
		folder_pixmap = gdk_pixmap_create_from_xpm_d(window->window, &folder_mask, NULL, folder);
		ofolder_pixmap = gdk_pixmap_create_from_xpm_d(window->window, &ofolder_mask, NULL, ofolder);
	}

	tree = gtk_ctree_new(1, 0);
	gtk_clist_set_column_auto_resize(GTK_CLIST(tree), 0, TRUE);
	gtk_ctree_set_line_style(GTK_CTREE(tree), GTK_CTREE_LINES_DOTTED);
	gtk_signal_connect(GTK_OBJECT(tree), "tree_expand", GTK_SIGNAL_FUNC(expand_cb), NULL);
	gtk_container_add(GTK_CONTAINER(scroll_win), tree);

	root_node = gtk_ctree_insert_node(GTK_CTREE(tree), NULL, NULL, &root_text, 4, folder_pixmap, folder_mask, ofolder_pixmap, ofolder_mask, FALSE, FALSE);
	dirnode = g_malloc0(sizeof (DirNode));
	dirnode->path = g_strdup("/");
	gtk_ctree_node_set_row_data_full(GTK_CTREE(tree), root_node, dirnode, destroy_cb);
	node = gtk_ctree_insert_node(GTK_CTREE(tree), root_node, NULL, &node_text, 4, NULL, NULL, NULL, NULL, TRUE, TRUE);
	gtk_ctree_expand(GTK_CTREE(tree), root_node);
	gtk_widget_show(tree);

	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);
	gtk_widget_show(sep);

	bbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 5);

	ok = gtk_button_new_with_label("Ok");
	gtk_object_set_user_data(GTK_OBJECT(ok), window);
	GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);
	gtk_window_set_default(GTK_WINDOW(window), ok);
	gtk_box_pack_start(GTK_BOX(bbox), ok, TRUE, TRUE, 0);
	gtk_object_set_data (GTK_OBJECT (ok), "response", response);
	gtk_signal_connect(GTK_OBJECT(ok), "clicked", GTK_SIGNAL_FUNC(ok_clicked), tree);
	gtk_widget_show(ok);

	cancel = gtk_button_new_with_label("Cancel");
	GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(bbox), cancel, TRUE, TRUE, 0);
	gtk_signal_connect_object(GTK_OBJECT(cancel), "clicked", GTK_SIGNAL_FUNC(cancel_clicked), GTK_OBJECT(window));
	gtk_widget_show(cancel);

	gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 0);
	gtk_widget_show(bbox);
	gtk_widget_show(vbox);

	if (current_path && *current_path)
	{
		currentdir = g_strdup(current_path);
		tpath = g_strdup("/");
		pos = strtok(currentdir, "/");
		node = gtk_ctree_find_by_row_data_custom(GTK_CTREE(tree), NULL, "/", filetreeent_compare_func);
		do
		{
			tpathnew = g_strconcat(tpath, pos, "/", NULL);
			g_free(tpath);
			tpath = tpathnew;
			nextnode = gtk_ctree_find_by_row_data_custom(GTK_CTREE(tree), node, tpath, filetreeent_compare_func);
			if (!nextnode)
				break;
			node = nextnode;
			pos = strtok(NULL, "/");
			gtk_ctree_get_node_info(GTK_CTREE(tree), node, NULL, NULL, NULL, NULL, NULL, NULL, &leaf, NULL);
			if (!leaf && pos)
				gtk_ctree_expand(GTK_CTREE(tree), node);
			else
			{
				gtk_ctree_select(GTK_CTREE(tree), node);
				break;
			}
		}
		while (pos);
		g_free(tpath);
		g_free(currentdir);
	}
	else
		gtk_ctree_select(GTK_CTREE(tree), root_node);

	return window;
}

char *
obj_show_dirbrowser()
{
    GtkWidget   *dialog;
    int         response=0;

    dialog = create_dir_browser(getenv("$HOME"),&response);
    gtk_widget_show(dialog);
    gtk_main();

    if(response)
	return obj_dirpath;
    else
	return NULL;
}

