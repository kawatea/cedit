#include "util.h"
#include "style.h"

void set_style(void)
{
    GtkSourceStyleSchemeManager *manager;
    char *s[] = {NULL, NULL};
    
    s[0] = malloc(1000);
    strcpy(s[0], getenv("CEDIT"));
    strcat(s[0], "/style/");
    
    manager = gtk_source_style_scheme_manager_new();
    gtk_source_style_scheme_manager_set_search_path(manager, s);
    gtk_source_buffer_set_style_scheme(buffer, gtk_source_style_scheme_manager_get_scheme(manager, "mystyle"));
    
    free(s[0]);
}
