#include "util.h"
#include "delete_hungry.h"

//前方向の空白をまとめて消去する
int delete_hungry_backward(void)
{
    gunichar c;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    end = start;
    
    if (!gtk_text_iter_backward_char(&start)) return 0;
    
    c = gtk_text_iter_get_char(&start);
    
    if (c != ' ' && c != '\t') return 0;
    
    while (1) {
        if (!gtk_text_iter_backward_char(&start)) break;
        
        c = gtk_text_iter_get_char(&start);
        
        if (c != ' ' && c != '\t') {
            c = gtk_text_iter_forward_char(&start);
            
            break;
        }
    }
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    return 1;
}

//後方向の空白をまとめて消去する
int delete_hungry_forward(void)
{
    gunichar c;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    end = start;
    c = gtk_text_iter_get_char(&end);
    
    if (c != ' ' && c != '\t') return 0;
    
    while (1) {
        if (!gtk_text_iter_forward_char(&end)) break;
        
        c = gtk_text_iter_get_char(&end);
        
        if (c != ' ' && c != '\t') break;
    }
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    return 1;
}
