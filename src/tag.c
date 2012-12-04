#include "util.h"
#include "tag.h"

void init_tag(void)
{
    search_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFFF00", NULL);
    match_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#40E0D0", NULL);
    unmatch_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#A020F0", NULL);
    error_tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(buffer), NULL, "background", "#FFBBBB", NULL);
}

void delete_tag(int flag)
{
    GtkTextIter start, end;
    
    set_start_end_iter(&start, &end);
    
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), search_tag, &start, &end);
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), match_tag, &start, &end);
    gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), unmatch_tag, &start, &end);
    
    if (flag == 1) gtk_text_buffer_remove_tag(GTK_TEXT_BUFFER(buffer), error_tag, &start, &end);
}
