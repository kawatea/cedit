#ifndef MENU_EDIT
#define MENU_EDIT

void undo_action(void);
void redo_action(void);
void cut_text(void);
void copy_text(void);
void paste_text(void);
void delete_text(void);
void kill_line(void);
void select_all(void);

void change_undo(void);
void change_redo(void);
void change_selection(void);

#endif
