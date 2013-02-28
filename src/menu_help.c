#include "util.h"
#include "menu_help.h"

void version(void)
{
    char s[1000];
    
    strcpy(s, getenv("CEDIT"));
    strcat(s, "/icon/logo.png");
    
    gtk_show_about_dialog(GTK_WINDOW(main_window), "program_name", "cedit", "version", "1.1", "copyright", "(C) 2012-2013 kawatea", "logo", gdk_pixbuf_new_from_file(s, NULL), NULL);
}
