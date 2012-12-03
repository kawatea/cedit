#include "util.h"
#include "file.h"
#include "language.h"

void set_language(void)
{
    GtkSourceLanguageManager *manager;
    char *s[] = {NULL, NULL};
    
    s[0] = malloc(1000);
    strcpy(s[0], getenv("HOME"));
    strcat(s[0], "/cedit/language/");
    
    manager = gtk_source_language_manager_new();
    gtk_source_language_manager_set_search_path(manager, s);
    gtk_source_buffer_set_language(buffer, gtk_source_language_manager_guess_language(manager, get_file_name_full(), NULL));
    
    free(s[0]);
}

const char *get_language(void)
{
    GtkSourceLanguage *language;
    
    language = gtk_source_buffer_get_language(buffer);
    
    if (language == NULL) return "";
    
    return gtk_source_language_get_name(language);
}