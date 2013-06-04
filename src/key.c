#include "util.h"
#include "indent.h"
#include "delete_hungry.h"
#include "key.h"

int press_brace;

gboolean key_press(GtkWidget *widget, GdkEventKey *event)
{
    if (event->keyval == GDK_Tab && (state & auto_mask)) {
        if (gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(buffer))) {
            set_indent_all();
        } else {
            set_indent();
        }
        
        return TRUE;
    } else if (event->keyval == GDK_BackSpace && (state & delete_mask)) {
        if (delete_hungry_backward()) return TRUE;
    } else if (event->keyval == GDK_Delete && (state & delete_mask)) {
        if (delete_hungry_forward()) return TRUE;
    }
    
    if (event->keyval == GDK_braceright && (state & auto_mask)) press_brace = 1;
    
    return FALSE;
}

gboolean key_release(GtkWidget *widget, GdkEventKey *event)
{
    if ((event->keyval == GDK_braceright || (event->keyval == GDK_bracketright && press_brace == 1)) && (state & auto_mask)) {
        press_brace = 0;
        
        set_indent();
    } else if (event->keyval == GDK_Return && (state & auto_mask)) {
        set_indent();
    }
    
    return FALSE;
}
