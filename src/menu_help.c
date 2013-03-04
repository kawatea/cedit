#include "util.h"
#include "menu_help.h"

void version(void)
{
    strcpy(buf, getenv("CEDIT"));
    strcat(buf, "/icon/logo.png");
    
    gtk_show_about_dialog(GTK_WINDOW(main_window), "program_name", "cedit", "version", "1.2.0", "copyright", "(C) 2012-2013 kawatea", "logo", gdk_pixbuf_new_from_file(buf, NULL), NULL);
}
