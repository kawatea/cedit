#include "util.h"
#include "file.h"

void set_action(char *name, gboolean flag)
{
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, name), flag);
}

void set_radio_action(void)
{
    if (encoding == JIS) {
        strcpy(buf, "JIS");
    } else if (encoding == SJIS) {
        strcpy(buf, "SJIS");
    } else if (encoding == EUC) {
        strcpy(buf, "EUC");
    } else {
        strcpy(buf, "UTF8");
    }
    
    gtk_radio_action_set_current_value(GTK_RADIO_ACTION(gtk_action_group_get_action(actions, buf)), encoding);
}

void set_start_end_iter(GtkTextIter *start, GtkTextIter *end)
{
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), end);
}

void change_text(void)
{
    strcpy(buf, "* ");
    strcat(buf, gtk_label_get_text(GTK_LABEL(name_label)));
    
    gtk_label_set_text(GTK_LABEL(name_label), buf);
    
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

void change_window(void)
{
    gtk_window_get_size(GTK_WINDOW(main_window), &window_width, &window_height);
    gtk_window_get_position(GTK_WINDOW(main_window), &window_x, &window_y);
    
    gtk_container_resize_children(GTK_CONTAINER(main_window));
}

void load_setting(void)
{
    strcpy(buf, getenv("CEDIT"));
    strcat(buf, "/setting");
    
    fp = fopen(buf, "r");
    
    fgets(font_name, 100, fp);
    font_name[strlen(font_name) - 1] = '\0';
    fscanf(fp, "%d %d %d %d %d", &window_width, &window_height, &window_x, &window_y, &state);
    
    fclose(fp);
}

void save_setting(void)
{
    strcpy(buf, getenv("CEDIT"));
    strcat(buf, "/setting");
    
    fp = fopen(buf, "w");
    
    fprintf(fp, "%s\n", font_name);
    fprintf(fp, "%d %d %d %d %d\n", window_width, window_height, window_x, window_y, state);
    
    fclose(fp);
}
