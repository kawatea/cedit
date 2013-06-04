#ifndef MENU_FILE
#define MENU_FILE

void new_file(void);
void open_file(void);
void save_file(void);
void save_file_another(void);
void save_check(void);
void print_file(void);
gboolean quit(void);
void receive(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data);

#endif
