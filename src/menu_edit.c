#include "util.h"
#include "menu_edit.h"

void undo_action(void)
{
    gtk_source_buffer_undo(buffer);
}

void redo_action(void)
{
    gtk_source_buffer_redo(buffer);
}

void cut_text(void)
{
    GtkClipboard *clipboard;
    GtkTextIter start, end;
    
    clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    gtk_clipboard_set_text(clipboard, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE), -1);
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
}

void copy_text(void)
{
    GtkClipboard *clipboard;
    GtkTextIter start, end;
    
    clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    gtk_clipboard_set_text(clipboard, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE), -1);
}

void paste_text(void)
{
    GtkClipboard *clipboard;
    GtkTextIter start, end;
    
    clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_paste_clipboard(GTK_TEXT_BUFFER(buffer), clipboard, NULL, TRUE);
}

void delete_text(void)
{
    gtk_text_buffer_delete_selection(GTK_TEXT_BUFFER(buffer), TRUE, TRUE);
}

void kill_line(void)
{
    int delete_flag = 0;
    GtkClipboard *clipboard;
    GtkTextIter start, end;
    
    clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    buf[0] = '\0';
    
    if (kill_flag == 1) strcat(buf, gtk_clipboard_wait_for_text(clipboard));
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    end = start;
    gtk_text_iter_forward_line(&end);
    
    if (!gtk_text_iter_starts_line(&start) && !gtk_text_iter_ends_line(&start) && !gtk_text_iter_is_end(&end) && kill_flag == 0) {
        gtk_text_iter_backward_char(&end);
    }
    
    strcat(buf, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
    
    if (buf[0] != '\0') delete_flag = 1;
    
    gtk_clipboard_set_text(clipboard, buf, -1);
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    kill_flag = delete_flag;
}

void select_all(void)
{
    GtkTextIter start, end;
    
    set_start_end_iter(&start, &end);
    
    gtk_text_buffer_select_range(GTK_TEXT_BUFFER(buffer), &start, &end);
}

void change_undo(void)
{
    set_action("Undo", gtk_source_buffer_can_undo(buffer));
}

void change_redo(void)
{
    set_action("Redo", gtk_source_buffer_can_redo(buffer));
}

void change_selection(void)
{
    gboolean flag = gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(buffer));
    
    set_action("Cut", flag);
    set_action("Copy", flag);
    set_action("Delete", flag);
}
