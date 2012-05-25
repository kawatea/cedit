#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceiter.h>
#include <gtksourceview/gtksourceprintcompositor.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gdk/gdkkeysyms.h>

int text_id, cursor_id;
int change_flag, kill_flag, highlight_flag;
int tab_width = 4, tab_flag, auto_flag = 1, delete_flag = 1;
char file_name[1000] = "\0";
char buf[10000];
GtkWidget *main_window;
GtkWidget *name_label;
GtkWidget *view;
GtkSourceBuffer *buffer;
GtkActionGroup *actions;

void save_check(void);
void change_text(void);
void change_cursor(void);

void read_file(void)
{
    FILE *fp;
    GtkTextIter end;
    
    if ((fp = fopen(file_name, "r")) != NULL) {
        gtk_source_buffer_begin_not_undoable_action(buffer);
        
        while (fgets(buf, 5000, fp) != NULL) {
            gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
            
            gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, buf, strlen(buf));
        }
        
        fclose(fp);
        
        gtk_source_buffer_end_not_undoable_action(buffer);
    }
}

void write_file(void)
{
    FILE *fp;
    GtkTextIter start, end;
    
    fp = fopen(file_name, "w");
    
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    
    fprintf(fp, "%s", gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
    
    fclose(fp);
}

void delete_file(void)
{
    GtkTextIter start, end;
    
    file_name[0] = '\0';
    
    gtk_source_buffer_begin_not_undoable_action(buffer);
    
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    gtk_source_buffer_end_not_undoable_action(buffer);
}

int empty_file_name(void)
{
    return file_name[0] == '\0';
}

char *get_file_name(void)
{
    int i;
    
    for (i = strlen(file_name); i >= 0; i--) {
        if (file_name[i] == '/' || file_name[i] == '\\') break;
    }
    
    i++;
    
    return &file_name[i];
}

void set_file_name(char *name)
{
    strcpy(file_name, name);
}

void set_language(void)
{
    GtkSourceLanguageManager *manager;
    char *s[] = {NULL, NULL};
    
    s[0] = malloc(1000);
    strcpy(s[0], getenv("HOME"));
    strcat(s[0], "/cedit/language/");
    
    manager = gtk_source_language_manager_new();
    gtk_source_language_manager_set_search_path(manager, s);
    gtk_source_buffer_set_language(buffer, gtk_source_language_manager_guess_language(manager, file_name, NULL));
    
    free(s[0]);
}

void set_style(void)
{
    GtkSourceStyleSchemeManager *manager;
    char *s[] = {NULL, NULL};
    
    s[0] = malloc(1000);
    strcpy(s[0], getenv("HOME"));
    strcat(s[0], "/cedit/style/");
    
    manager = gtk_source_style_scheme_manager_new();
    gtk_source_style_scheme_manager_set_search_path(manager, s);
    gtk_source_buffer_set_style_scheme(buffer, gtk_source_style_scheme_manager_get_scheme(manager, "mystyle"));
    
    free(s[0]);
}

void clear_highlight(void)
{
    GtkTextIter start, end;
    GtkTextTag *tag;
    
    if (highlight_flag == 1) {
        tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFFFFF", NULL);
        
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
        gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
        
        highlight_flag = 0;
    }    
}

void new_file(void)
{
    if (change_flag == 1) {
        save_check();
        
        if (change_flag == 1) return;
    }
    
    delete_file();
    
    gtk_label_set_text(GTK_LABEL(name_label), "編集中のファイル");
    
    change_flag = kill_flag = 0;
    
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "New"), FALSE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Save"), FALSE);
    
    if (!g_signal_handler_is_connected(G_OBJECT(buffer), text_id)) {
        text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
    }
}

void open_file(void)
{
    GtkWidget *open_dialog;
    
    if (change_flag == 1) {
        save_check();
        
        if (change_flag == 1) return;
    }
    
    open_dialog = gtk_file_chooser_dialog_new("開く", GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    
    gtk_dialog_set_default_response(GTK_DIALOG(open_dialog), GTK_RESPONSE_ACCEPT);
    
    if (gtk_dialog_run(GTK_DIALOG(open_dialog)) == GTK_RESPONSE_ACCEPT) {
        delete_file();
        
        set_file_name(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(open_dialog)));
        
        read_file();
        
        gtk_label_set_text(GTK_LABEL(name_label), get_file_name());
        
        set_language();
        
        change_flag = kill_flag = 0;
        
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "New"), TRUE);
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Save"), FALSE);
        
        if (!g_signal_handler_is_connected(G_OBJECT(buffer), text_id)) {
            text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
        }
    }
    
    gtk_widget_destroy(open_dialog);
}

void save_file(void)
{
    write_file();
    
    gtk_label_set_text(GTK_LABEL(name_label), get_file_name());
    
    change_flag = kill_flag = 0;
    
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "New"), TRUE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Save"), FALSE);
    
    if (!g_signal_handler_is_connected(G_OBJECT(buffer), text_id)) {
        text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
    }
}

void save_file_another(void)
{
    GtkWidget *save_dialog;
    
    save_dialog = gtk_file_chooser_dialog_new("名前をつけて保存", GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    
    gtk_dialog_set_default_response(GTK_DIALOG(save_dialog), GTK_RESPONSE_ACCEPT);
    
    gtk_widget_show_all(save_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(save_dialog)) == GTK_RESPONSE_ACCEPT) {
        set_file_name(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(save_dialog)));
        
        write_file();
        
        gtk_label_set_text(GTK_LABEL(name_label), get_file_name());
        
        set_language();
        
        change_flag = kill_flag = 0;
        
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "New"), TRUE);
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Save"), FALSE);
        
        if (!g_signal_handler_is_connected(G_OBJECT(buffer), text_id)) {
            text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
        }
    }
    
    gtk_widget_destroy(save_dialog);
}

void save_check(void)
{
    GtkWidget *save_check_dialog;
    GtkWidget *save_check_label;
    gint response;
    
    save_check_dialog = gtk_dialog_new_with_buttons("保存", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_YES, GTK_RESPONSE_YES, GTK_STOCK_NO, GTK_RESPONSE_NO, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    save_check_label = gtk_label_new("編集中のファイルを保存しますか？");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(save_check_dialog)->vbox), save_check_label, FALSE, FALSE, 0);
    
    gtk_dialog_set_default_response(GTK_DIALOG(save_check_dialog), GTK_RESPONSE_YES);
    
    gtk_widget_show_all(save_check_dialog);
    
    response = gtk_dialog_run(GTK_DIALOG(save_check_dialog));
    
    if (response == GTK_RESPONSE_YES) {
        if (!empty_file_name()) {
            save_file();
        } else {
            save_file_another();
        }
    } else if (response == GTK_RESPONSE_NO) {
        change_flag = 0;
    }
    
    gtk_widget_destroy(save_check_dialog);
}

void begin_print(GtkPrintOperation *operation, GtkPrintContext *context, GtkSourcePrintCompositor *compositor)
{
    while (!gtk_source_print_compositor_paginate(compositor, context)) ;

    gtk_print_operation_set_n_pages(operation, gtk_source_print_compositor_get_n_pages(compositor));
}

void draw_page(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkSourcePrintCompositor *compositor)
{
    gtk_source_print_compositor_draw_page(compositor, context, page_nr);
}

void print_file(void)
{
    GtkPrintOperation *op;
    GtkSourcePrintCompositor *compositor;

    op = gtk_print_operation_new();
    gtk_print_operation_set_print_settings(op, gtk_print_settings_new());
    
    compositor = gtk_source_print_compositor_new(buffer);
    gtk_source_print_compositor_set_wrap_mode(compositor, GTK_WRAP_CHAR);
    gtk_source_print_compositor_set_print_line_numbers(compositor, 1);

    g_signal_connect(op, "begin_print", G_CALLBACK(begin_print), compositor);
    g_signal_connect(op, "draw_page", G_CALLBACK(draw_page), compositor);

    gtk_print_operation_run(op, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, NULL, NULL);
}

void quit(void)
{
    if (change_flag == 1) {
        save_check();
        
        if (change_flag == 1) return;
    }
    
    gtk_main_quit();
}

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
    
    clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_cut_clipboard(GTK_TEXT_BUFFER(buffer), clipboard, TRUE);
}

void copy_text(void)
{
    GtkClipboard *clipboard;
    
    clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_copy_clipboard(GTK_TEXT_BUFFER(buffer), clipboard);
}

void paste_text(void)
{
    GtkClipboard *clipboard;
    
    clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    gtk_text_buffer_paste_clipboard(GTK_TEXT_BUFFER(buffer), clipboard, NULL, TRUE);
}

void delete_text(void)
{
    gtk_text_buffer_delete_selection(GTK_TEXT_BUFFER(buffer), TRUE, TRUE);
}

void kill_line(void)
{
    GtkClipboard *clipboard;
    GtkTextIter start, end;
    int delete_flag = 0;
    
    clipboard = gtk_widget_get_clipboard(view, GDK_SELECTION_CLIPBOARD);
    
    buf[0] = '\0';
    
    if (kill_flag == 1) strcat(buf, gtk_clipboard_wait_for_text(clipboard));
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    end = start;
    gtk_text_iter_forward_line(&end);
    
    strcat(buf, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
    
    if (buf[0] != '\0') delete_flag = 1;
    
    gtk_clipboard_set_text(clipboard, buf, -1);
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    kill_flag = delete_flag;
}

void select_all(void)
{
    GtkTextIter start, end;
    
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    
    gtk_text_buffer_select_range(GTK_TEXT_BUFFER(buffer), &start, &end);
}

void change_entry(GtkWidget *entry, GObject *object, gpointer button)
{
    GdkColor color[] = {{0, 0xFFFF, 0xFFFF, 0xFFFF}};
    
    strcpy(buf, gtk_entry_get_text(GTK_ENTRY(entry)));
    
    if (buf[0] != '\0') {
        gtk_widget_set_sensitive((GtkWidget *)button, TRUE);
    } else {
        gtk_widget_set_sensitive((GtkWidget *)button, FALSE);
    }
    
    gtk_widget_modify_base(entry, GTK_STATE_NORMAL, color);
}

void search_text_start(GtkWidget *entry, int case_flag, int back_flag, int wrap_flag)
{
    int find_flag = 0;
    GtkTextIter cursor, start, end;
    GtkTextTag *tag;
    
    clear_highlight();
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &cursor, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    strcpy(buf, gtk_entry_get_text(GTK_ENTRY(entry)));
    
    if (back_flag == 0) {
        find_flag = gtk_source_iter_forward_search(&cursor, buf, case_flag, &start, &end, NULL);
    } else {
        find_flag = gtk_source_iter_backward_search(&cursor, buf, case_flag, &start, &end, NULL);
    }
    
    if (find_flag == 0 && wrap_flag == 1) {
        if (back_flag == 0) {
            gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &cursor);
            
            find_flag = gtk_source_iter_forward_search(&cursor, buf, case_flag, &start, &end, NULL);
        } else {
            gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &cursor);
            
            find_flag = gtk_source_iter_backward_search(&cursor, buf, case_flag, &start, &end, NULL);
        }
    }
    
    if (find_flag == 1) {
        GdkColor color[] = {{0, 0xFFFF, 0xFFFF, 0xFFFF}};
        
        gtk_widget_modify_base(entry, GTK_STATE_NORMAL, color);
        
        if (back_flag == 0) {
            gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &end);
        } else {
            gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
        }
        
        tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFFF00", NULL);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
        
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &start, 0, FALSE, 0, 0);
        
        highlight_flag = 1;
    } else {
        GdkColor color[] = {{0, 0xFFFF, 0xB600, 0xC100}};
        
        gtk_widget_modify_base(entry, GTK_STATE_NORMAL, color);
    }
}

void search_text(void)
{
    GtkWidget *search_dialog;
    GtkWidget *search_hbox;
    GtkWidget *search_label;
    GtkWidget *search_entry;
    GtkWidget *case_button;
    GtkWidget *back_button;
    GtkWidget *wrap_button;
    GtkWidget *find_button;
    
    search_dialog = gtk_dialog_new_with_buttons("検索", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    find_button = gtk_dialog_add_button(GTK_DIALOG(search_dialog), GTK_STOCK_FIND, GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(search_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    search_hbox = gtk_hbox_new(FALSE, 2);
    search_label = gtk_label_new("検索する文字列 : ");
    search_entry = gtk_entry_new();
    case_button = gtk_check_button_new_with_label("大文字と小文字を区別する");
    back_button = gtk_check_button_new_with_label("先頭に向かって検索する");
    wrap_button = gtk_check_button_new_with_label("折り返しも対象にする");
    
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
    
    g_signal_connect(G_OBJECT(search_entry), "notify::text", G_CALLBACK(change_entry), (gpointer)find_button);
    
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
    GtkTextTag *tag;
    
    clear_highlight();
    
    g_signal_handler_disconnect(G_OBJECT(buffer), cursor_id);
    
    if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(buffer))) {
        gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &now, &end);
        
        select_flag = 1;
    } else {
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &now, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    }
    
    tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFFF00", NULL);
    
    strcpy(buf, gtk_entry_get_text(GTK_ENTRY(search_entry)));
    
    while (1) {
        GtkTextIter match_start, match_end;
        
        if (!gtk_source_iter_forward_search(&now, buf, case_flag, &match_start, &match_end, NULL)) break;
        
        if (select_flag == 1) {
            gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
        } else {
            gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
        }
        
        if (gtk_text_iter_compare(&match_end, &end) == 1) break;
        
        gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &match_start, &match_end);
        
        now = match_start;
        
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &now, gtk_entry_get_text(GTK_ENTRY(replace_entry)), -1);
        
        start = end = now;
        gtk_text_iter_backward_chars(&start, strlen(gtk_entry_get_text(GTK_ENTRY(replace_entry))));
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
        
        find_flag = 1;
    }
    
    if (find_flag == 1) {
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &now);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &now, 0, FALSE, 0, 0);
        
        highlight_flag = 1;
    } else {
        GdkColor color[] = {{0, 0xFFFF, 0xB600, 0xC100}};
        
        gtk_widget_modify_base(search_entry, GTK_STATE_NORMAL, color);
    }
    
    cursor_id = g_signal_connect(G_OBJECT(buffer), "notify::cursor-position", G_CALLBACK(change_cursor), NULL);
}

void replace_text(void)
{
    GtkWidget *replace_dialog;
    GtkWidget *search_hbox;
    GtkWidget *replace_hbox;
    GtkWidget *search_label;
    GtkWidget *replace_label;
    GtkWidget *search_entry;
    GtkWidget *replace_entry;
    GtkWidget *case_button;
    GtkWidget *replace_button;
    
    replace_dialog = gtk_dialog_new_with_buttons("置換", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    replace_button = gtk_dialog_add_button(GTK_DIALOG(replace_dialog), GTK_STOCK_FIND_AND_REPLACE, GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(replace_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    search_hbox = gtk_hbox_new(FALSE, 2);
    replace_hbox = gtk_hbox_new(FALSE, 2);
    search_label = gtk_label_new("検索する文字列 : ");
    replace_label = gtk_label_new("置換する文字列 : ");
    search_entry = gtk_entry_new();
    replace_entry = gtk_entry_new();
    case_button = gtk_check_button_new_with_label("大文字と小文字を区別する");
    
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
    
    g_signal_connect(G_OBJECT(search_entry), "notify::text", G_CALLBACK(change_entry), (gpointer)replace_button);
    
    gtk_widget_show_all(replace_dialog);
    
    while (gtk_dialog_run(GTK_DIALOG(replace_dialog)) == GTK_RESPONSE_OK) {
        int case_flag = 0;
        
        if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(case_button))) case_flag = GTK_SOURCE_SEARCH_CASE_INSENSITIVE;
        
        replace_text_start(search_entry, replace_entry, case_flag);
    }
    
    gtk_widget_destroy(replace_dialog);
}

void jump_line(void)
{
    GtkWidget *jump_dialog;
    GtkWidget *jump_hbox;
    GtkWidget *jump_label;
    GtkWidget *jump_button;
    GtkTextIter start;
    
    jump_dialog = gtk_dialog_new_with_buttons("移動", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    jump_hbox = gtk_hbox_new(FALSE, 2);
    jump_label = gtk_label_new("移動する行番号 : ");
    jump_button = gtk_spin_button_new_with_range(1, gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(buffer)), 1);
    
    gtk_box_pack_start(GTK_BOX(jump_hbox), jump_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(jump_hbox), jump_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(jump_dialog)->vbox), jump_hbox, FALSE, FALSE, 0);
    
    gtk_entry_set_activates_default(GTK_ENTRY(jump_button), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(jump_dialog), GTK_RESPONSE_OK);
    
    gtk_widget_show_all(jump_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(jump_dialog)) == GTK_RESPONSE_OK) {
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(jump_button)) - 1);
        
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &start, 0, FALSE, 0, 0);
    }
    
    gtk_widget_destroy(jump_dialog);
}

void jump_up(void)
{
    GtkTextIter start;
    
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &start, 0, FALSE, 0, 0);
}

void jump_down(void)
{
    GtkTextIter end;
    
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &end);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &end, 0, FALSE, 0, 0);
}

void jump_left(void)
{
    int line;
    GtkTextIter start;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    line = gtk_text_iter_get_line(&start);
    
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
    
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
}

void jump_right(void)
{
    int line;
    GtkTextIter end;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &end, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    line = gtk_text_iter_get_line(&end);
    
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &end, line);
    
    gtk_text_iter_forward_line(&end);
    
    if (gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(buffer)) > line + 1) {
        gtk_text_iter_backward_char(&end);
    }
    
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &end);
}

void set_font(void)
{
    GtkWidget *font_dialog;
    
    font_dialog = gtk_font_selection_dialog_new("フォント");
    
    gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(font_dialog), pango_font_description_to_string(gtk_widget_get_style(view)->font_desc));
    gtk_font_selection_dialog_set_preview_text(GTK_FONT_SELECTION_DIALOG(font_dialog), "abcdefghij ABCDEFGHIJ あいうえお");
    
    gtk_dialog_set_default_response(GTK_DIALOG(font_dialog), GTK_RESPONSE_OK);
    
    if (gtk_dialog_run(GTK_DIALOG(font_dialog)) == GTK_RESPONSE_OK) {
        gtk_widget_modify_font(view, pango_font_description_from_string(gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(font_dialog))));
    }
    
    gtk_widget_destroy(font_dialog);
}

void set_tab_width(void)
{
    GtkWidget *tab_dialog;
    GtkWidget *tab_hbox;
    GtkWidget *tab_label;
    GtkWidget *tab_button;
    
    tab_dialog = gtk_dialog_new_with_buttons("タブ", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    tab_hbox = gtk_hbox_new(FALSE, 2);
    tab_label = gtk_label_new("新しいタブの幅 : ");
    tab_button = gtk_spin_button_new_with_range(1, 32, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(tab_button), tab_width);
    
    gtk_box_pack_start(GTK_BOX(tab_hbox), tab_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tab_hbox), tab_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(tab_dialog)->vbox), tab_hbox, FALSE, FALSE, 0);
    
    gtk_entry_set_activates_default(GTK_ENTRY(tab_button), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(tab_dialog), GTK_RESPONSE_OK);
    
    gtk_widget_show_all(tab_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(tab_dialog)) == GTK_RESPONSE_OK) {
        tab_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tab_button));
        
        gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view), tab_width);
    }
    
    gtk_widget_destroy(tab_dialog);
}

void set_wrap_line(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Wrap")))) {
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_CHAR);
    } else {
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_NONE);
    }
}

void set_display_line_number(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Line")))) {
        gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
    } else {
        gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), FALSE);
    }
}

void set_auto_indent(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Indent")))) {
        auto_flag = 1;
    } else {
        auto_flag = 0;
    }
}

void set_highlight_line(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Highlight")))) {
        gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), TRUE);
    } else {
        gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), FALSE);
    }
}

void set_insert_space(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Space")))) {
        tab_flag = 0;
    } else {
        tab_flag = 1;
    }
}

void set_delete_hungry(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Hungry")))) {
        delete_flag = 1;
    } else {
        delete_flag = 0;
    }
}

void help(void)
{
    GtkWidget *help_dialog;
    
    help_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_OTHER, GTK_BUTTONS_CLOSE, "一行削除について\n　カーソルから行末までを削除する\n　連続で消去したものはバッファに追加されまとめて貼り付けできる\n\n置換について\n　カーソルから文末までを検索して置換する\n　ただし選択された範囲があればその中だけを検索して置換する\n\nインデントについて\n　C(C++)の形式でインデントする\n　オートインデントの場合改行すると自動的にインデントを行う\n　Tabを押すとカーソルのある行のインデントを行う\n　範囲を選択してTabを押すとその中のすべての行のインデントを行う\n");
    
    gtk_dialog_run(GTK_DIALOG(help_dialog));
    
    gtk_widget_destroy(help_dialog);
}

void version(void)
{
    strcpy(buf, getenv("HOME"));
    strcat(buf, "/cedit/icon/logo.png");
    
    gtk_show_about_dialog(GTK_WINDOW(main_window), "name", "cedit", "version", "1.0", "copyright", "(C) 2012 kawatea", "logo", gdk_pixbuf_new_from_file(buf, NULL), NULL);
}

void highlight_bracket_backward(GtkTextIter now)
{
    int num = 0, find_flag = 0;
    char s[1000];
    GtkTextIter start, end;
    GtkTextTag *tag;
    
    start = now;
    
    while (!gtk_text_iter_is_start(&start)) {
        gunichar c = gtk_text_iter_get_char(&start);
        
        if (c == '"' || c == '\'') {
            int count = 0;
            
            while (1) {
                if (!gtk_text_iter_backward_char(&start)) break;
                
                if (gtk_text_iter_get_char(&start) == '\\') {
                    count++;
                } else {
                    break;
                }
            }
            
            if (count % 2 == 1) continue;
            
            while (1) {
                if (gtk_text_iter_get_char(&start) == c) {
                    count = 0;
                    
                    while (1) {
                        if (!gtk_text_iter_backward_char(&start)) break;
                        
                        if (gtk_text_iter_get_char(&start) == '\\') {
                            count++;
                        } else {
                            break;
                        }
                    }
                    
                    if (count % 2 == 0) break;
                    
                    continue;
                }
                
                if (!gtk_text_iter_backward_char(&start)) break;
            }
            
            continue;
        }

        if (c == '}' || c == ')' || c == ']') {
            s[num++] = c;
        } else if (c == '{') {
            if (s[num - 1] == '}') {
                num--;
            } else {
                break;
            }
        } else if (c == '(') {
            if (s[num - 1] == ')') {
                num--;
            } else {
                break;
            }
        } else if (c == '[') {
            if (s[num - 1] == ']') {
                num--;
            } else {
                break;
            }
        }
        
        if (num == 0) {
            find_flag = 1;
            
            break;
        }
        
        if (!gtk_text_iter_backward_char(&start)) break;
    }
    
    if (find_flag == 1) {
        tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#40E0D0", NULL);
        
        end = start;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
        
        start = end = now;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
    } else {
        tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#A020F0", NULL);
        
        start = end = now;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
    }
}

void highlight_bracket_forward(GtkTextIter now)
{
    int num = 0, find_flag = 0;
    char s[1000];
    GtkTextIter start, end;
    GtkTextTag *tag;
    
    start = now;
    
    while (!gtk_text_iter_is_end(&start)) {
        gunichar c = gtk_text_iter_get_char(&start);
        
        if (c == '\\') {
            if (!gtk_text_iter_forward_chars(&start, 2)) break;
            
            continue;
        }

        if (c == '"' || c == '\'') {
            while (1) {
                if (!gtk_text_iter_forward_char(&start)) break;
                
                while (gtk_text_iter_get_char(&start) == '\\') {
                    if (!gtk_text_iter_forward_chars(&start, 2)) break;
                }
                
                if (gtk_text_iter_get_char(&start) == c) break;
            }
            
            if (!gtk_text_iter_forward_char(&start)) break;
            
            continue;
        }

        if (c == '{' || c == '(' || c == '[') {
            s[num++] = c;
        } else if (c == '}') {
            if (s[num - 1] == '{') {
                num--;
            } else {
                break;
            }
        } else if (c == ')') {
            if (s[num - 1] == '(') {
                num--;
            } else {
                break;
            }
        } else if (c == ']') {
            if (s[num - 1] == '[') {
                num--;
            } else {
                break;
            }
        }
        
        if (num == 0) {
            find_flag = 1;
            
            break;
        }
        
        if (!gtk_text_iter_forward_char(&start)) break;
    }
    
    if (find_flag == 1) {
        tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#40E0D0", NULL);
        
        end = start;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
        
        start = end = now;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
    } else {
        tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#A020F0", NULL);
        
        start = end = now;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), tag, &start, &end);
    }
}

void highlight_bracket(void)
{
    gunichar c;
    GtkTextIter start, end;
    
    clear_highlight();
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    end = start;
    
    gtk_text_iter_backward_char(&start);
    
    c = gtk_text_iter_get_char(&start);
    
    if (c == '}' || c == ')' || c == ']') {
        highlight_bracket_backward(start);
        
        highlight_flag = 1;
        
        return;
    }
    
    c = gtk_text_iter_get_char(&end);
    
    if (c == '{' || c == '(' || c == '[') {
        highlight_bracket_forward(end);
        
        highlight_flag = 1;
    }
}

void change_text(void)
{
    strcpy(buf, "* ");
    strcat(buf, gtk_label_get_text(GTK_LABEL(name_label)));
    
    gtk_label_set_text(GTK_LABEL(name_label), buf);
    
    change_flag = 1;
    kill_flag = 0;
    
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "New"), TRUE);
    if (!empty_file_name()) {
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Save"), TRUE);
    }
    
    g_signal_handler_disconnect(G_OBJECT(buffer), text_id);
}

void change_cursor(void)
{
    kill_flag = 0;
    
    highlight_bracket();
}

void change_undo(void)
{
    if (gtk_source_buffer_can_undo(buffer)) {
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Undo"), TRUE);
    } else {
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Undo"), FALSE);
    }
}

void change_redo(void)
{
    if (gtk_source_buffer_can_redo(buffer)) {
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Redo"), TRUE);
    } else {
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Redo"), FALSE);
    }
}

void change_selection(void)
{
    if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(buffer))) {
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Cut"), TRUE);
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Copy"), TRUE);
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Delete"), TRUE);
    } else {
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Cut"), FALSE);
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Copy"), FALSE);
        gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Delete"), FALSE);
    }
}

int get_indent_depth(int line)
{
    int num = 0, len, flag = 0, i;
    GtkTextIter start, end;
    
    if (line < 0) return 0;
    
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &end, line + 1);
    
    strcpy(buf, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
    
    len = strlen(buf) - 1;
    
    if (len == 0) return get_indent_depth(line - 1);
    
    for (i = 0; i < len; i++) {
        if (buf[i] == ' ') {
            num++;
        } else if (buf[i] == '\t') {
            num += tab_width;
        } else {
            break;
        }
    }
    
    for (i = 0; i < len; i++) {
        if (buf[i] == '\\') {
            i++;
            
            continue;
        }
        
        if (buf[i] == '"' || buf[i] == '\'') {
            char c = buf[i];
            
            for (i++; i < len; i++) {
                if (buf[i] == '\\') {
                    i++;
                    
                    continue;
                }
                
                if (buf[i] == c) break;
            }
            
            continue;
        }
        
        if (buf[i] == '{') {
            flag++;
        } else if (buf[i] == '}') {
            flag--;
            
            if (flag < 0) flag = 0;
        }
    }
    
    if (flag > 0) num += tab_width;
    
    if (num < 0) num = 0;
    
    return num;
}

void set_indent_depth(int line, int depth)
{
    int len, flag = 0, i;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
    
    end = start;
    
    gtk_text_iter_forward_line(&end);
    
    strcpy(buf, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
    
    len = strlen(buf);
    
    for (i = 0; i < len; i++) {
        if (buf[i] == '\\') {
            i++;
            
            continue;
        }
        
        if (buf[i] == '"' || buf[i] == '\'') {
            char c = buf[i];
            
            for (i++; i < len; i++) {
                if (buf[i] == '\\') {
                    i++;
                    
                    continue;
                }
                
                if (buf[i] == c) break;
            }
            
            continue;
        }
        
        if (buf[i] == '{') {
            flag++;
        } else if (buf[i] == '}') {
            if (flag == 0) {
                depth -= tab_width;
            } else {
                flag--;
            }
        }
    }
    
    if (depth < 0) depth = 0;
    
    for (i = 0; i < len; i++) {
        if (buf[i] != ' ' && buf[i] != '\t') break;
    }
    
    end = start;
    
    gtk_text_iter_forward_chars(&end, i);
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    if (tab_flag == 1) {
        depth /= tab_width;
        
        for (i = 0; i < depth; i++) buf[i] = '\t';
        
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &start, buf, depth);
    } else {
        for (i = 0; i < depth; i++) buf[i] = ' ';
        
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &start, buf, depth);
    }
}

void set_indent(void)
{
    int line, depth;
    GtkTextIter cursor;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &cursor, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    line = gtk_text_iter_get_line(&cursor);
    
    depth = get_indent_depth(line - 1);
    
    set_indent_depth(line, depth);
}

void set_indent_all(void)
{
    int line_start, line_end, i;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    line_start = gtk_text_iter_get_line(&start);
    line_end = gtk_text_iter_get_line(&end);
    
    for (i = line_start; i <= line_end; i++) {
        int depth = get_indent_depth(i - 1);
        
        set_indent_depth(i, depth);
    }
}

int delete_hungry_backward(void)
{
    gunichar c;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    end = start;
    
    if (!gtk_text_iter_backward_char(&start)) return 0;
    
    c = gtk_text_iter_get_char(&start);
    
    if (c != ' ' && c != '\t') return 0;
    
    while (1) {
        if (!gtk_text_iter_backward_char(&start)) break;
        
        c = gtk_text_iter_get_char(&start);
        
        if (c != ' ' && c != '\t') {
            c = gtk_text_iter_forward_char(&start);
            
            break;
        }
    }
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    return 1;
}

int delete_hungry_forward(void)
{
    gunichar c;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    end = start;
    
    c = gtk_text_iter_get_char(&end);
    
    if (c != ' ' && c != '\t') return 0;
    
    while (1) {
        if (!gtk_text_iter_forward_char(&end)) break;
        
        c = gtk_text_iter_get_char(&end);
        
        if (c != ' ' && c != '\t') break;
    }
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    return 1;
}

gboolean key_press(GtkWidget *widget, GdkEventKey *event)
{
    if (event->keyval == GDK_Tab) {
        if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(buffer))) {
            set_indent_all();
        } else {
            set_indent();
        }
        
        return TRUE;
    } else if (event->keyval == GDK_BackSpace && delete_flag == 1) {
        if (delete_hungry_backward()) return TRUE;
    } else if (event->keyval == GDK_Delete && delete_flag == 1) {
        if (delete_hungry_forward()) return TRUE;
    }
    
    return FALSE;
}

gboolean key_release(GtkWidget *widget, GdkEventKey *event)
{
    if (event->keyval == GDK_braceright && (event->state & GDK_SHIFT_MASK)) {
        set_indent();
    } else if (event->keyval == GDK_Return && auto_flag == 1) {
        set_indent();
    }
    
    return FALSE;
}

const gchar *menu_info =
"<ui>"
"  <menubar name='Menubar'>"
"    <menu name='File' action='File'>"
"      <menuitem name='New' action='New'/>"
"      <menuitem name='Open' action='Open'/>"
"      <separator/>"
"      <menuitem name='Save' action='Save'/>"
"      <menuitem name='Another' action='Another'/>"
"      <separator/>"
"      <menuitem name='Print' action='Print'/>"
"      <separator/>"
"      <menuitem name='Quit' action='Quit'/>"
"    </menu>"
"    <menu name='Edit' action='Edit'>"
"      <menuitem name='Undo' action='Undo'/>"
"      <menuitem name='Redo' action='Redo'/>"
"      <separator/>"
"      <menuitem name='Cut' action='Cut'/>"
"      <menuitem name='Copy' action='Copy'/>"
"      <menuitem name='Paste' action='Paste'/>"
"      <menuitem name='Delete' action='Delete'/>"
"      <menuitem name='Kill' action='Kill'/>"
"      <separator/>"
"      <menuitem name='Select' action='Select'/>"
"    </menu>"
"    <menu name='Search' action='Search'>"
"      <menuitem name='Find' action='Find'/>"
"      <menuitem name='Replace' action='Replace'/>"
"    </menu>"
"    <menu name='Move' action='Move'>"
"      <menuitem name='Jump' action='Jump'/>"
"      <menuitem name='Up' action='Up'/>"
"      <menuitem name='Down' action='Down'/>"
"      <menuitem name='Left' action='Left'/>"
"      <menuitem name='Right' action='Right'/>"
"    </menu>"
"    <menu name='Option' action='Option'>"
"      <menuitem name='Font' action='Font'/>"
"      <menuitem name='Tab' action='Tab'/>"
"      <separator/>"
"      <menuitem name='Wrap' action='Wrap'/>"
"      <menuitem name='Line' action='Line'/>"
"      <menuitem name='Indent' action='Indent'/>"
"      <menuitem name='Highlight' action='Highlight'/>"
"      <menuitem name='Space' action='Space'/>"
"      <menuitem name='Hungry' action='Hungry'/>"
"    </menu>"
"    <menu name='About' action='About'>"
"      <menuitem name='Help' action='Help'/>"
"      <menuitem name='Version' action='Version'/>"
"    </menu>"
"  </menubar>"
"  <toolbar name='Toolbar'>"
"      <toolitem name='New' action='New'/>"
"      <toolitem name='Open' action='Open'/>"
"      <toolitem name='Save' action='Save'/>"
"      <toolitem name='Another' action='Another'/>"
"      <toolitem name='Print' action='Print'/>"
"      <toolitem name='Undo' action='Undo'/>"
"      <toolitem name='Redo' action='Redo'/>"
"      <toolitem name='Cut' action='Cut'/>"
"      <toolitem name='Copy' action='Copy'/>"
"      <toolitem name='Paste' action='Paste'/>"
"      <toolitem name='Find' action='Find'/>"
"      <toolitem name='Replace' action='Replace'/>"
"  </toolbar>"
"</ui>";

GtkActionEntry entries[] =
{{"File", NULL, "ファイル(_F)"}, {"New", GTK_STOCK_NEW, "新規作成(_N)", "<control>N", "ファイルを新規作成する", G_CALLBACK(new_file)}, {"Open", GTK_STOCK_OPEN, "開く(_O)", "<control>O", "ファイルを開く", G_CALLBACK(open_file)}, {"Save", GTK_STOCK_SAVE, "上書き保存(_S)", "<control>S", "ファイルを上書き保存する", G_CALLBACK(save_file)}, {"Another", GTK_STOCK_SAVE_AS, "名前をつけて保存(_A)", "<shift><control>S", "ファイルを名前をつけて保存する", G_CALLBACK(save_file_another)}, {"Print", GTK_STOCK_PRINT, "印刷(_P)", "<control>P", "ファイルを印刷する", G_CALLBACK(print_file)}, {"Quit", GTK_STOCK_QUIT, "ceditの終了(_Q)", "<control>Q", NULL, G_CALLBACK(quit)}, {"Edit", NULL, "編集(_E)"}, {"Undo", GTK_STOCK_UNDO, "元に戻す(_U)", "<control>Z", "元に戻す", G_CALLBACK(undo_action)}, {"Redo", GTK_STOCK_REDO, "やり直し(_R)", "<control>Y", "やり直し", G_CALLBACK(redo_action)}, {"Cut", GTK_STOCK_CUT, "切り取り(_X)", "<control>X", "切り取り", G_CALLBACK(cut_text)}, {"Copy", GTK_STOCK_COPY, "コピー(_C)", "<control>C", "コピー", G_CALLBACK(copy_text)}, {"Paste", GTK_STOCK_PASTE, "貼り付け(_V)", "<control>V", "貼り付け", G_CALLBACK(paste_text)}, {"Delete", GTK_STOCK_DELETE, "削除(_D)", "<control>D", NULL, G_CALLBACK(delete_text)}, {"Kill", GTK_STOCK_CANCEL, "一行削除(_K)", "<control>K", NULL, G_CALLBACK(kill_line)}, {"Select", GTK_STOCK_SELECT_ALL, "全て選択(_A)", "<control>A", NULL, G_CALLBACK(select_all)}, {"Search", NULL, "検索(_S)"}, {"Find", GTK_STOCK_FIND, "検索(_F)", "<control>F", "文字列を検索する", G_CALLBACK(search_text)}, {"Replace", GTK_STOCK_FIND_AND_REPLACE, "置換(_R)", "<control>H", "文字列を置換する", G_CALLBACK(replace_text)}, {"Move", NULL, "移動(_M)"}, {"Jump", GTK_STOCK_JUMP_TO, "指定行に移動(_J)", "<control>G", NULL, G_CALLBACK(jump_line)}, {"Up", GTK_STOCK_GOTO_TOP, "ファイルの先頭に移動(_U)", "<control>Up", NULL, G_CALLBACK(jump_up)}, {"Down", GTK_STOCK_GOTO_BOTTOM, "ファイルの末尾に移動(_D)", "<control>Down", NULL, G_CALLBACK(jump_down)}, {"Left", GTK_STOCK_GOTO_FIRST, "行頭に移動(_L)", "<control>Left", NULL, G_CALLBACK(jump_left)}, {"Right", GTK_STOCK_GOTO_LAST, "行末に移動(_R)", "<control>Right", NULL, G_CALLBACK(jump_right)}, {"Option", NULL, "オプション(_O)"}, {"Font", GTK_STOCK_SELECT_FONT, "フォント(_F)", NULL, NULL, G_CALLBACK(set_font)}, {"Tab", GTK_STOCK_INDENT, "タブ幅の変更(_T)", NULL, NULL, G_CALLBACK(set_tab_width)}, {"About", NULL, "ヘルプ(_H)"}, {"Help", GTK_STOCK_HELP, "ヘルプ(_H)", "F1", NULL, G_CALLBACK(help)}, {"Version", GTK_STOCK_ABOUT, "ヴァージョン情報(_V)", NULL, NULL, G_CALLBACK(version)}};

GtkToggleActionEntry toggle_entries[] =
{{"Wrap", NULL, "右端で折り返す", NULL, NULL, G_CALLBACK(set_wrap_line), TRUE}, {"Line", NULL, "行番号を表示する", NULL, NULL, G_CALLBACK(set_display_line_number), TRUE}, {"Indent", NULL, "オートインデント", NULL, NULL, G_CALLBACK(set_auto_indent), TRUE}, {"Highlight", NULL, "カーソル行をハイライト", NULL, NULL, G_CALLBACK(set_highlight_line), TRUE}, {"Space", NULL, "タブの代わりにスペースを挿入", NULL, NULL, G_CALLBACK(set_insert_space), TRUE}, {"Hungry", NULL, "空白をまとめて消去", NULL, NULL, G_CALLBACK(set_delete_hungry), TRUE}};

void init_bar(GtkWidget *main_vbox)
{
    GtkWidget *menubar;
    GtkWidget *toolbar;
    GtkUIManager *ui;
    
    ui = gtk_ui_manager_new();
    actions = gtk_action_group_new("menu");
    gtk_action_group_add_actions(actions, entries, sizeof(entries) / sizeof(entries[0]), main_window);
    gtk_action_group_add_toggle_actions(actions, toggle_entries, sizeof(toggle_entries) / sizeof(toggle_entries[0]), main_window);
    gtk_ui_manager_insert_action_group(ui, actions, 0);
    gtk_ui_manager_add_ui_from_string(ui, menu_info, -1, NULL);
    gtk_window_add_accel_group(GTK_WINDOW(main_window), gtk_ui_manager_get_accel_group(ui));
    menubar = gtk_ui_manager_get_widget(ui, "/Menubar");
    toolbar = gtk_ui_manager_get_widget(ui, "/Toolbar");
    
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_LARGE_TOOLBAR);
    
    gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), toolbar, FALSE, FALSE, 0);
}

void init_editor(GtkWidget *main_vbox)
{
    GtkWidget *notebook;
    GtkWidget *scroll_window;
    
    notebook = gtk_notebook_new();
    scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    buffer = gtk_source_buffer_new(NULL);
    view = gtk_source_view_new_with_buffer(buffer);
    name_label = gtk_label_new("編集中のファイル");
    gtk_container_add(GTK_CONTAINER(scroll_window), view);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_window, name_label);
    
    gtk_source_buffer_set_highlight_matching_brackets(buffer, FALSE);
    gtk_widget_modify_font(view, pango_font_description_from_string("Inconsolata 11"));
    gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view), tab_width);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_CHAR);
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(view), TRUE);
    
    set_style();
    
    gtk_box_pack_start(GTK_BOX(main_vbox), notebook, TRUE, TRUE, 0);
}

void init_all(void)
{
    GtkWidget *main_vbox;
    
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "cedit");
    strcpy(buf, getenv("HOME"));
    strcat(buf, "/cedit/icon/icon.png");
    gtk_window_set_icon(GTK_WINDOW(main_window), gdk_pixbuf_new_from_file(buf, NULL));
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 600, 700);
    
    main_vbox = gtk_vbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
    
    init_bar(main_vbox);
    init_editor(main_vbox);
    
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "New"), FALSE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Save"), FALSE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Undo"), FALSE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Redo"), FALSE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Cut"), FALSE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Copy"), FALSE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Delete"), FALSE);
    
    text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
    cursor_id = g_signal_connect(G_OBJECT(buffer), "notify::cursor-position", G_CALLBACK(change_cursor), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::can-undo", G_CALLBACK(change_undo), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::can-redo", G_CALLBACK(change_redo), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::has-selection", G_CALLBACK(change_selection), NULL);
    g_signal_connect(G_OBJECT(main_window), "key-press-event", G_CALLBACK(key_press), NULL);
    g_signal_connect(G_OBJECT(main_window), "key-release-event", G_CALLBACK(key_release), NULL);
    g_signal_connect(G_OBJECT(main_window), "delete-event", G_CALLBACK(quit), NULL);
}

void start_file(char *name)
{
    set_file_name(name);
    
    read_file();
    
    gtk_label_set_text(GTK_LABEL(name_label), get_file_name());
    
    set_language();
    
    change_flag = 0;
    
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "New"), TRUE);
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, "Save"), FALSE);
    
    if (!g_signal_handler_is_connected(G_OBJECT(buffer), text_id)) {
        text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    
    init_all();
    
    if (argc != 1) start_file(argv[1]);
    
    gtk_widget_show_all(main_window);
    
    gtk_main();
    
    return 0;
}
