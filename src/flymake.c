#include "util.h"
#include "tag.h"
#include "flymake.h"

#ifdef _WIN32
#include <windows.h>
#endif

//flymakeのコマンドを設定する
void set_flymake(void)
{
    GtkWidget *set_dialog = gtk_dialog_new_with_buttons("flymakeの設定", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *c_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *c_label = gtk_label_new("Cのコマンド      : ");
    GtkWidget *c_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(c_entry), c_flymake);
    GtkWidget *cpp_hbox = gtk_hbox_new(FALSE, 2);
    GtkWidget *cpp_label = gtk_label_new("C++のコマンド : ");
    GtkWidget *cpp_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(cpp_entry), cpp_flymake);
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
        strcpy(c_flymake, gtk_entry_get_text(GTK_ENTRY(c_entry)));
        strcpy(cpp_flymake, gtk_entry_get_text(GTK_ENTRY(cpp_entry)));
    }
    
    gtk_widget_destroy(set_dialog);
}

//バックグラウンドで文法をチェックする
int get_message(char *command)
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
        if (!CreateProcess(NULL, command, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) break;
        
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
    
    strcat(command, " 2>&1");
    
    FILE *fp = popen(command, "r");
    
    if (fp == NULL) return 0;
    
    len = fread(buf, sizeof(char), 100000, fp);
    
    pclose(fp);
    
    buf[len] = '\0';
    
    return len;
    #endif
}

//CとC++の文法をチェックする
void flymake(void)
{
    char command[1000];
    
    delete_tag(1);
    
    if (strcmp(get_language(), "C") == 0) {
        strcpy(command, c_flymake);
    } else if (strcmp(get_language(), "C++") == 0) {
        strcpy(command, cpp_flymake);
    } else {
        return;
    }
    
    strcat(command, " ");
    strcat(command, get_file_name_full());
    
    putenv("LANG=C");
    
    int len = get_message(command), i, j;
    
    for (i = 0; i < len; ) {
        int next = len;
        
        for (j = i; j < len; j++) {
            if (buf[j] == '\r') {
                buf[j] = buf[j + 1] = '\0';
                next = j + 2;
                
                break;
            } else if (buf[j] == '\n' || buf[j] == '\0') {
                buf[j] = '\0';
                next = j + 1;
                
                break;
            }
        }
        
        if (strncmp(get_file_name_full(), buf + i, strlen(get_file_name_full())) == 0 && buf[i + strlen(get_file_name_full()) + 1] != ' ') {
            int line = 0;
            GtkTextIter start, end;
            
            for (j = strlen(get_file_name_full()) + 1; ; j++) {
                if (buf[i + j] >= '0' && buf[i + j] <= '9') {
                    line *= 10;
                    line += buf[i + j] - '0';
                } else {
                    break;
                }
            }
            
            while (buf[i + j++] != ' ') ;
            
            gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line - 1);
            end = start;
            gtk_text_iter_forward_line(&end);
            
            while (gtk_text_iter_get_char(&start) == ' ' || gtk_text_iter_get_char(&start) == '\t') gtk_text_iter_forward_char(&start);
            
            gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), add_error_tag(&buf[i + j]), &start, &end);
        }
        
        i = next;
    }
}
