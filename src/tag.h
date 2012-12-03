#ifndef TAG
#define TAG

GtkTextTag *search_tag;
GtkTextTag *match_tag;
GtkTextTag *unmatch_tag;
GtkTextTag *error_tag;

void init_tag(void);
void delete_tag(int flag);

#endif
