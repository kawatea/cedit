#include "util.h"
#include "language.h"
#include "action.h"
#include "file.h"

char file_name[1000];

int empty_file_name(void)
{
    return file_name[0] == '\0';
}

const char *get_file_name(void)
{
    int i;
    
    for (i = strlen(file_name); i >= 0; i--) {
        if (file_name[i] == '/' || file_name[i] == '\\') break;
    }
    
    i++;
    
    return &file_name[i];
}

const char *get_file_name_full(void)
{
    return file_name;
}

void set_file_name(char *name)
{
    strcpy(file_name, name);
}

void read_file(void)
{
    if ((fp = fopen(get_file_name_full(), "r")) != NULL) {
        int num = 0;
        char buf[10010];
        GtkTextIter start, end;
        
        gtk_source_buffer_begin_not_undoable_action(buffer);
        
        set_start_end_iter(&start, &end);
        
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
                gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, buf, num);

                num = 0;
            }
        }
        
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, buf, num);
        
        fclose(fp);
        
        gtk_source_buffer_end_not_undoable_action(buffer);
    }
}

void write_file(void)
{
    GtkTextIter start, end;
    
    fp = fopen(get_file_name_full(), "w");
    
    set_start_end_iter(&start, &end);
    
    fprintf(fp, "%s", gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
    
    fclose(fp);
}

void delete_file(void)
{
    GtkTextIter start, end;
    
    set_file_name("");
    
    gtk_source_buffer_begin_not_undoable_action(buffer);
    
    set_start_end_iter(&start, &end);
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    gtk_source_buffer_end_not_undoable_action(buffer);
}

void start_file(char *name)
{
    set_file_name(name);
    
    read_file();
    
    gtk_label_set_text(GTK_LABEL(name_label), get_file_name());
    
    set_language();
    
    change_flag = kill_flag = 0;
    
    set_action("New", TRUE);
    set_action("Save", FALSE);
    
    change_text_connect();
}
