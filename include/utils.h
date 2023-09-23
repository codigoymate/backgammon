#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>

gboolean question(GtkWidget *parent, const char *msg);

GString *randomize_name(void);

#endif
