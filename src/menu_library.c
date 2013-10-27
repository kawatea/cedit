#include <dirent.h>
#include "util.h"
#include "menu_search.h"
#include "menu_library.h"

GtkListStore *library_list;
GtkEntryCompletion *completion;
GtkTreeIter iter;

//挿入や削除のボタンの状態を変える
void change_library_entry(GtkWidget *entry, GObject *object, gpointer button)
{
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(library_list), &iter)) {
        while (1) {
            char *name;
            
            gtk_tree_model_get(GTK_TREE_MODEL(library_list), &iter, 0, &name, -1);
            
            if (strcmp(gtk_entry_get_text(GTK_ENTRY(entry)), name) == 0) {
                gtk_widget_set_sensitive((GtkWidget *)button, TRUE);
                free(name);
                
                return;
            }
            
            free(name);
            
            if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(library_list), &iter)) break;
        }
    }
    
    gtk_widget_set_sensitive((GtkWidget *)button, FALSE);
}

//選択されたライブラリの名前を挿入する
void select_library(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer entry)
{
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(library_list), &iter, path)) {
        char *name;
        
        gtk_tree_model_get(GTK_TREE_MODEL(library_list), &iter, 0, &name, -1);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry), name);
        
        free(name);
    }
}

//ライブラリの一覧を読み込む
void init_library(void)
{
    library_list = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(library_list), 0, GTK_SORT_ASCENDING);
    
    char *path = get_path("/library");
    DIR *directory = opendir(path);
    
    free(path);
    
    if (directory != NULL) {
        struct dirent *now;
        
        while ((now = readdir(directory)) != NULL) {
            if (now->d_name[0] == '.') continue;
            
            gtk_list_store_append(library_list, &iter);
            gtk_list_store_set(library_list, &iter, 0, now->d_name, -1);
        }
        
        closedir(directory);
    }
    
    completion = gtk_entry_completion_new();
    gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(library_list));
    gtk_entry_completion_set_text_column(completion, 0);
}

void call_library(void)
{
    //呼び出しダイアログの設定
    GtkWidget *call_dialog = gtk_dialog_new_with_buttons("ライブラリの呼び出し", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    GtkWidget *call_button = gtk_dialog_add_button(GTK_DIALOG(call_dialog), "挿入", GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(call_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    GtkWidget *call_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(call_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(call_window, 100, 300);
    GtkWidget *call_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(library_list));
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Name");
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(call_view), column);
    GtkWidget *call_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *call_label = gtk_label_new("挿入するライブラリ : ");
    GtkWidget *call_entry = gtk_entry_new();
    gtk_entry_set_completion(GTK_ENTRY(call_entry), completion);
    gtk_container_add(GTK_CONTAINER(call_window), call_view);
    gtk_box_pack_start(GTK_BOX(call_hbox), call_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(call_hbox), call_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(call_dialog)->vbox), call_window, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(call_dialog)->vbox), call_hbox, FALSE, FALSE, 0);
    gtk_window_set_focus(GTK_WINDOW(call_dialog), call_entry);
    gtk_entry_set_activates_default(GTK_ENTRY(call_entry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(call_dialog), GTK_RESPONSE_OK);
    gtk_widget_set_sensitive(call_button, FALSE);
    g_signal_connect(call_view, "row-activated", G_CALLBACK(select_library), (gpointer)call_entry);
    g_signal_connect(G_OBJECT(call_entry), "notify::text", G_CALLBACK(change_library_entry), (gpointer)call_button);
    gtk_widget_show_all(call_dialog);
    
    //ライブラリの読み込み
    while (gtk_dialog_run(GTK_DIALOG(call_dialog)) == GTK_RESPONSE_OK) {
        int num = 0;
        GtkTextIter cursor;
        char *path = get_path("/library/");
        
        strcat(path, gtk_entry_get_text(GTK_ENTRY(call_entry)));
        
        FILE *fp = fopen(path, "r");
        
        gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &cursor, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
        
        while (1) {
            char c = fgetc(fp);
            
            if (c == EOF) break;
            
            if (((c >> 7) & 1) == 0) {
                buf[num++] = c;
            } else if (((c >> 5) & 1) == 0) {
                buf[num++] = c;
                buf[num++] = fgetc(fp);
            } else if (((c >> 4) & 1) == 0) {
                buf[num++] = c;
                buf[num++] = fgetc(fp);
                buf[num++] = fgetc(fp);
            } else {
                buf[num++] = c;
                buf[num++] = fgetc(fp);
                buf[num++] = fgetc(fp);
                buf[num++] = fgetc(fp);
            }
            
            if (num > 100000) {
                gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &cursor, buf, num);
                
                num = 0;
            }
        }
        
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &cursor, buf, num);
        
        fclose(fp);
        free(path);
    }
    
    gtk_widget_destroy(call_dialog);
}

void add_library(void)
{
    //追加ダイアログの設定
    GtkWidget *add_dialog = gtk_dialog_new_with_buttons("ライブラリの追加", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    GtkWidget *add_button = gtk_dialog_add_button(GTK_DIALOG(add_dialog), "追加", GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(add_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    GtkWidget *add_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *add_label = gtk_label_new("追加するライブラリ : ");
    GtkWidget *add_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(add_hbox), add_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_hbox), add_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(add_dialog)->vbox), add_hbox, FALSE, FALSE, 0);
    gtk_entry_set_activates_default(GTK_ENTRY(add_entry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(add_dialog), GTK_RESPONSE_OK);
    gtk_widget_set_sensitive(add_button, FALSE);
    g_signal_connect(G_OBJECT(add_entry), "notify::text", G_CALLBACK(change_find_entry), (gpointer)add_button);
    gtk_widget_show_all(add_dialog);
    
    //ライブラリの追加
    while (gtk_dialog_run(GTK_DIALOG(add_dialog)) == GTK_RESPONSE_OK) {
        int flag = 0;
        GtkTextIter start, end;
        
        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(library_list), &iter)) {
            while (1) {
                char *name;
                
                gtk_tree_model_get(GTK_TREE_MODEL(library_list), &iter, 0, &name, -1);
                
                if (strcmp(gtk_entry_get_text(GTK_ENTRY(add_entry)), name) == 0) {
                    flag = 1;
                    free(name);
                    
                    break;
                }
                
                free(name);
                
                if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(library_list), &iter)) break;
            }
        }
        
        if (flag == 1) {
            GtkWidget *check_dialog = gtk_dialog_new_with_buttons("確認", GTK_WINDOW(add_dialog), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_YES, GTK_RESPONSE_YES, GTK_STOCK_NO, GTK_RESPONSE_NO, NULL);
            GtkWidget *check_label = gtk_label_new("既に存在する名前ですが上書きしますか？");
            gtk_box_pack_start(GTK_BOX(GTK_DIALOG(check_dialog)->vbox), check_label, FALSE, FALSE, 0);
            gtk_dialog_set_default_response(GTK_DIALOG(check_dialog), GTK_RESPONSE_YES);
            gtk_widget_show_all(check_dialog);
            
            if (gtk_dialog_run(GTK_DIALOG(check_dialog)) == GTK_RESPONSE_NO) flag = 2;
            
            gtk_widget_destroy(check_dialog);
            
            if (flag == 2) continue;
        } else if (flag == 0) {
            gtk_list_store_append(library_list, &iter);
            gtk_list_store_set(library_list, &iter, 0, gtk_entry_get_text(GTK_ENTRY(add_entry)), -1);
        }
        
        char *path = get_path("/library/");
        strcat(path, gtk_entry_get_text(GTK_ENTRY(add_entry)));
        
        FILE *fp = fopen(path, "w");
        
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
        gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
        
        char *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE);
        fprintf(fp, "%s", text);
        
        fclose(fp);
        free(path);
        free(text);
        
        break;
    }
    
    gtk_widget_destroy(add_dialog);
}

void remove_library(void)
{
    //削除ダイアログの設定
    GtkWidget *remove_dialog = gtk_dialog_new_with_buttons("ライブラリの削除", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    GtkWidget *remove_button = gtk_dialog_add_button(GTK_DIALOG(remove_dialog), "削除", GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(remove_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    GtkWidget *remove_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(remove_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(remove_window, 100, 300);
    GtkWidget *remove_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(library_list));
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Name");
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(remove_view), column);
    GtkWidget *remove_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *remove_label = gtk_label_new("削除するライブラリ : ");
    GtkWidget *remove_entry = gtk_entry_new();
    gtk_entry_set_completion(GTK_ENTRY(remove_entry), completion);
    gtk_container_add(GTK_CONTAINER(remove_window), remove_view);
    gtk_box_pack_start(GTK_BOX(remove_hbox), remove_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(remove_hbox), remove_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(remove_dialog)->vbox), remove_window, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(remove_dialog)->vbox), remove_hbox, FALSE, FALSE, 0);
    gtk_window_set_focus(GTK_WINDOW(remove_dialog), remove_entry);
    gtk_entry_set_activates_default(GTK_ENTRY(remove_entry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(remove_dialog), GTK_RESPONSE_OK);
    gtk_widget_set_sensitive(remove_button, FALSE);
    g_signal_connect(G_OBJECT(remove_view), "row-activated", G_CALLBACK(select_library), (gpointer)remove_entry);
    g_signal_connect(G_OBJECT(remove_entry), "notify::text", G_CALLBACK(change_library_entry), (gpointer)remove_button);
    gtk_widget_show_all(remove_dialog);
    
    //ライブラリの削除
    while (gtk_dialog_run(GTK_DIALOG(remove_dialog)) == GTK_RESPONSE_OK) {
        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(library_list), &iter)) {
            while (1) {
                char *name;
                
                gtk_tree_model_get(GTK_TREE_MODEL(library_list), &iter, 0, &name, -1);
                
                if (strcmp(gtk_entry_get_text(GTK_ENTRY(remove_entry)), name) == 0) {
                    gtk_list_store_remove(library_list, &iter);
                    free(name);
                    
                    break;
                }
                
                free(name);
                
                if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(library_list), &iter)) break;
            }
        }
        
        char *path = get_path("/library/");
        strcat(path, gtk_entry_get_text(GTK_ENTRY(remove_entry)));
        
        remove(path);
        
        free(path);
    }
    
    gtk_widget_destroy(remove_dialog);
}
