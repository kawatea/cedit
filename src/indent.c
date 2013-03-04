#include "util.h"
#include "indent.h"

int get_indent_depth(int line)
{
    int num = 0, len, flag = 0, i;
    GtkTextIter start, end;
    
    if (line < 0) return 0;
    
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &end, line + 1);
    
    strcpy(buf, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
    
    len = strlen(buf) - 1;
    
    if (len == 0) return get_indent_depth(line - 1);
    
    for (i = 0; i < len; i++) {
        if (buf[i] == ' ') {
            num++;
        } else if (buf[i] == '\t') {
            num += state & width_mask;
        } else {
            break;
        }
    }
    
    for (i = 0; i < len; i++) {
        if (buf[i] == '\\') {
            i++;
            
            continue;
        }
        
        if (buf[i] == '"' || buf[i] == '\'') {
            char c = buf[i];
            
            for (i++; i < len; i++) {
                if (buf[i] == '\\') {
                    i++;
                    
                    continue;
                }
                
                if (buf[i] == c) break;
            }
            
            continue;
        }
        
        if (buf[i] == '{') {
            flag++;
        } else if (buf[i] == '}') {
            flag--;
            
            if (flag < 0) flag = 0;
        }
    }
    
    if (flag > 0) num += state & width_mask;
    
    if (num < 0) num = 0;
    
    return num;
}

void set_indent_depth(int line, int depth)
{
    int len, flag = 0, i;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
    
    end = start;
    
    gtk_text_iter_forward_line(&end);
    
    strcpy(buf, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
    
    len = strlen(buf);
    
    for (i = 0; i < len; i++) {
        if (buf[i] == '\\') {
            i++;
            
            continue;
        }
        
        if (buf[i] == '"' || buf[i] == '\'') {
            char c = buf[i];
            
            for (i++; i < len; i++) {
                if (buf[i] == '\\') {
                    i++;
                    
                    continue;
                }
                
                if (buf[i] == c) break;
            }
            
            continue;
        }
        
        if (buf[i] == '{') {
            flag++;
        } else if (buf[i] == '}') {
            if (flag == 0) {
                depth -= state & width_mask;
            } else {
                flag--;
            }
        }
    }
    
    if (depth < 0) depth = 0;
    
    for (i = 0; i < len; i++) {
        if (buf[i] != ' ' && buf[i] != '\t') break;
    }
    
    end = start;
    
    gtk_text_iter_forward_chars(&end, i);
    
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    if (state & space_mask) {
        for (i = 0; i < depth; i++) buf[i] = ' ';
        
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &start, buf, depth);
    } else {
        depth /= state & width_mask;
        
        for (i = 0; i < depth; i++) buf[i] = '\t';
        
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &start, buf, depth);
    }
}

void set_indent(void)
{
    int line, depth;
    GtkTextIter cursor;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &cursor, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    
    line = gtk_text_iter_get_line(&cursor);
    
    depth = get_indent_depth(line - 1);
    
    set_indent_depth(line, depth);
}

void set_indent_all(void)
{
    int line_start, line_end, i;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
    
    line_start = gtk_text_iter_get_line(&start);
    line_end = gtk_text_iter_get_line(&end);
    
    for (i = line_start; i <= line_end; i++) {
        int depth = get_indent_depth(i - 1);
        
        set_indent_depth(i, depth);
    }
}
