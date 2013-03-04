#include "util.h"
#include "indent.h"
#include "key.h"
#include "menu_option.h"

void set_font(void)
{
    GtkWidget *font_dialog;
    
    font_dialog = gtk_font_selection_dialog_new("フォント");
    
    gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(font_dialog), font_name);
    gtk_font_selection_dialog_set_preview_text(GTK_FONT_SELECTION_DIALOG(font_dialog), "abcdefghij ABCDEFGHIJ あいうえお");
    
    gtk_dialog_set_default_response(GTK_DIALOG(font_dialog), GTK_RESPONSE_OK);
    
    if (gtk_dialog_run(GTK_DIALOG(font_dialog)) == GTK_RESPONSE_OK) {
        strcpy(font_name, gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(font_dialog)));
        gtk_widget_modify_font(view, pango_font_description_from_string(font_name));
    }
    
    gtk_widget_destroy(font_dialog);
}

void set_tab_width(void)
{
    GtkWidget *tab_dialog;
    GtkWidget *tab_hbox;
    GtkWidget *tab_label;
    GtkWidget *tab_button;
    
    tab_dialog = gtk_dialog_new_with_buttons("タブ", GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    tab_hbox = gtk_hbox_new(FALSE, 2);
    tab_label = gtk_label_new("新しいタブの幅 : ");
    tab_button = gtk_spin_button_new_with_range(1, 31, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(tab_button), state & width_mask);
    
    gtk_box_pack_start(GTK_BOX(tab_hbox), tab_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tab_hbox), tab_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(tab_dialog)->vbox), tab_hbox, FALSE, FALSE, 0);
    
    gtk_entry_set_activates_default(GTK_ENTRY(tab_button), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(tab_dialog), GTK_RESPONSE_OK);
    
    gtk_widget_show_all(tab_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(tab_dialog)) == GTK_RESPONSE_OK) {
        state &= ~width_mask;
        state |= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tab_button));
        
        gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view), state & width_mask);
    }
    
    gtk_widget_destroy(tab_dialog);
}

void set_wrap_line(void)
{
    if (state & wrap_mask) {
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_NONE);
    } else {
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_CHAR);
    }
    
    state ^= wrap_mask;
}

void set_display_line_number(void)
{
    if (state & line_mask) {
        gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), FALSE);
    } else {
        gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
    }
    
    state ^= line_mask;
}

void set_auto_indent(void)
{
    state ^= auto_mask;
}

void set_highlight_line(void)
{
    if (state & highlight_mask) {
        gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), FALSE);
    } else {
        gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), TRUE);
    }
    
    state ^= highlight_mask;
}

void set_insert_space(void)
{
    if (state & space_mask) {
        gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(view), FALSE);
    } else {
        gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(view), TRUE);
    }
    
    state ^= space_mask;
}

void set_delete_hungry(void)
{
    state ^= delete_mask;
}
