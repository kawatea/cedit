#include "util.h"
#include "callback.h"
#include "file.h"
#include "menu_file.h"
#include "menu_edit.h"
#include "menu_search.h"
#include "menu_library.h"
#include "tag.h"
#include "highlight_bracket.h"
#include "key.h"
#include "ui.h"
#include "init.h"

void init_editor(GtkWidget *box)
{
    //エディタ部分の外観の設定
    GtkWidget *notebook = gtk_notebook_new();
    scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    buffer = gtk_source_buffer_new(NULL);
    view = gtk_source_view_new_with_buffer(buffer);
    set_file_name("");
    name_label = gtk_label_new("編集中のファイル");
    gtk_container_add(GTK_CONTAINER(scroll_window), view);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_window, name_label);
    gtk_widget_set_has_tooltip(view, TRUE);
    gtk_widget_grab_focus(view);
    set_style();
    gtk_box_pack_start(GTK_BOX(box), notebook, TRUE, TRUE, 0);
    
    //エディタ部分の動作の初期設定
    gtk_source_buffer_set_highlight_matching_brackets(buffer, FALSE);
    gtk_widget_modify_font(view, pango_font_description_from_string(font_name));
    gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view), state & width_mask);
    if (state & wrap_mask) {
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_CHAR);
    } else {
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_NONE);
    }
    if (state & line_mask) {
        gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
    } else {
        gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), FALSE);
    }
    if (state & highlight_mask) {
        gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), TRUE);
    } else {
        gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), FALSE);
    }
    if (state & space_mask) {
        gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(view), TRUE);
    } else {
        gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(view), FALSE);
    }
}

void set_signal(void)
{
    text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
    cursor_id = g_signal_connect(G_OBJECT(buffer), "notify::cursor-position", G_CALLBACK(change_cursor), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::can-undo", G_CALLBACK(change_undo), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::can-redo", G_CALLBACK(change_redo), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::has-selection", G_CALLBACK(change_selection), NULL);
    g_signal_connect(G_OBJECT(view), "query-tooltip", G_CALLBACK(popup), NULL);
    g_signal_connect(G_OBJECT(view), "button_press_event", G_CALLBACK(right_click), NULL);
    g_signal_connect(G_OBJECT(view), "drag-data-received", G_CALLBACK(drop_receive), NULL);
    g_signal_connect(G_OBJECT(main_window), "drag-data-received", G_CALLBACK(drop_receive), NULL);
    g_signal_connect(G_OBJECT(main_window), "configure-event", G_CALLBACK(change_window), NULL);
    g_signal_connect(G_OBJECT(main_window), "key-press-event", G_CALLBACK(key_press), NULL);
    g_signal_connect(G_OBJECT(main_window), "key-release-event", G_CALLBACK(key_release), NULL);    
    g_signal_connect(G_OBJECT(main_window), "delete-event", G_CALLBACK(quit), NULL);
}

void init_window(void)
{
    load_setting();
    
    //メインウィンドウの外観の設定
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "cedit");
    gtk_window_set_icon(GTK_WINDOW(main_window), get_icon("icon.png"));
    gtk_window_move(GTK_WINDOW(main_window), window_x, window_y);
    gtk_window_set_default_size(GTK_WINDOW(main_window), window_width, window_height);
    
    //ウィンドウの内部の設定
    GtkWidget *main_vbox = gtk_vbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
    init_bar(main_vbox);
    init_editor(main_vbox);
    
    //全体の動作の設定
    init_tag();
    init_library();
    set_action("New", FALSE);
    set_action("Save", FALSE);
    set_action("Undo", FALSE);
    set_action("Redo", FALSE);
    set_action("Cut", FALSE);
    set_action("Copy", FALSE);
    set_action("Delete", FALSE);
    static GtkTargetEntry target[] = {{"text/uri-list",  0, 10}};
    gtk_drag_dest_set(view, GTK_DEST_DEFAULT_ALL, target, 1, GDK_ACTION_COPY);
    gtk_drag_dest_set(main_window, GTK_DEST_DEFAULT_ALL, target, 1, GDK_ACTION_COPY);
    set_signal();
}
