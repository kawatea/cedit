#ifndef __MENU_SEARCH_H__
#define __MENU_SEARCH_H__

int cursor_id;

void change_find_entry(GtkWidget *entry, GObject *object, gpointer button);
void search_text(void);
void replace_text(void);

#endif
