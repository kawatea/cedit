#ifndef KEY
#define KEY

int auto_flag;
int delete_flag;

gboolean key_press(GtkWidget *widget, GdkEventKey *event);
gboolean key_release(GtkWidget *widget, GdkEventKey *evenr);

#endif
