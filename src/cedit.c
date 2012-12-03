#include "util.h"
#include "file.h"
#include "init.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    
    init_editor();
    
    if (argc != 1) start_file(argv[1]);
    
    gtk_widget_show_all(main_window);
    
    gtk_main();
    
    return 0;
}
