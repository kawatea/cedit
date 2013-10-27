#ifndef __TAG_H__
#define __TAG_H__

GtkTextTag *search_tag;
GtkTextTag *match_tag;
GtkTextTag *unmatch_tag;

void init_tag(void);
GtkTextTag *add_error_tag(const char *message);
void delete_tag(int flag);
gboolean popup(GtkWidget *widget, int x, int y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer user_data);

#endif
