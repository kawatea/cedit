#include "util.h"
#include "file.h"
#include "action.h"

void set_start_end_iter(void)
{
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
}

void change_text(void)
{
    char s[1000];
    
    strcpy(s, "* ");
    strcat(s, gtk_label_get_text(GTK_LABEL(name_label)));
    
    gtk_label_set_text(GTK_LABEL(name_label), s);
    
    change_flag = 1;
    kill_flag = 0;
    
    set_action("New", TRUE);
    
    if (!empty_file_name()) set_action("Save", TRUE);
    
    g_signal_handler_disconnect(G_OBJECT(buffer), text_id);
}

void change_text_connect(void)
{
    if (!g_signal_handler_is_connected(G_OBJECT(buffer), text_id)) {
        text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
    }    
}
