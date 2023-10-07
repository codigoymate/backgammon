/**
 * @file utils.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Misc functions module.
 * @version 0.1
 * @date 2023-10-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>

/**
 * @brief Show yes/no question dialog
 * 
 * @param parent Parent window
 * @param msg The context message
 * @return gboolean TRUE if "yes" was selected
 */
gboolean question(GtkWidget *parent, const char *msg);

/**
 * @brief Show information dialog
 * 
 * @param parent Parent window
 * @param msg The context message
 */
void information(GtkWidget *parent, const char *msg);

/**
 * @brief Pick a name from ui/names
 * randomize a name from 100 names file and returns it.
 * In file: 50 male names and 50 female names.
 * 
 * @return GString* the randomize name
 */
GString *randomize_name(void);

#endif
