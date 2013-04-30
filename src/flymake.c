#include "util.h"
#include "file.h"
#include "style.h"
#include "tag.h"
#include "flymake.h"

#ifdef _WIN32
#include <windows.h>
#endif

int get_message(void)
{
    #ifdef _WIN32
    HANDLE read, write;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD len = 0;
    
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = 0;
    sa.bInheritHandle = TRUE;
    
    if (!CreatePipe(&read, &write, &sa, 0)) return 0;
    
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdError = write;
    
    do {
        if (!CreateProcess(NULL, buf, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) break;
        
        WaitForInputIdle(pi.hProcess, INFINITE);
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        
        if (!PeekNamedPipe(read, NULL, 0, NULL, &len, NULL)) break;
        
        if (len > 0) ReadFile(read, buf, 100000, &len, NULL);
    } while (0);
    
    CloseHandle(read);
    CloseHandle(write);
    
    buf[len] = '\0';
    
    return len;
    #else
    int len;
    
    strcat(buf, " 2>&1");
    
    if ((fp = popen(buf, "r")) == NULL) return 0;
    
    len = fread(buf, sizeof(char), 100000, fp);
    
    pclose(fp);
    
    buf[len] = '\0';
    
    return len;
    #endif
}

void set_flymake(void)
{
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
    
    strcpy(buf, getenv("CEDIT"));
    strcat(buf, "/flymake/c_flymake");
    
    if ((fp = fopen(buf, "r")) != NULL) {
        fgets(buf, 100000, fp);
        
        gtk_entry_set_text(GTK_ENTRY(c_entry), buf);
        
        fclose(fp);
    }
    
    strcpy(buf, getenv("CEDIT"));
    strcat(buf, "/flymake/cpp_flymake");
    
    if ((fp = fopen(buf, "r")) != NULL) {
        fgets(buf, 100000, fp);
        
        gtk_entry_set_text(GTK_ENTRY(cpp_entry), buf);
        
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
        strcpy(buf, getenv("CEDIT"));
        strcat(buf, "/flymake/c_flymake");
        
        fp = fopen(buf, "w");
        
        fprintf(fp, "%s", gtk_entry_get_text(GTK_ENTRY(c_entry)));
        
        fclose(fp);

        strcpy(buf, getenv("CEDIT"));
        strcat(buf, "/flymake/cpp_flymake");
        
        fp = fopen(buf, "w");
        
        fprintf(fp, "%s", gtk_entry_get_text(GTK_ENTRY(cpp_entry)));
        
        fclose(fp);
    }
    
    gtk_widget_destroy(set_dialog);
}

void flymake(void)
{
    delete_tag(1);
    
    strcpy(buf, get_language());
    
    if (strcmp(buf, "C") == 0 || strcmp(buf, "C++") == 0) {
        if (strcmp(buf, "C") == 0) {
            strcpy(buf, getenv("CEDIT"));
            strcat(buf, "/flymake/c_flymake");
        } else {
            strcpy(buf, getenv("CEDIT"));
            strcat(buf, "/flymake/cpp_flymake");
        }
        
        if ((fp = fopen(buf, "r")) != NULL) {
            int len, i, j;
            char path[1000];
            GtkTextIter start, end;
            
            fgets(buf, 100000, fp);
            
            fclose(fp);
            
            buf[strlen(buf) - 1] = ' ';
            strcat(buf, get_file_name_full());
            
            putenv("LANG=C");
            
            len = get_message();
            
            strcpy(path, get_file_name_full());
            
            for (i = 0; i < len; i++) {
                for (j = i; j < len; j++) {
                    if (buf[j] == '\r') {
                        buf[j] = buf[j + 1] = '\0';
                        
                        break;
                    } else if (buf[j] == '\0') {
                        break;
                    }
                }
                
                if (strncmp(path, buf + i, strlen(path)) == 0 && buf[i + strlen(path) + 1] != ' ') {
                    int line = 0;
                    
                    for (j = strlen(path) + 1; ; j++) {
                        if (buf[i + j] >= '0' && buf[i + j] <= '9') {
                            line *= 10;
                            line += buf[i + j] - '0';
                        } else {
                            break;
                        }
                    }
                    
                    while (buf[i + j++] != ' ') ;
                    
                    line--;
                    
                    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
                    end = start;
                    gtk_text_iter_forward_line(&end);
                    
                    while (gtk_text_iter_get_char(&start) == ' ' || gtk_text_iter_get_char(&start) == '\t') gtk_text_iter_forward_char(&start);
                    
                    gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), add_error_tag(&buf[i + j]), &start, &end);
                }
                
                while (buf[i++] != '\0') ;
            }
        }
    }
}
