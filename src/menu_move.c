#include "util.h"
#include "menu_move.h"

//カーソルを指定された行に移動する
void jump_line(void)
{
    GtkWidget *jump_dialog = gtk_dialog_new_with_buttons("移動", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    GtkWidget *jump_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *jump_label = gtk_label_new("移動する行番号 : ");
    GtkWidget *jump_button = gtk_spin_button_new_with_range(1, gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(buffer)), 1);
    gtk_box_pack_start(GTK_BOX(jump_hbox), jump_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(jump_hbox), jump_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(jump_dialog)->vbox), jump_hbox, FALSE, FALSE, 0);
    gtk_entry_set_activates_default(GTK_ENTRY(jump_button), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(jump_dialog), GTK_RESPONSE_OK);
    gtk_widget_show_all(jump_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(jump_dialog)) == GTK_RESPONSE_OK) {
        GtkTextIter start;
        
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(jump_button)) - 1);
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &start, 0, FALSE, 0, 0);
    }
    
    gtk_widget_destroy(jump_dialog);
}

//カーソルをファイルの先頭に移動する
void jump_up(void)
{
    GtkTextIter start;
    
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &start, 0, FALSE, 0, 0);
}

//カーソルをファイルの末尾に移動する
void jump_down(void)
{
    GtkTextIter end;
    
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &end);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &end, 0, FALSE, 0, 0);
}

//カーソルを行頭に移動する
void jump_left(void)
{
    GtkTextIter start;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, gtk_text_iter_get_line(&start));
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
}

//カーソルを行末に移動する
void jump_right(void)
{
    int line;
    GtkTextIter end;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &end, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    line = gtk_text_iter_get_line(&end);
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &end, line);
    gtk_text_iter_forward_line(&end);
    if (gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(buffer)) > line + 1) gtk_text_iter_backward_char(&end);
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &end);
}
