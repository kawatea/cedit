#include "util.h"
#include "menu_edit.h"

//操作を元に戻す
void undo_action(void)
{
    gtk_source_buffer_undo(buffer);
}

//操作をやり直す
void redo_action(void)
{
    gtk_source_buffer_redo(buffer);
}

//選択された範囲を切り取る
void cut_text(void)
{
    GtkTextIter start, end;
    GtkClipboard *clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
    char *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE);
    gtk_clipboard_set_text(clipboard, text, -1);
    free(text);
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
}

//選択された範囲をコピーする
void copy_text(void)
{
    GtkTextIter start, end;
    GtkClipboard *clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
    char *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE);
    gtk_clipboard_set_text(clipboard, text, -1);
    free(text);
}

//クリップボードの内容を貼り付ける
void paste_text(void)
{
    GtkClipboard *clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_paste_clipboard(GTK_TEXT_BUFFER(buffer), clipboard, NULL, TRUE);
}

//選択された範囲を削除する
void delete_text(void)
{
    gtk_text_buffer_delete_selection(GTK_TEXT_BUFFER(buffer), TRUE, TRUE);
}

//カーソルのある行を削除する
void kill_line(void)
{
    int delete_flag = 0;
    GtkTextIter start, end;
    GtkClipboard *clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    buf[0] = '\0';
    if (kill_flag == 1) {
        char *text = gtk_clipboard_wait_for_text(clipboard);
        strcat(buf, text);
        free(text);
    }
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    end = start;
    gtk_text_iter_forward_line(&end);
    if (!gtk_text_iter_starts_line(&start) && !gtk_text_iter_ends_line(&start) && !gtk_text_iter_is_end(&end) && kill_flag == 0) gtk_text_iter_backward_char(&end);
    
    char *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE);
    strcat(buf, text);
    free(text);
    
    if (buf[0] != '\0') delete_flag = 1;
    
    gtk_clipboard_set_text(clipboard, buf, -1);
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    kill_flag = delete_flag;
}

//文章全てを選択する
void select_all(void)
{
    GtkTextIter start, end;
    
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    gtk_text_buffer_select_range(GTK_TEXT_BUFFER(buffer), &start, &end);
}

//元に戻せるかどうかの状態が変わったときの動作
void change_undo(void)
{
    set_action("Undo", gtk_source_buffer_can_undo(buffer));
}

//やり直せるかどうかの状態が変わったときの動作
void change_redo(void)
{
    set_action("Redo", gtk_source_buffer_can_redo(buffer));
}

//範囲を選択しているかどうかの状態が変わったときの動作
void change_selection(void)
{
    gboolean flag = gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(buffer));
    
    set_action("Cut", flag);
    set_action("Copy", flag);
    set_action("Delete", flag);
}
