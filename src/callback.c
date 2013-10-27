#include "util.h"
#include "callback.h"

//編集されたときにラベルを変更する
void change_text(void)
{
    char *name = malloc(1000);
    
    strcpy(name, "* ");
    strcat(name, gtk_label_get_text(GTK_LABEL(name_label)));
    gtk_label_set_text(GTK_LABEL(name_label), name);
    
    free(name);
    
    change_flag = 1;
    kill_flag = 0;
    
    set_action("New", TRUE);
    if (!empty_file_name()) set_action("Save", TRUE);
    
    g_signal_handler_disconnect(G_OBJECT(buffer), text_id);
}

//編集に対するシグナルを接続する
void change_text_connect(void)
{
    if (!g_signal_handler_is_connected(G_OBJECT(buffer), text_id)) text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
}

//ウィンドウのサイズを変更する
void change_window(void)
{
    gtk_window_get_size(GTK_WINDOW(main_window), &window_width, &window_height);
    gtk_window_get_position(GTK_WINDOW(main_window), &window_x, &window_y);
    gtk_container_resize_children(GTK_CONTAINER(main_window));
}

//エンコーディングを変更する
void change_encoding(GtkRadioAction *action, GtkRadioAction *current)
{
    encoding = gtk_radio_action_get_current_value(current);
}

//右クリックメニューを表示する
gboolean right_click(GtkWidget *widget, GdkEventButton *event)
{
    if (event->button == 3) {
        gtk_menu_popup(GTK_MENU(popup_menu), NULL, NULL, NULL, NULL, 0, event->time);
        
        return TRUE;
    } else {
        return FALSE;
    }
}
