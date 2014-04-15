#include "util.h"
#include "menu_help.h"

//ヴァージョン情報を表示する
void version(void)
{
    gtk_show_about_dialog(GTK_WINDOW(main_window), "program_name", "cedit", "version", "1.3.2", "copyright", "(C) 2012-2014 kawatea", "logo", get_icon("logo.png"), NULL);
}
