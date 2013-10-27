#ifndef __CALLBACK_H__
#define __CALLBACK_H__

int text_id;

void change_text(void);
void change_text_connect(void);
void change_window(void);
void change_encoding(GtkRadioAction *action, GtkRadioAction *current);
gboolean right_click(GtkWidget *widget, GdkEventButton *event);

#endif
