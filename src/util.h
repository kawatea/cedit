#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceiter.h>
#include <gtksourceview/gtksourceprintcompositor.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gdk/gdkkeysyms.h>

#define BUF_SIZE 100010

GtkWidget *main_window;
GtkWidget *scroll_window;
GtkSourceBuffer *buffer;
GtkWidget *view;
GtkWidget *name_label;
GtkWidget *popup_menu;
GtkActionGroup *actions;
char buf[BUF_SIZE];
char cbuf[BUF_SIZE];
char font_name[100];
char c_flymake[1000];
char cpp_flymake[1000];
int window_width, window_height, window_x, window_y;
int encoding;
int change_flag, kill_flag;
int state;
static const int width_mask = 31;
static const int wrap_mask = 1 << 5;
static const int line_mask = 1 << 6;
static const int auto_mask = 1 << 7;
static const int highlight_mask = 1 << 8;
static const int space_mask = 1 << 9;
static const int delete_mask = 1 << 10;

int empty_file_name(void);
const char *get_file_name(void);
const char *get_file_name_full(void);
void set_file_name(const char *name);
char *get_path(const char *file);
GdkPixbuf *get_icon(const char *file);
void load_setting(void);
void save_setting(void);
const char *get_language(void);
void set_language(void);
void set_style(void);
void set_action(const char *name, gboolean flag);

#endif