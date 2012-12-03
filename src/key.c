#include "util.h"
#include "indent.h"
#include "delete_hungry.h"
#include "key.h"

gboolean key_press(GtkWidget *widget, GdkEventKey *event)
{
    if (event->keyval == GDK_Tab) {
        if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(buffer))) {
            set_indent_all();
        } else {
            set_indent();
        }
        
        return TRUE;
    } else if (event->keyval == GDK_BackSpace && delete_flag == 1) {
        if (delete_hungry_backward()) return TRUE;
    } else if (event->keyval == GDK_Delete && delete_flag == 1) {
        if (delete_hungry_forward()) return TRUE;
    }
    
    return FALSE;
}

gboolean key_release(GtkWidget *widget, GdkEventKey *event)
{
    if (event->keyval == GDK_braceright && (event->state & GDK_SHIFT_MASK)) {
        set_indent();
    } else if (event->keyval == GDK_Return && auto_flag == 1) {
        set_indent();
    }
    
    return FALSE;
}
