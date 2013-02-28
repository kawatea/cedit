#include "util.h"
#include "file.h"
#include "style.h"
#include "action.h"
#include "menu_file.h"
#include "menu_edit.h"
#include "menu_search.h"
#include "menu_move.h"
#include "menu_option.h"
#include "menu_help.h"
#include "tag.h"
#include "highlight_bracket.h"
#include "indent.h"
#include "key.h"
#include "ui.h"
#include "init.h"

void init_view(GtkWidget *box)
{
    GtkWidget *notebook;
    
    notebook = gtk_notebook_new();
    scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    buffer = gtk_source_buffer_new(NULL);
    view = gtk_source_view_new_with_buffer(buffer);
    set_file_name("");
    name_label = gtk_label_new("編集中のファイル");
    gtk_container_add(GTK_CONTAINER(scroll_window), view);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll_window, name_label);
    
    tab_width = 4;
    auto_flag = 1;
    delete_flag = 1;    
    
    gtk_source_buffer_set_highlight_matching_brackets(buffer, FALSE);
    gtk_widget_modify_font(view, pango_font_description_from_string("Inconsolata 11"));
    gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view), tab_width);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_CHAR);
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(view), TRUE);
    gtk_widget_set_has_tooltip(view, TRUE);
    
    set_style();
    
    gtk_box_pack_start(GTK_BOX(box), notebook, TRUE, TRUE, 0);
}

void init_editor(void)
{
    char s[1000];
    GtkWidget *main_vbox;
    
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "cedit");
    strcpy(s, getenv("CEDIT"));
    strcat(s, "/icon/icon.png");
    gtk_window_set_icon(GTK_WINDOW(main_window), gdk_pixbuf_new_from_file(s, NULL));
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 600, 700);
    
    main_vbox = gtk_vbox_new(FALSE, 3);
    gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
    
    init_ui(main_vbox);
    init_view(main_vbox);
    
    init_tag();
    
    set_action("New", FALSE);
    set_action("Save", FALSE);
    set_action("Undo", FALSE);
    set_action("Redo", FALSE);
    set_action("Cut", FALSE);
    set_action("Copy", FALSE);
    set_action("Delete", FALSE);
    
    text_id = g_signal_connect(G_OBJECT(buffer), "changed", G_CALLBACK(change_text), NULL);
    cursor_id = g_signal_connect(G_OBJECT(buffer), "notify::cursor-position", G_CALLBACK(change_cursor), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::can-undo", G_CALLBACK(change_undo), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::can-redo", G_CALLBACK(change_redo), NULL);
    g_signal_connect(G_OBJECT(buffer), "notify::has-selection", G_CALLBACK(change_selection), NULL);
    g_signal_connect(G_OBJECT(view), "query-tooltip", G_CALLBACK(popup), NULL);
    g_signal_connect(G_OBJECT(main_window), "key-press-event", G_CALLBACK(key_press), NULL);
    g_signal_connect(G_OBJECT(main_window), "key-release-event", G_CALLBACK(key_release), NULL);    
    g_signal_connect(G_OBJECT(main_window), "delete-event", G_CALLBACK(quit), NULL);
}
