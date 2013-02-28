#include "util.h"
#include "file.h"
#include "action.h"

void set_start_end_iter(GtkTextIter *start, GtkTextIter *end)
{
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), end);
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

void change_window(void)
{
    gtk_window_get_size(GTK_WINDOW(main_window), &window_width, &window_height);
    gtk_window_get_position(GTK_WINDOW(main_window), &window_x, &window_y);
}

void load_setting(void)
{
    char s[1000];
    FILE *fp;
    
    strcpy(s, getenv("CEDIT"));
    strcat(s, "/setting");
    
    fp = fopen(s, "r");
    
    fgets(font_name, 100, fp);
    font_name[strlen(font_name) - 1] = '\0';
    fscanf(fp, "%d %d %d %d %d", &window_width, &window_height, &window_x, &window_y, &state);
    
    fclose(fp);
}

void save_setting(void)
{
    char s[1000];
    FILE *fp;
    
    strcpy(s, getenv("CEDIT"));
    strcat(s, "/setting");
    
    fp = fopen(s, "w");
    
    fprintf(fp, "%s\n", font_name);
    fprintf(fp, "%d %d %d %d %d\n", window_width, window_height, window_x, window_y, state);
    
    fclose(fp);
}
