#include <dirent.h>
#include "util.h"
#include "menu_search.h"
#include "menu_library.h"

GtkListStore *library_list;
GtkEntryCompletion *completion;
GtkTreeIter iter;

void change_library_entry(GtkWidget *entry, GObject *object, gpointer button)
{
    char s[1000];
    
    strcpy(s, gtk_entry_get_text(GTK_ENTRY(entry)));
    
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(library_list), &iter)) {
        while (1) {
            char *name;
            
            gtk_tree_model_get(GTK_TREE_MODEL(library_list), &iter, 0, &name, -1);
            
            if (strcmp(s, name) == 0) {
                gtk_widget_set_sensitive((GtkWidget *)button, TRUE);
                
                return;
            }
            
            if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(library_list), &iter)) break;
        }
    }
    
    gtk_widget_set_sensitive((GtkWidget *)button, FALSE);
}

void select_library(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer entry)
{
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(library_list), &iter, path)) {
        char *name;
        
        gtk_tree_model_get(GTK_TREE_MODEL(library_list), &iter, 0, &name, -1);
        
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry), name);
    }
}

void init_library(void)
{
    char s[1000];
    DIR *directory;
    struct dirent *now;
    
    library_list = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(library_list), 0, GTK_SORT_ASCENDING);
    
    strcpy(s, getenv("CEDIT"));
    strcat(s, "/library");
    
    if ((directory = opendir(s)) != NULL) {
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
    GtkWidget *call_dialog;
    GtkWidget *call_window;
    GtkWidget *call_view;
    GtkWidget *call_hbox;
    GtkWidget *call_label;
    GtkWidget *call_entry;
    GtkWidget *call_button;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    call_dialog = gtk_dialog_new_with_buttons("ライブラリの呼び出し", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    call_button = gtk_dialog_add_button(GTK_DIALOG(call_dialog), "挿入", GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(call_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    call_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(call_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(call_window, 100, 300);
    call_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(library_list));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Name");
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(call_view), column);
    call_hbox = gtk_hbox_new(FALSE, 2);
    call_label = gtk_label_new("挿入するライブラリ : ");
    call_entry = gtk_entry_new();
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
    
    while (gtk_dialog_run(GTK_DIALOG(call_dialog)) == GTK_RESPONSE_OK) {
        char s[1000];
        
        strcpy(s, getenv("CEDIT"));
        strcat(s, "/library/");
        strcat(s, gtk_entry_get_text(GTK_ENTRY(call_entry)));
        
        if ((fp = fopen(s, "r")) != NULL) {
            int num = 0;
            char buf[10010];
            GtkTextIter cursor;
            
            gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &cursor, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
            
            while (1) {
                char c = fgetc(fp);
                
                if (c == EOF) break;
                
                if (((c >> 7) & 1) == 0) {
                    buf[num++] = c;
                } else if (((c >> 5) & 1) == 0) {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                } else {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                    buf[num++] = fgetc(fp);
                }
                
                if (num > 10000) {
                    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &cursor, buf, num);
                    
                    num = 0;
                }
            }
            
            gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &cursor, buf, num);
            
            fclose(fp);
        }
    }
    
    gtk_widget_destroy(call_dialog);
}

void add_library(void)
{
    GtkWidget *add_dialog;
    GtkWidget *add_hbox;
    GtkWidget *add_label;
    GtkWidget *add_entry;
    GtkWidget *add_button;
    
    add_dialog = gtk_dialog_new_with_buttons("ライブラリの追加", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    add_button = gtk_dialog_add_button(GTK_DIALOG(add_dialog), "追加", GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(add_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    add_hbox = gtk_hbox_new(FALSE, 2);
    add_label = gtk_label_new("追加するライブラリ : ");
    add_entry = gtk_entry_new();
    
    gtk_box_pack_start(GTK_BOX(add_hbox), add_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_hbox), add_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(add_dialog)->vbox), add_hbox, FALSE, FALSE, 0);
    
    gtk_entry_set_activates_default(GTK_ENTRY(add_entry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(add_dialog), GTK_RESPONSE_OK);
    
    gtk_widget_set_sensitive(add_button, FALSE);
    
    g_signal_connect(G_OBJECT(add_entry), "notify::text", G_CALLBACK(change_find_entry), (gpointer)add_button);
    
    gtk_widget_show_all(add_dialog);
    
    while (gtk_dialog_run(GTK_DIALOG(add_dialog)) == GTK_RESPONSE_OK) {
        int flag = 0;
        char s[1000];
        GtkTextIter start, end;
        
        strcpy(s, gtk_entry_get_text(GTK_ENTRY(add_entry)));
        
        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(library_list), &iter)) {
            while (1) {
                char *name;
                
                gtk_tree_model_get(GTK_TREE_MODEL(library_list), &iter, 0, &name, -1);
                
                if (strcmp(s, name) == 0) {
                    flag = 1;
                    
                    break;
                }
                
                if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(library_list), &iter)) break;
            }
        }
        
        if (flag == 1) {
            GtkWidget *check_dialog;
            GtkWidget *check_label;
            
            check_dialog = gtk_dialog_new_with_buttons("確認", GTK_WINDOW(add_dialog), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_YES, GTK_RESPONSE_YES, GTK_STOCK_NO, GTK_RESPONSE_NO, NULL);
            check_label = gtk_label_new("既に存在する名前ですが上書きしますか？");
            
            gtk_box_pack_start(GTK_BOX(GTK_DIALOG(check_dialog)->vbox), check_label, FALSE, FALSE, 0);
            
            gtk_dialog_set_default_response(GTK_DIALOG(check_dialog), GTK_RESPONSE_YES);
            
            gtk_widget_show_all(check_dialog);
            
            if (gtk_dialog_run(GTK_DIALOG(check_dialog)) == GTK_RESPONSE_NO) flag = 2;
            
            gtk_widget_destroy(check_dialog);
            
            if (flag == 2) continue;
        }
        
        if (flag == 0) {
            gtk_list_store_append(library_list, &iter);
            gtk_list_store_set(library_list, &iter, 0, s, -1);
        }
        
        strcpy(s, getenv("CEDIT"));
        strcat(s, "/library/");
        strcat(s, gtk_entry_get_text(GTK_ENTRY(add_entry)));
        
        fp = fopen(s, "w");
        
        set_start_end_iter(&start, &end);
        
        fprintf(fp, "%s", gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
        
        fclose(fp);
        
        break;
    }
    
    gtk_widget_destroy(add_dialog);
}

void remove_library(void)
{
    GtkWidget *remove_dialog;
    GtkWidget *remove_window;
    GtkWidget *remove_view;
    GtkWidget *remove_hbox;
    GtkWidget *remove_label;
    GtkWidget *remove_entry;
    GtkWidget *remove_button;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    remove_dialog = gtk_dialog_new_with_buttons("ライブラリの削除", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
    remove_button = gtk_dialog_add_button(GTK_DIALOG(remove_dialog), "削除", GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(remove_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
    remove_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(remove_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(remove_window, 100, 300);
    remove_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(library_list));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Name");
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(remove_view), column);
    remove_hbox = gtk_hbox_new(FALSE, 2);
    remove_label = gtk_label_new("削除するライブラリ : ");
    remove_entry = gtk_entry_new();
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
    
    while (gtk_dialog_run(GTK_DIALOG(remove_dialog)) == GTK_RESPONSE_OK) {
        char s[1000];
        
        strcpy(s, gtk_entry_get_text(GTK_ENTRY(remove_entry)));
        
        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(library_list), &iter)) {
            while (1) {
                char *name;
                
                gtk_tree_model_get(GTK_TREE_MODEL(library_list), &iter, 0, &name, -1);
                
                if (strcmp(s, name) == 0) {
                    gtk_list_store_remove(library_list, &iter);
                    
                    break;
                }
                
                if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(library_list), &iter)) break;
            }
        }
        
        strcpy(s, getenv("CEDIT"));
        strcat(s, "/library/");
        strcat(s, gtk_entry_get_text(GTK_ENTRY(remove_entry)));
        
        remove(s);
    }
    
    gtk_widget_destroy(remove_dialog);
}
