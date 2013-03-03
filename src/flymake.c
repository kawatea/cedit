#include "util.h"
#include "file.h"
#include "style.h"
#include "tag.h"
#include "flymake.h"

void set_flymake(void)
{
    char s[1000];
    GtkWidget *set_dialog;
    GtkWidget *c_hbox;
    GtkWidget *c_label;
    GtkWidget *c_entry;
    GtkWidget *cpp_hbox;
    GtkWidget *cpp_label;
    GtkWidget *cpp_entry;
    
    set_dialog = gtk_dialog_new_with_buttons("flymakeの設定", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    c_hbox = gtk_hbox_new(FALSE, 2);
    c_label = gtk_label_new("Cのコマンド      : ");
    c_entry = gtk_entry_new();
    cpp_hbox = gtk_hbox_new(FALSE, 2);
    cpp_label = gtk_label_new("C++のコマンド : ");
    cpp_entry = gtk_entry_new();
    
    strcpy(s, getenv("CEDIT"));
    strcat(s, "/flymake/c_flymake");
    
    if ((fp = fopen(s, "r")) != NULL) {
        fgets(s, 1000, fp);
        
        gtk_entry_set_text(GTK_ENTRY(c_entry), s);
        
        fclose(fp);
    }
    
    strcpy(s, getenv("CEDIT"));
    strcat(s, "/flymake/cpp_flymake");
    
    if ((fp = fopen(s, "r")) != NULL) {
        fgets(s, 1000, fp);
        
        gtk_entry_set_text(GTK_ENTRY(cpp_entry), s);
        
        fclose(fp);
    }
    
    gtk_box_pack_start(GTK_BOX(c_hbox), c_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(c_hbox), c_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cpp_hbox), cpp_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cpp_hbox), cpp_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(set_dialog)->vbox), c_hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(set_dialog)->vbox), cpp_hbox, FALSE, FALSE, 0);
    
    gtk_entry_set_activates_default(GTK_ENTRY(c_entry), TRUE);
    gtk_entry_set_activates_default(GTK_ENTRY(cpp_entry), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(set_dialog), GTK_RESPONSE_OK);
    
    gtk_widget_show_all(set_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(set_dialog)) == GTK_RESPONSE_OK) {
        strcpy(s, getenv("CEDIT"));
        strcat(s, "/flymake/c_flymake");
        
        fp = fopen(s, "w");
        
        fprintf(fp, "%s", gtk_entry_get_text(GTK_ENTRY(c_entry)));
        
        fclose(fp);

        strcpy(s, getenv("CEDIT"));
        strcat(s, "/flymake/cpp_flymake");
        
        fp = fopen(s, "w");
        
        fprintf(fp, "%s", gtk_entry_get_text(GTK_ENTRY(cpp_entry)));
        
        fclose(fp);
    }
    
    gtk_widget_destroy(set_dialog);
}

void flymake(void)
{
    int i;
    char s[1000];
    char buf[10000];
    GtkTextIter start, end;
    
    delete_tag(1);
    
    strcpy(s, get_language());
    
    if (strcmp(s, "C") == 0 || strcmp(s, "C++") == 0) {
        if (strcmp(s, "C") == 0) {
            strcpy(s, getenv("CEDIT"));
            strcat(s, "/flymake/c_flymake");
        } else {
            strcpy(s, getenv("CEDIT"));
            strcat(s, "/flymake/cpp_flymake");
        }
        
        if ((fp = fopen(s, "r")) != NULL) {
            fgets(s, 1000, fp);
            
            fclose(fp);
            
            s[strlen(s) - 1] = ' ';
            strcat(s, get_file_name_full());
            strcat(s, " 2>&1");
            
            putenv("LANG=C");
            
            if ((fp = popen(s, "r")) != NULL) {
                strcpy(s, get_file_name_full());
                
                while (fgets(buf, 10000, fp) != NULL) {
                    if (strncmp(s, buf, strlen(s)) == 0 && buf[strlen(s) + 1] != ' ') {
                        int line = 0;
                        
                        for (i = strlen(s) + 1; ; i++) {
                            if (buf[i] >= '0' && buf[i] <= '9') {
                                line *= 10;
                                line += buf[i] - '0';
                            } else {
                                break;
                            }
                        }
                        
                        while (buf[i++] != ' ') ;
                        
                        line--;
                        
                        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
                        end = start;
                        gtk_text_iter_forward_line(&end);
                        
                        while (gtk_text_iter_get_char(&start) == ' ' || gtk_text_iter_get_char(&start) == '\t') gtk_text_iter_forward_char(&start);
                        
                        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), add_error_tag(&buf[i]), &start, &end);
                    }
                }
                
                pclose(fp);
            }
        }
    }
}
