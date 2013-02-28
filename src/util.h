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
int change_flag;
int kill_flag;
int tab_width;
int text_id;

void set_start_end_iter(GtkTextIter *start, GtkTextIter *end);
void change_text(void);
void change_text_connect(void);

#endif