#include "util.h"
#include "tag.h"
#include "highlight_bracket.h"

void highlight_bracket_backward(GtkTextIter now)
{
    int num = 0, find_flag = 0;
    char s[1000];
    
    start = now;
    
    while (1) {
        gunichar c = gtk_text_iter_get_char(&start);
        
        if (c == '"' || c == '\'') {
            int count = 0, flag = 0;
            
            while (1) {
                if (!gtk_text_iter_backward_char(&start)) {
                    flag = 1;
                    
                    break;
                }
                
                if (gtk_text_iter_get_char(&start) == '\\') {
                    count++;
                } else {
                    break;
                }
            }
            
            if (flag == 1) break;
            
            if (count % 2 == 1) continue;
            
            while (1) {
                if (gtk_text_iter_get_char(&start) == c) {
                    count = 0;
                    
                    while (1) {
                        if (!gtk_text_iter_backward_char(&start)) break;
                        
                        if (gtk_text_iter_get_char(&start) == '\\') {
                            count++;
                        } else {
                            break;
                        }
                    }
                    
                    if (count % 2 == 0) break;
                    
                    continue;
                }
                
                if (!gtk_text_iter_backward_char(&start)) break;
            }
            
            continue;
        }
        
        if (c == '}' || c == ')' || c == ']') {
            s[num++] = c;
        } else if (c == '{') {
            if (s[num - 1] == '}') {
                num--;
            } else {
                break;
            }
        } else if (c == '(') {
            if (s[num - 1] == ')') {
                num--;
            } else {
                break;
            }
        } else if (c == '[') {
            if (s[num - 1] == ']') {
                num--;
            } else {
                break;
            }
        }
        
        if (num == 0) {
            find_flag = 1;
            
            break;
        }
        
        if (!gtk_text_iter_backward_char(&start)) break;
    }
    
    if (find_flag == 1) {
        end = start;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), match_tag, &start, &end);
        
        start = end = now;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), match_tag, &start, &end);
    } else {
        start = end = now;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), unmatch_tag, &start, &end);
    }
}

void highlight_bracket_forward(GtkTextIter now)
{
    int num = 0, find_flag = 0;
    char s[1000];
    
    start = now;
    
    while (!gtk_text_iter_is_end(&start)) {
        gunichar c = gtk_text_iter_get_char(&start);
        
        if (c == '\\') {
            if (!gtk_text_iter_forward_chars(&start, 2)) break;
            
            continue;
        }
        
        if (c == '"' || c == '\'') {
            while (1) {
                if (!gtk_text_iter_forward_char(&start)) break;
                
                while (gtk_text_iter_get_char(&start) == '\\') {
                    if (!gtk_text_iter_forward_chars(&start, 2)) break;
                }
                
                if (gtk_text_iter_get_char(&start) == c) break;
            }
            
            if (!gtk_text_iter_forward_char(&start)) break;
            
            continue;
        }
        
        if (c == '{' || c == '(' || c == '[') {
            s[num++] = c;
        } else if (c == '}') {
            if (s[num - 1] == '{') {
                num--;
            } else {
                break;
            }
        } else if (c == ')') {
            if (s[num - 1] == '(') {
                num--;
            } else {
                break;
            }
        } else if (c == ']') {
            if (s[num - 1] == '[') {
                num--;
            } else {
                break;
            }
        }
        
        if (num == 0) {
            find_flag = 1;
            
            break;
        }
        
        if (!gtk_text_iter_forward_char(&start)) break;
    }
    
    if (find_flag == 1) {
        end = start;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), match_tag, &start, &end);
        
        start = end = now;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), match_tag, &start, &end);
    } else {
        start = end = now;
        
        gtk_text_iter_forward_char(&end);
        
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(buffer), unmatch_tag, &start, &end);
    }
}

void highlight_bracket(void)
{
    gunichar c;
    
    delete_tag(0);
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &start, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    end = start;
    
    gtk_text_iter_backward_char(&start);
    
    c = gtk_text_iter_get_char(&start);
    
    if (c == '}' || c == ')' || c == ']') {
        highlight_bracket_backward(start);
        
        return;
    }
    
    c = gtk_text_iter_get_char(&end);
    
    if (c == '{' || c == '(' || c == '[') {
        highlight_bracket_forward(end);
    }
}

void change_cursor(void)
{
    kill_flag = 0;
    
    highlight_bracket();
}
