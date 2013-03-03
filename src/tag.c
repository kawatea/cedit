#include "util.h"
#include "tag.h"

typedef struct _list {
    GtkTextTag *tag;
    char *message;
    struct _list *next;
} list;

list *top;

void clear_list(list *now, GtkTextIter *start, GtkTextIter *end)
{
    if (now == NULL) return;
    
    clear_list(now->next, start, end);
    
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), now->tag, start, end);
    free(now->message);
    free(now->next);
}

void init_tag(void)
{
    search_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFFF00", NULL);
    match_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#40E0D0", NULL);
    unmatch_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#A020F0", NULL);
    
    top = NULL;
}

GtkTextTag *add_error_tag(char *message)
{
    if (top == NULL) {
        top = malloc(sizeof(list));
        
        if (strncmp(message, "error", 5) == 0) {
            top->tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFBBBB", NULL);
        } else {
            top->tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#B0E0E6", NULL);
        }
        
        top->message = malloc(strlen(message) + 1);
        strcpy(top->message, message);
        top->next = NULL;
        
        return top->tag;
    } else {
        list *now = top;
        
        while (now->next != NULL) now = now->next;
        
        now->next = malloc(sizeof(list));
        
        now->next->tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFBBBB", NULL);
        now->next->message = malloc(strlen(message) + 1);
        strcpy(now->next->message, message);
        now->next->next = NULL;
        
        return now->next->tag;
    }
}

void delete_tag(int flag)
{
    GtkTextIter start, end;
    
    set_start_end_iter(&start, &end);
    
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), search_tag, &start, &end);
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), match_tag, &start, &end);
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), unmatch_tag, &start, &end);
    
    if (flag == 1) {
        clear_list(top, &start, &end);
        
        top = NULL;
    }
}

gboolean popup(GtkWidget *widget, int x, int y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer user_data)
{
    int nx, ny, trail;
    GtkTextIter cursor;
    list *now;
    
    if (keyboard_mode == TRUE) return FALSE;
    
    gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(view), GTK_TEXT_WINDOW_TEXT, x, y, &nx, &ny);
    
    nx += gtk_adjustment_get_value(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll_window)));
    ny += gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll_window)));
    
    gtk_text_view_get_iter_at_position(GTK_TEXT_VIEW(view), &cursor, &trail, x, y);
    
    now = top;
    
    while (now != NULL) {
        if (gtk_text_iter_has_tag(&cursor, now->tag) == TRUE) {
            gtk_tooltip_set_text(tooltip, now->message);
            
            return TRUE;
        }
        
        now = now->next;
    }
    
    return FALSE;
}
