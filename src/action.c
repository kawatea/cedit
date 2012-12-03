#include "util.h"
#include "action.h"

void set_action(char *name, gboolean flag)
{
    gtk_action_set_sensitive(gtk_action_group_get_action(actions, name), flag);
}
