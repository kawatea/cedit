#include "util.h"
#include "tag.h"
#include "highlight_bracket.h"
#include "menu_search.h"

//検索ワードのエントリーの状態を変える
void change_find_entry(GtkWidget *entry, GObject *object, gpointer button)
{
    GdkColor color[] = {{0, 0xFFFF, 0xFFFF, 0xFFFF}};
    
    if (strcmp(gtk_entry_get_text(GTK_ENTRY(entry)), "") != 0) {
        gtk_widget_set_sensitive((GtkWidget *)button, TRUE);
    } else {
        gtk_widget_set_sensitive((GtkWidget *)button, FALSE);
    }
    
    gtk_widget_modify_base(entry, GTK_STATE_NORMAL, color);
}

//ダイアログを右上に移動する
void move_dialog(GtkWidget *widget, GtkRequisition *requisition, gpointer dialog)
{
    int width;
    
    gtk_window_get_size(GTK_WINDOW(dialog), &width, NULL);
    gtk_window_move(GTK_WINDOW(dialog), window_x + window_width - width, window_y);
}

void search_text_start(GtkWidget *entry, int case_flag, int back_flag, int wrap_flag)
{
    int find_flag = 0;
    GtkTextIter cursor, start, end;
    
    //与えられた文字列を検索
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &cursor, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    if (back_flag == 0) {
        find_flag = gtk_source_iter_forward_search(&cursor, gtk_entry_get_text(GTK_ENTRY(entry)), case_flag, &start, &end, NULL);
    } else {
        find_flag = gtk_source_iter_backward_search(&cursor, gtk_entry_get_text(GTK_ENTRY(entry)), case_flag, &start, &end, NULL);
    }
    
    if (find_flag == 0 && wrap_flag == 1) {
        if (back_flag == 0) {
            gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &cursor);
            
            find_flag = gtk_source_iter_forward_search(&cursor, gtk_entry_get_text(GTK_ENTRY(entry)), case_flag, &start, &end, NULL);
        } else {
            gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &cursor);
            
            find_flag = gtk_source_iter_backward_search(&cursor, gtk_entry_get_text(GTK_ENTRY(entry)), case_flag, &start, &end, NULL);
        }
    }
    
    //タグの更新とカーソルの移動
    delete_tag(0);
    
    if (find_flag == 1) {
        GdkColor color[] = {{0, 0xFFFF, 0xFFFF, 0xFFFF}};
        
        gtk_widget_modify_base(entry, GTK_STATE_NORMAL, color);
        
        if (back_flag == 0) {
            gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &end);
        } else {
            gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
        }
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), search_tag, &start, &end);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &start, 0, FALSE, 0, 0);
    } else {
        GdkColor color[] = {{0, 0xFFFF, 0xB600, 0xC100}};
        
        gtk_widget_modify_base(entry, GTK_STATE_NORMAL, color);
    }
}

void search_text(void)
{
    //検索ダイアログの設定
    GtkWidget *search_dialog = gtk_dialog_new_with_buttons("検索", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    GtkWidget *find_button = gtk_dialog_add_button(GTK_DIALOG(search_dialog), GTK_STOCK_FIND, GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(search_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    GtkWidget *search_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *search_label = gtk_label_new("検索する文字列 : ");
    GtkWidget *search_entry = gtk_entry_new();
    GtkWidget *case_button = gtk_check_button_new_with_label("大文字と小文字を区別する");
    GtkWidget *back_button = gtk_check_button_new_with_label("先頭に向かって検索する");
    GtkWidget *wrap_button = gtk_check_button_new_with_label("折り返しも対象にする");
    gtk_box_pack_start(GTK_BOX(search_hbox), search_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(search_hbox), search_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(search_dialog)->vbox), search_hbox, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(search_dialog)->vbox), case_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(search_dialog)->vbox), back_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(search_dialog)->vbox), wrap_button, FALSE, FALSE, 0);
    gtk_entry_set_activates_default(GTK_ENTRY(search_entry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(search_dialog), GTK_RESPONSE_OK);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wrap_button), TRUE);
    gtk_widget_set_sensitive(find_button, FALSE);
    g_signal_connect(G_OBJECT(search_entry), "notify::text", G_CALLBACK(change_find_entry), (gpointer)find_button);
    g_signal_connect(G_OBJECT(search_dialog), "size-request", G_CALLBACK(move_dialog), (gpointer)search_dialog);
    gtk_widget_show_all(search_dialog);
    
    while (gtk_dialog_run(GTK_DIALOG(search_dialog)) == GTK_RESPONSE_OK) {
        int case_flag = 0, back_flag = 0, wrap_flag = 0;
        
        if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(case_button))) case_flag = GTK_SOURCE_SEARCH_CASE_INSENSITIVE;
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(back_button))) back_flag = 1;
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wrap_button))) wrap_flag = 1;
        
        search_text_start(search_entry, case_flag, back_flag, wrap_flag);
    }
    
    gtk_widget_destroy(search_dialog);
}

void replace_text_start(GtkWidget *search_entry, GtkWidget *replace_entry, int case_flag)
{
    int select_flag = 0, find_flag = 0;
    GtkTextIter now, start, end;
    
    delete_tag(0);
    g_signal_handler_disconnect(G_OBJECT(buffer), cursor_id);
    
    //与えられた文字列を検索して置換
    if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(buffer))) {
        gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &now, &end);
        select_flag = 1;
    } else {
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &now, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    }
    
    while (1) {
        GtkTextIter match_start, match_end;
        
        if (select_flag == 1) {
            gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
        } else {
            gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
        }
        
        if (!gtk_source_iter_forward_search(&now, gtk_entry_get_text(GTK_ENTRY(search_entry)), case_flag, &match_start, &match_end, &end)) break;
        
        gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &match_start, &match_end);
        now = match_start;
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &now, gtk_entry_get_text(GTK_ENTRY(replace_entry)), -1);
        start = end = now;
        gtk_text_iter_backward_chars(&start, strlen(gtk_entry_get_text(GTK_ENTRY(replace_entry))));
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), search_tag, &start, &end);
        find_flag = 1;
    }
    
    //カーソルの移動
    if (find_flag == 1) {
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &now);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &now, 0, FALSE, 0, 0);
    } else {
        GdkColor color[] = {{0, 0xFFFF, 0xB600, 0xC100}};
        
        gtk_widget_modify_base(search_entry, GTK_STATE_NORMAL, color);
    }
    
    cursor_id = g_signal_connect(G_OBJECT(buffer), "notify::cursor-position", G_CALLBACK(change_cursor), NULL);
}

void replace_text(void)
{
    //置換ダイアログの設定
    GtkWidget *replace_dialog = gtk_dialog_new_with_buttons("置換", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    GtkWidget *replace_button = gtk_dialog_add_button(GTK_DIALOG(replace_dialog), GTK_STOCK_FIND_AND_REPLACE, GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(replace_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    GtkWidget *search_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *replace_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *search_label = gtk_label_new("検索する文字列 : ");
    GtkWidget *replace_label = gtk_label_new("置換する文字列 : ");
    GtkWidget *search_entry = gtk_entry_new();
    GtkWidget *replace_entry = gtk_entry_new();
    GtkWidget *case_button = gtk_check_button_new_with_label("大文字と小文字を区別する");
    gtk_box_pack_start(GTK_BOX(search_hbox), search_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(search_hbox), search_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(replace_hbox), replace_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(replace_hbox), replace_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(replace_dialog)->vbox), search_hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(replace_dialog)->vbox), replace_hbox, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(replace_dialog)->vbox), case_button, FALSE, FALSE, 0);
    gtk_entry_set_activates_default(GTK_ENTRY(search_entry), TRUE);
    gtk_entry_set_activates_default(GTK_ENTRY(replace_entry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(replace_dialog), GTK_RESPONSE_OK);
    gtk_widget_set_sensitive(replace_button, FALSE);
    g_signal_connect(G_OBJECT(search_entry), "notify::text", G_CALLBACK(change_find_entry), (gpointer)replace_button);
    g_signal_connect(G_OBJECT(replace_dialog), "size-request", G_CALLBACK(move_dialog), (gpointer)replace_dialog);
    gtk_widget_show_all(replace_dialog);
    
    while (gtk_dialog_run(GTK_DIALOG(replace_dialog)) == GTK_RESPONSE_OK) {
        int case_flag = 0;
        
        if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(case_button))) case_flag = GTK_SOURCE_SEARCH_CASE_INSENSITIVE;
        
        replace_text_start(search_entry, replace_entry, case_flag);
    }
    
    gtk_widget_destroy(replace_dialog);
}
