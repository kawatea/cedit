#include "util.h"
#include "tag.h"

typedef struct _tag_list {
    GtkTextTag *tag;
    char *message;
    struct _tag_list *next;
} tag_list;

tag_list *top, *last;

//タグを初期化する
void init_tag(void)
{
    top = last = NULL;
    
    search_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFFF00", NULL);
    match_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#40E0D0", NULL);
    unmatch_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#A020F0", NULL);
}

//タグを追加し新しく追加されたタグを返す
GtkTextTag *add_error_tag(const char *message)
{
    tag_list *new = malloc(sizeof(tag_list));
    
    //エラーと警告で色分け
    if (strncmp(message, "error", 5) == 0) {
        new->tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFBBBB", NULL);
    } else {
        new->tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#B0E0E6", NULL);
    }
    new->message = malloc(strlen(message) + 1);
    strcpy(new->message, message);
    new->next = NULL;
    
    if (top == NULL) {
        top = last = new;
    } else {
        last = last->next = new;
    }
    
    return new->tag;
}

//タグを外しリストを空にする
void clear_tag_list(tag_list *now, GtkTextIter *start, GtkTextIter *end)
{
    if (now == NULL) return;
    
    clear_tag_list(now->next, start, end);
    
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), now->tag, start, end);
    free(now->message);
    free(now);
}

//タグを取り除く
void delete_tag(int flag)
{
    GtkTextIter start, end;
    
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), search_tag, &start, &end);
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), match_tag, &start, &end);
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), unmatch_tag, &start, &end);
    
    if (flag == 1) {
        clear_tag_list(top, &start, &end);
        
        top = last = NULL;
    }
}

//ツールチップのテキストを設定する
gboolean popup(GtkWidget *widget, int x, int y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer user_data)
{
    if (keyboard_mode == TRUE) return FALSE;
    
    int nx, ny, trail;
    GtkTextIter cursor;
    tag_list *now = top;
    
    gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(view), GTK_TEXT_WINDOW_TEXT, x, y, &nx, &ny);
    gtk_text_view_get_iter_at_position(GTK_TEXT_VIEW(view), &cursor, &trail, nx, ny);
    
    while (now != NULL) {
        if (gtk_text_iter_has_tag(&cursor, now->tag) == TRUE) {
            gtk_tooltip_set_text(tooltip, now->message);
            
            return TRUE;
        }
        
        now = now->next;
    }
    
    return FALSE;
}
