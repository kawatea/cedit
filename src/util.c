#include "util.h"
#include "file.h"

char file_name[1000];

//ファイル名が空かどうかチェックする
int empty_file_name(void)
{
    return file_name[0] == '\0';
}

//ファイル名を返す
const char *get_file_name(void)
{
    int i;
    
    for (i = strlen(file_name) - 1; i >= 0; i--) {
        if (file_name[i] == '/' || file_name[i] == '\\') break;
    }
    
    return &file_name[i + 1];
}

//ディレクトリを含めたファイル名を返す
const char *get_file_name_full(void)
{
    return file_name;
}

//ファイル名を変更する
void set_file_name(const char *name)
{
    strcpy(file_name, name);
}

//与えられたファイルのパスを返す
//メモリを解放すること
char *get_path(const char *file)
{
    char *path = malloc(1000);
    
    strcpy(path, getenv("CEDIT"));
    strcat(path, file);
    
    return path;
}

//与えられたファイルにあるアイコンを返す
GdkPixbuf *get_icon(const char *file)
{
    char *path = get_path("/icon/");
    
    strcat(path, file);
    
    GdkPixbuf *icon = gdk_pixbuf_new_from_file(path, NULL);
    
    free(path);
    
    return icon;
}

//設定ファイルを読み込む
void load_setting(void)
{
    char *path = get_path("/setting");
    FILE *fp = fopen(path, "r");
    
    fgets(font_name, 100, fp);
    font_name[strlen(font_name) - 1] = '\0';
    fscanf(fp, "%d %d %d %d %d", &window_width, &window_height, &window_x, &window_y, &state);
    
    fclose(fp);
    free(path);
    
    path = get_path("/flymake/c_flymake");
    fp = fopen(path, "r");
    
    fgets(c_flymake, 1000, fp);
    c_flymake[strlen(c_flymake) - 1] = '\0';
    
    fclose(fp);
    free(path);
    
    path = get_path("/flymake/cpp_flymake");
    fp = fopen(path, "r");
    
    fgets(cpp_flymake, 1000, fp);
    cpp_flymake[strlen(cpp_flymake) - 1] = '\0';
    
    fclose(fp);
    free(path);
}

//設定ファイルを保存する
void save_setting(void)
{
    char *path = get_path("/setting");
    FILE *fp = fopen(path, "w");
    
    fprintf(fp, "%s\n", font_name);
    fprintf(fp, "%d %d %d %d %d\n", window_width, window_height, window_x, window_y, state);
    
    fclose(fp);
    free(path);
    
    path = get_path("/flymake/c_flymake");
    fp = fopen(path, "w");
    
    fprintf(fp, "%s\n", c_flymake);
    
    fclose(fp);
    free(path);
    
    path = get_path("/flymake/cpp_flymake");
    fp = fopen(path, "w");
    
    fprintf(fp, "%s\n", cpp_flymake);
    
    fclose(fp);
    free(path);
}

//現在設定されている言語を返す
const char *get_language(void)
{
    GtkSourceLanguage *language = gtk_source_buffer_get_language(buffer);
    
    if (language == NULL) return "";
    
    return gtk_source_language_get_name(language);
}

//現在のファイルの言語を設定する
void set_language(void)
{
    char *path[] = {NULL, NULL};
    GtkSourceLanguageManager *manager = gtk_source_language_manager_new();
    
    path[0] = get_path("/language/");
    
    gtk_source_language_manager_set_search_path(manager, path);
    gtk_source_buffer_set_language(buffer, gtk_source_language_manager_guess_language(manager, get_file_name_full(), NULL));
    
    free(path[0]);
}

//ファイルのスタイルを設定する
void set_style(void)
{
    char *path[] = {NULL, NULL};
    GtkSourceStyleSchemeManager *manager = gtk_source_style_scheme_manager_new();
    
    path[0] = get_path("/style/");
    
    gtk_source_style_scheme_manager_set_search_path(manager, path);
    gtk_source_buffer_set_style_scheme(buffer, gtk_source_style_scheme_manager_get_scheme(manager, "mystyle"));
    
    free(path[0]);
}

//与えられた動作のアイコンの状態を変える
void set_action(const char *name, gboolean flag)
{
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, name), flag);
}
