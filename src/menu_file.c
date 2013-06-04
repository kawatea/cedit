#include "util.h"
#include "file.h"
#include "style.h"
#include "flymake.h"
#include "menu_file.h"

void new_file(void)
{
    if (change_flag == 1) save_check();
    
    if (change_flag == 1) return;
    
    delete_file();
    
    gtk_label_set_text(GTK_LABEL(name_label), "編集中のファイル");
    
    change_flag = kill_flag = 0;
    
    set_action("New", FALSE);
    set_action("Save", FALSE);
    
    change_text_connect();
}

void open_file(void)
{
    GtkWidget *open_dialog;
    
    if (change_flag == 1) save_check();
    
    if (change_flag == 1) return;
    
    open_dialog = gtk_file_chooser_dialog_new("開く", GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    
    gtk_dialog_set_default_response(GTK_DIALOG(open_dialog), GTK_RESPONSE_ACCEPT);
    
    if (gtk_dialog_run(GTK_DIALOG(open_dialog)) == GTK_RESPONSE_ACCEPT) {
        delete_file();
        
        start_file(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(open_dialog)));
    }
    
    gtk_widget_destroy(open_dialog);
}

void save_file(void)
{
    write_file();
    
    gtk_label_set_text(GTK_LABEL(name_label), get_file_name());
    
    change_flag = kill_flag = 0;
    
    set_action("New", TRUE);
    set_action("Save", FALSE);
    
    change_text_connect();
    
    flymake();
}

void save_file_another(void)
{
    GtkWidget *save_dialog;
    
    save_dialog = gtk_file_chooser_dialog_new("名前をつけて保存", GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    
    gtk_dialog_set_default_response(GTK_DIALOG(save_dialog), GTK_RESPONSE_ACCEPT);
    
    gtk_widget_show_all(save_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(save_dialog)) == GTK_RESPONSE_ACCEPT) {
        set_file_name(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(save_dialog)));
        
        set_language();
        
        save_file();
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

gboolean quit(void)
{
    if (change_flag == 1) save_check();
    
    if (change_flag == 1) return TRUE;
    
    save_setting();
    
    gtk_main_quit();
}

void receive(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data)
{
    gchar *name = g_filename_from_uri(*g_uri_list_extract_uris(data->data), NULL, NULL);
    
    if (change_flag == 1) save_check();
    
    if (change_flag == 1) return;
    
    delete_file();
    
    start_file(name);
}
