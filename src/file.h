#ifndef FILE
#define FILE

int empty_file_name(void);
const char *get_file_name(void);
const char *get_file_name_full(void);
void set_file_name(char *name);
void read_file(void);
void write_file(void);
void delete_file(void);
void start_file(char *name);

#endif
