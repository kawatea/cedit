#include "util.h"
#include "indent.h"

//指定された行のインデントから次の行のインデントを計算する
int get_indent_depth(int line)
{
    if (line < 0) return 0;
    
    int num = 0, len, count = 0, i;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &end, line + 1);
    char *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE);
    len = strlen(text) - 1;
    
    if (len == 0) {
        free(text);
        
        return get_indent_depth(line - 1);
    }
    
    //空白の数を数える
    for (i = 0; i < len; i++) {
        if (text[i] == ' ') {
            num++;
        } else if (text[i] == '\t') {
            num += state & width_mask;
        } else {
            break;
        }
    }
    
    //開いたかっこの数を数える
    for (i = 0; i < len; i++) {
        if (text[i] == '\\') {
            i++;
            
            continue;
        }
        
        if (text[i] == '"' || text[i] == '\'') {
            char c = text[i];
            
            for (i++; i < len; i++) {
                if (text[i] == '\\') {
                    i++;
                    
                    continue;
                }
                
                if (text[i] == c) break;
            }
            
            continue;
        }
        
        if (text[i] == '{') {
            count++;
        } else if (text[i] == '}') {
            count--;
            
            if (count < 0) count = 0;
        }
    }
    
    num += (state & width_mask) * count;
    
    free(text);
    
    return num;
}

//指定された行をある深さでインデントする
void set_indent_depth(int line, int depth)
{
    int len, count = 0, i;
    GtkTextIter start, end;
    
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start, line);
    end = start;
    gtk_text_iter_forward_line(&end);
    char *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE);
    len = strlen(text);
    
    //閉じたかっこの数を数える
    for (i = 0; i < len; i++) {
        if (text[i] == '\\') {
            i++;
            
            continue;
        }
        
        if (text[i] == '"' || text[i] == '\'') {
            char c = text[i];
            
            for (i++; i < len; i++) {
                if (text[i] == '\\') {
                    i++;
                    
                    continue;
                }
                
                if (text[i] == c) break;
            }
            
            continue;
        }
        
        if (text[i] == '{') {
            count++;
        } else if (text[i] == '}') {
            if (count == 0) {
                depth -= state & width_mask;
            } else {
                count--;
            }
        }
    }
    
    if (depth < 0) depth = 0;
    
    for (i = 0; i < len; i++) {
        if (text[i] != ' ' && text[i] != '\t') break;
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
    
    free(text);
}

//カーソルのある行をインデントする
void set_indent(void)
{
    int line, depth;
    GtkTextIter cursor;
    
    gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &cursor, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
    line = gtk_text_iter_get_line(&cursor);
    depth = get_indent_depth(line - 1);
    set_indent_depth(line, depth);
}

//選択された範囲の行を全てインデントする
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
