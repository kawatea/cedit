#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char path[3][100] = {
"/cedit/etc/gtk-2.0/gdk-pixbuf.loaders",
"/cedit/etc/gtk-2.0/gtk.immodules",
"/cedit/etc/pango/pango.modules"
};

char buf[200][1000];

int main()
{
    int i, j, k;
    char home[1000];
    FILE *fp;
    
    strcpy(home, getenv("HOME"));
    
    for (i = 0; i < strlen(home); i++) {
        if (home[i] == '\\') home[i] = '/';
    }
    
    for (i = 0; i < 3; i++) {
        char target[1000];
        
        strcpy(target, home);
        strcat(target, path[i]);
        
        fp = fopen(target, "r");
        
        for (j = 0; ; j++) {
            if (fgets(buf[j], 1000, fp) == NULL) break;
        }
        
        fclose(fp);
        
        fp = fopen(target, "w");
        
        for (k = 0; k < j; k++) {
            char *tmp = strstr(buf[k], "HOME");
            
            if (tmp == NULL) {
                fprintf(fp, "%s", buf[k]);
            } else {
                char *now = buf[k];
                
                while (now != tmp) fputc(*(now++), fp);
                
                fprintf(fp, "%s", home);
                
                fprintf(fp, "%s", tmp + 4);
            }
        }
        
        fclose(fp);
    }
    
    return 0;
}
