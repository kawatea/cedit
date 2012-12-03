#include "util.h"
#include "file.h"
#include "language.h"
#include "tag.h"
#include "flymake.h"

void flymake(void)
{
    int i;
    char s[1000];
    char buf[10000];
    
    delete_tag(1);
    
    strcpy(s, get_language());
    
    if (strcmp(s, "C") == 0 || strcmp(s, "C++") == 0) {
        if (strcmp(s, "C") == 0) {
            strcpy(s, getenv("HOME"));
            strcat(s, "/cedit/flymake/c_flymake");
        } else {
            strcpy(s, getenv("HOME"));
            strcat(s, "/cedit/flymake/cpp_flymake");
        }
        
        if ((fp = fopen(s, "r")) != NULL) {
            fgets(s, 1000, fp);
            
            fclose(fp);
            
            s[strlen(s) - 1] = ' ';
            strcat(s, get_file_name_full());
            strcat(s, " 2>&1");
            
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
                        
                        line--;
                        
                        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
                        end = start;
                        gtk_text_iter_forward_line(&end);
                        
                        while (gtk_text_iter_get_char(&start) == ' ' || gtk_text_iter_get_char(&start) == '\t') gtk_text_iter_forward_char(&start);
                        
                        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), error_tag, &start, &end);
                    }
                }
                
                pclose(fp);
            }
        }
    }
}
