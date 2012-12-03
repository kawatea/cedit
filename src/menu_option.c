#include "util.h"
#include "action.h"
#include "indent.h"
#include "key.h"
#include "menu_option.h"

void set_font(void)
{
    GtkWidget *font_dialog;
    
    font_dialog = gtk_font_selection_dialog_new("フォント");
    
    gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(font_dialog), pango_font_description_to_string(gtk_widget_get_style(view)->font_desc));
    gtk_font_selection_dialog_set_preview_text(GTK_FONT_SELECTION_DIALOG(font_dialog), "abcdefghij ABCDEFGHIJ あいうえお");
    
    gtk_dialog_set_default_response(GTK_DIALOG(font_dialog), GTK_RESPONSE_OK);
    
    if (gtk_dialog_run(GTK_DIALOG(font_dialog)) == GTK_RESPONSE_OK) {
        gtk_widget_modify_font(view, pango_font_description_from_string(gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(font_dialog))));
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
    tab_button = gtk_spin_button_new_with_range(1, 32, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(tab_button), tab_width);
    
    gtk_box_pack_start(GTK_BOX(tab_hbox), tab_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tab_hbox), tab_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(tab_dialog)->vbox), tab_hbox, FALSE, FALSE, 0);
    
    gtk_entry_set_activates_default(GTK_ENTRY(tab_button), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(tab_dialog), GTK_RESPONSE_OK);
    
    gtk_widget_show_all(tab_dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(tab_dialog)) == GTK_RESPONSE_OK) {
        tab_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tab_button));
        
        gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view), tab_width);
    }
    
    gtk_widget_destroy(tab_dialog);
}

void set_wrap_line(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Wrap")))) {
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_CHAR);
    } else {
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_NONE);
    }
}

void set_display_line_number(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Line")))) {
        gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
    } else {
        gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), FALSE);
    }
}

void set_auto_indent(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Indent")))) {
        auto_flag = 1;
    } else {
        auto_flag = 0;
    }
}

void set_highlight_line(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Highlight")))) {
        gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), TRUE);
    } else {
        gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), FALSE);
    }
}

void set_insert_space(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Space")))) {
        tab_flag = 0;
    } else {
        tab_flag = 1;
    }
}

void set_delete_hungry(void)
{
    if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(actions, "Hungry")))) {
        delete_flag = 1;
    } else {
        delete_flag = 0;
    }
}
