#include "util.h"
#include "action.h"
#include "menu_file.h"
#include "menu_edit.h"
#include "menu_search.h"
#include "menu_move.h"
#include "menu_option.h"
#include "menu_help.h"
#include "ui.h"

const gchar *menu_info =
"<ui>"
"  <menubar name='Menubar'>"
"    <menu name='File' action='File'>"
"      <menuitem name='New' action='New'/>"
"      <menuitem name='Open' action='Open'/>"
"      <separator/>"
"      <menuitem name='Save' action='Save'/>"
"      <menuitem name='Another' action='Another'/>"
"      <separator/>"
"      <menuitem name='Print' action='Print'/>"
"      <separator/>"
"      <menuitem name='Quit' action='Quit'/>"
"    </menu>"
"    <menu name='Edit' action='Edit'>"
"      <menuitem name='Undo' action='Undo'/>"
"      <menuitem name='Redo' action='Redo'/>"
"      <separator/>"
"      <menuitem name='Cut' action='Cut'/>"
"      <menuitem name='Copy' action='Copy'/>"
"      <menuitem name='Paste' action='Paste'/>"
"      <menuitem name='Delete' action='Delete'/>"
"      <menuitem name='Kill' action='Kill'/>"
"      <separator/>"
"      <menuitem name='Select' action='Select'/>"
"    </menu>"
"    <menu name='Search' action='Search'>"
"      <menuitem name='Find' action='Find'/>"
"      <menuitem name='Replace' action='Replace'/>"
"    </menu>"
"    <menu name='Move' action='Move'>"
"      <menuitem name='Jump' action='Jump'/>"
"      <menuitem name='Up' action='Up'/>"
"      <menuitem name='Down' action='Down'/>"
"      <menuitem name='Left' action='Left'/>"
"      <menuitem name='Right' action='Right'/>"
"    </menu>"
"    <menu name='Option' action='Option'>"
"      <menuitem name='Font' action='Font'/>"
"      <menuitem name='Tab' action='Tab'/>"
"      <separator/>"
"      <menuitem name='Wrap' action='Wrap'/>"
"      <menuitem name='Line' action='Line'/>"
"      <menuitem name='Indent' action='Indent'/>"
"      <menuitem name='Highlight' action='Highlight'/>"
"      <menuitem name='Space' action='Space'/>"
"      <menuitem name='Hungry' action='Hungry'/>"
"    </menu>"
"    <menu name='About' action='About'>"
"      <menuitem name='Version' action='Version'/>"
"    </menu>"
"  </menubar>"
"  <toolbar name='Toolbar'>"
"      <toolitem name='New' action='New'/>"
"      <toolitem name='Open' action='Open'/>"
"      <toolitem name='Save' action='Save'/>"
"      <toolitem name='Another' action='Another'/>"
"      <toolitem name='Print' action='Print'/>"
"      <toolitem name='Undo' action='Undo'/>"
"      <toolitem name='Redo' action='Redo'/>"
"      <toolitem name='Cut' action='Cut'/>"
"      <toolitem name='Copy' action='Copy'/>"
"      <toolitem name='Paste' action='Paste'/>"
"      <toolitem name='Find' action='Find'/>"
"      <toolitem name='Replace' action='Replace'/>"
"  </toolbar>"
"</ui>";

GtkActionEntry entries[] =
{{"File", NULL, "ファイル(_F)"}, {"New", GTK_STOCK_NEW, "新規作成(_N)", "<control>N", "ファイルを新規作成する", G_CALLBACK(new_file)}, {"Open", GTK_STOCK_OPEN, "開く(_O)", "<control>O", "ファイルを開く", G_CALLBACK(open_file)}, {"Save", GTK_STOCK_SAVE, "上書き保存(_S)", "<control>S", "ファイルを上書き保存する", G_CALLBACK(save_file)}, {"Another", GTK_STOCK_SAVE_AS, "名前をつけて保存(_A)", "<shift><control>S", "ファイルを名前をつけて保存する", G_CALLBACK(save_file_another)}, {"Print", GTK_STOCK_PRINT, "印刷(_P)", "<control>P", "ファイルを印刷する", G_CALLBACK(print_file)}, {"Quit", GTK_STOCK_QUIT, "ceditの終了(_Q)", "<control>Q", NULL, G_CALLBACK(quit)}, {"Edit", NULL, "編集(_E)"}, {"Undo", GTK_STOCK_UNDO, "元に戻す(_U)", "<control>Z", "元に戻す", G_CALLBACK(undo_action)}, {"Redo", GTK_STOCK_REDO, "やり直し(_R)", "<control>Y", "やり直し", G_CALLBACK(redo_action)}, {"Cut", GTK_STOCK_CUT, "切り取り(_X)", "<control>X", "切り取り", G_CALLBACK(cut_text)}, {"Copy", GTK_STOCK_COPY, "コピー(_C)", "<control>C", "コピー", G_CALLBACK(copy_text)}, {"Paste", GTK_STOCK_PASTE, "貼り付け(_V)", "<control>V", "貼り付け", G_CALLBACK(paste_text)}, {"Delete", GTK_STOCK_DELETE, "削除(_D)", "<control>D", NULL, G_CALLBACK(delete_text)}, {"Kill", GTK_STOCK_CANCEL, "一行削除(_K)", "<control>K", NULL, G_CALLBACK(kill_line)}, {"Select", GTK_STOCK_SELECT_ALL, "全て選択(_A)", "<control>A", NULL, G_CALLBACK(select_all)}, {"Search", NULL, "検索(_S)"}, {"Find", GTK_STOCK_FIND, "検索(_F)", "<control>F", "文字列を検索する", G_CALLBACK(search_text)}, {"Replace", GTK_STOCK_FIND_AND_REPLACE, "置換(_R)", "<control>H", "文字列を置換する", G_CALLBACK(replace_text)}, {"Move", NULL, "移動(_M)"}, {"Jump", GTK_STOCK_JUMP_TO, "指定行に移動(_J)", "<control>G", NULL, G_CALLBACK(jump_line)}, {"Up", GTK_STOCK_GOTO_TOP, "ファイルの先頭に移動(_U)", "<control>Up", NULL, G_CALLBACK(jump_up)}, {"Down", GTK_STOCK_GOTO_BOTTOM, "ファイルの末尾に移動(_D)", "<control>Down", NULL, G_CALLBACK(jump_down)}, {"Left", GTK_STOCK_GOTO_FIRST, "行頭に移動(_L)", "<control>Left", NULL, G_CALLBACK(jump_left)}, {"Right", GTK_STOCK_GOTO_LAST, "行末に移動(_R)", "<control>Right", NULL, G_CALLBACK(jump_right)}, {"Option", NULL, "オプション(_O)"}, {"Font", GTK_STOCK_SELECT_FONT, "フォント(_F)", NULL, NULL, G_CALLBACK(set_font)}, {"Tab", GTK_STOCK_INDENT, "タブ幅の変更(_T)", NULL, NULL, G_CALLBACK(set_tab_width)}, {"About", NULL, "ヘルプ(_H)"}, {"Version", GTK_STOCK_ABOUT, "ヴァージョン情報(_V)", NULL, NULL, G_CALLBACK(version)}};

GtkToggleActionEntry toggle_entries[] =
{{"Wrap", NULL, "右端で折り返す", NULL, NULL, G_CALLBACK(set_wrap_line), FALSE}, {"Line", NULL, "行番号を表示する", NULL, NULL, G_CALLBACK(set_display_line_number), FALSE}, {"Indent", NULL, "オートインデント", NULL, NULL, G_CALLBACK(set_auto_indent), FALSE}, {"Highlight", NULL, "カーソル行をハイライト", NULL, NULL, G_CALLBACK(set_highlight_line), FALSE}, {"Space", NULL, "タブの代わりにスペースを挿入", NULL, NULL, G_CALLBACK(set_insert_space), FALSE}, {"Hungry", NULL, "空白をまとめて消去", NULL, NULL, G_CALLBACK(set_delete_hungry), FALSE}};

void init_ui(GtkWidget *box)
{
    GtkUIManager *ui;
    GtkWidget *menubar;
    GtkWidget *toolbar;
    
    if (state & wrap_mask) toggle_entries[0].is_active = TRUE;
    if (state & line_mask) toggle_entries[1].is_active = TRUE;
    if (state & auto_mask) toggle_entries[2].is_active = TRUE;
    if (state & highlight_mask) toggle_entries[3].is_active = TRUE;
    if (state & space_mask) toggle_entries[4].is_active = TRUE;
    if (state & delete_mask) toggle_entries[5].is_active = TRUE;
    
    actions = gtk_action_group_new("menu");
    gtk_action_group_add_actions(actions, entries, sizeof(entries) / sizeof(entries[0]), main_window);
    gtk_action_group_add_toggle_actions(actions, toggle_entries, sizeof(toggle_entries) / sizeof(toggle_entries[0]), main_window);
    
    ui = gtk_ui_manager_new();
    gtk_ui_manager_insert_action_group(ui, actions, 0);
    gtk_ui_manager_add_ui_from_string(ui, menu_info, -1, NULL);
    gtk_window_add_accel_group(GTK_WINDOW(main_window), gtk_ui_manager_get_accel_group(ui));
    
    menubar = gtk_ui_manager_get_widget(ui, "/Menubar");
    toolbar = gtk_ui_manager_get_widget(ui, "/Toolbar");
    
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_LARGE_TOOLBAR);
    
    gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), toolbar, FALSE, FALSE, 0);
}
