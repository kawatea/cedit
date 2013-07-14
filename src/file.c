#include "util.h"
#include "style.h"
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

void check_encoding(void)
{
    int num = 0, ascii_flag = 1, jis_flag = 0, utf_flag = 1, euc_flag = 0, sjis_flag = 0;
    
    if ((fp = fopen(get_file_name_full(), "r")) == NULL) return;
    
    while (1) {
        int c = fgetc(fp);
        
        if (c == EOF) break;
        
        if (c == 0x1B) {
            jis_flag = 1;
            
            break;
        }
        
        if ((c >> 7) & 1) ascii_flag = 0;
        
        if (c == 0xFD || c == 0xFE) euc_flag = 1;
        
        if (c >= 0x80 && c <= 0xA0) sjis_flag = 1;
        
        if (num == 0) {
            if ((c >> 7) & 1) {
                if (c < 0xC0 || c > 0xFD) utf_flag = 0;
                
                if ((c & 0xFC) == 0xFC) {
                    num = 5;
                } else if ((c & 0xF8) == 0xF8) {
                    num = 4;
                } else if ((c & 0xF0) == 0xF0) {
                    num = 3;
                } else if ((c & 0xE0) == 0xE0) {
                    num = 2;
                } else {
                    num = 1;
                }
            }
        } else {
            if (c < 0x80 || c > 0xBF) utf_flag = 0;
            
            num--;
        }
    }
    
    fclose(fp);
    
    if (jis_flag == 1) {
        encoding = JIS;
    } else if (ascii_flag == 1 || utf_flag == 1) {
        encoding = UTF8;
    } else if (sjis_flag == 1 && euc_flag == 0) {
        encoding = SJIS;
    } else {
        encoding = EUC;
    }
}

void read_file(void)
{
    check_encoding();
    
    set_radio_action();
    
    if ((fp = fopen(get_file_name_full(), "r")) != NULL) {
        int num = 0;
        GtkTextIter start, end;
        
        gtk_source_buffer_begin_not_undoable_action(buffer);
        
        set_start_end_iter(&start, &end);
        
        if (encoding == JIS) {
            int size;
            char const *p1;
            char *p2;
            iconv_t cd = iconv_open("UTF-8", "ISO-2022-JP");
            
            while (1) {
                int c = fgetc(fp);
                
                if (c == EOF) break;
                
                if (c == 0x1B || num > 50000) {
                    p1 = buf;
                    p2 = cbuf;
                    size = BUF_SIZE - 1;
                    
                    iconv(cd, &p1, &num, &p2, &size);
                    
                    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, cbuf, BUF_SIZE - size - 1);
                    
                    num = 0;
                }
                
                if (c == 0x1B) {
                    buf[num++] = c;
                    
                    c = fgetc(fp);
                    
                    if (c == 0x24) {
                        buf[num++] = c;
                        
                        c = fgetc(fp);
                        
                        if (c == 0x28) {
                            buf[num++] = c;
                            buf[num++] = fgetc(fp);
                        } else {
                            buf[num++] = c;
                        }
                    } else if (c == 0x28) {
                        buf[num++] = c;
                        buf[num++] = fgetc(fp);
                    } else {
                        buf[num++] = c;
                        buf[num++] = fgetc(fp);
                        buf[num++] = fgetc(fp);
                        buf[num++] = fgetc(fp);
                        buf[num++] = fgetc(fp);
                    }
                } else {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                }
            }
            
            p1 = buf;
            p2 = cbuf;
            size = BUF_SIZE - 1;
            
            iconv(cd, &p1, &num, &p2, &size);
            
            gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, cbuf, BUF_SIZE - size - 1);
            
            iconv_close(cd);
        } else if (encoding == SJIS) {
            int size;
            char const *p1;
            char *p2;
            iconv_t cd = iconv_open("UTF-8", "Shift_JIS");
            
            while (1) {
                int c = fgetc(fp);
                
                if (c == EOF) break;
                
                if (((c >> 7) & 1) == 0 || (c >= 0xA1 && c <= 0xDF)) {
                    buf[num++] = c;
                } else {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                }
                
                if (num > 30000) {
                    p1 = buf;
                    p2 = cbuf;
                    size = BUF_SIZE - 1;
                    
                    iconv(cd, &p1, &num, &p2, &size);
                    
                    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, cbuf, BUF_SIZE - size - 1);
                    
                    num = 0;
                }
            }
            
            p1 = buf;
            p2 = cbuf;
            size = BUF_SIZE - 1;
            
            iconv(cd, &p1, &num, &p2, &size);
            
            gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, cbuf, BUF_SIZE - size - 1);
            
            iconv_close(cd);
        } else if (encoding == EUC) {
            int size;
            char const *p1;
            char *p2;
            iconv_t cd = iconv_open("UTF-8", "EUC-JP");
            
            while (1) {
                int c = fgetc(fp);
                
                if (c == EOF) break;
                
                if (((c >> 7) & 1) == 0) {
                    buf[num++] = c;
                } else if (c != 0x8F) {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                } else {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                    buf[num++] = fgetc(fp);
                }
                
                if (num > 50000) {
                    p1 = buf;
                    p2 = cbuf;
                    size = BUF_SIZE - 1;
                    
                    iconv(cd, &p1, &num, &p2, &size);
                    
                    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, cbuf, BUF_SIZE - size - 1);
                    
                    num = 0;
                }
            }
            
            p1 = buf;
            p2 = cbuf;
            size = BUF_SIZE - 1;
            
            iconv(cd, &p1, &num, &p2, &size);
            
            gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, cbuf, BUF_SIZE - size - 1);
            
            iconv_close(cd);
        } else {
            while (1) {
                int c = fgetc(fp);
                
                if (c == EOF) break;
                
                if (((c >> 7) & 1) == 0) {
                    buf[num++] = c;
                } else if (((c >> 5) & 1) == 0) {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                } else if (((c >> 4) & 1) == 0) {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                    buf[num++] = fgetc(fp);
                } else {
                    buf[num++] = c;
                    buf[num++] = fgetc(fp);
                    buf[num++] = fgetc(fp);
                    buf[num++] = fgetc(fp);
                }
                
                if (num > 100000) {
                    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, buf, num);
                    
                    num = 0;
                }
            }
            
            gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, buf, num);
        }
        
        set_start_end_iter(&start, &end);
        
        gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &start);
        
        fclose(fp);
        
        gtk_source_buffer_end_not_undoable_action(buffer);
    }
}

void write_file(void)
{
    int count, step = 25000, i;
    GtkTextIter start, end;
    
    count = gtk_text_buffer_get_char_count(GTK_TEXT_BUFFER(buffer));
    
    fp = fopen(get_file_name_full(), "w");
    
    set_start_end_iter(&start, &end);
    
    end = start;
    
    if (encoding == UTF8) {
        for (i = 0; i < count; i += step) {
            start = end;
            
            if (i + step >= count) {
                gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
            } else {
                gtk_text_iter_set_offset(&end, i + step);
            }
            
            strcpy(buf, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
            
            fprintf(fp, "%s", buf);
        }
    } else {
        int num, size;
        char const *p1;
        char *p2;
        iconv_t cd;
        
        if (encoding == JIS) {
            step = 10000;
            cd = iconv_open("ISO-2022-JP", "UTF-8");
        } else if (encoding == SJIS) {
            cd = iconv_open("Shift_JIS", "UTF-8");
        } else {
            cd = iconv_open("EUC-JP", "UTF-8");
        }
        
        for (i = 0; i < count; i += step) {
            start = end;
            
            if (i + step >= count) {
                gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
            } else {
                gtk_text_iter_set_offset(&end, i + step);
            }
            
            strcpy(buf, gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, TRUE));
            
            p1 = buf;
            p2 = cbuf;
            num = strlen(buf);
            size = BUF_SIZE - 1;
            
            iconv(cd, &p1, &num, &p2, &size);
            
            *p2 = '\0';
            
            fprintf(fp, "%s", cbuf);
        }
        
        iconv_close(cd);
    }

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
