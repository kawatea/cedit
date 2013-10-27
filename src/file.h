#ifndef __FILE_H__
#define __FILE_H__

enum {
    JIS,
    SJIS,
    EUC,
    UTF8
};

void read_file(void);
void write_file(void);
void delete_file(void);
void start_file(const char *name);

#endif
