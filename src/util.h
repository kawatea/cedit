#ifndef UTIL
#define UTIL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceiter.h>
#include <gtksourceview/gtksourceprintcompositor.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gdk/gdkkeysyms.h>

GtkWidget *main_window;
GtkWidget *scroll_window;
GtkSourceBuffer *buffer;
GtkWidget *view;
GtkWidget *name_label;
FILE *fp;
char font_name[100];
int window_width, window_height;
int window_x, window_y;
int state;
static const int width_mask = 31;
static const int wrap_mask = 1 << 5;
static const int line_mask = 1 << 6;
static const int auto_mask = 1 << 7;
static const int highlight_mask = 1 << 8;
static const int space_mask = 1 << 9;
static const int delete_mask = 1 << 10;
int change_flag;
int kill_flag;
int text_id;

void set_start_end_iter(GtkTextIter *start, GtkTextIter *end);
void change_text(void);
void change_text_connect(void);
void change_window(void);
void load_setting(void);
void save_setting(void);

#endif