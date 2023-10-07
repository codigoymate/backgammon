/**
 * @file utils.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief implementation of utils.h
 * @version 0.1
 * @date 2023-10-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <utils.h>

/**
 * @brief Show yes/no question dialog
 * 
 * @param parent Parent window
 * @param msg The context message
 * @return gboolean TRUE if "yes" was selected
 */
gboolean question(GtkWidget *parent, const char *msg) {

	GtkWidget *dialog;
	gboolean result = FALSE;

	dialog = gtk_message_dialog_new(
		GTK_WINDOW(parent),
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION,
		GTK_BUTTONS_YES_NO,
		"%s", msg
	);

	gtk_window_set_title(GTK_WINDOW(dialog), "Backgammon");

	switch(gtk_dialog_run(GTK_DIALOG(dialog))) {
	case GTK_RESPONSE_YES:
		result = TRUE;
		break;
	}

	gtk_widget_destroy(dialog);

	return result;
}

/**
 * @brief Show information dialog
 * 
 * @param parent Parent window
 * @param msg The context message
 */
void information(GtkWidget *parent, const char *msg) {
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(
		GTK_WINDOW(parent),
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"%s", msg
	);

	gtk_window_set_title(GTK_WINDOW(dialog), "Backgammon");

	gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
}

/**
 * @brief Pick a name from ui/names
 * randomize a name from 100 names file and returns it.
 * In file: 50 male names and 50 female names.
 * 
 * @return GString* the randomize name
 */
GString *randomize_name(void) {
    gchar *file_contents = NULL, **lines, **line;
    gsize file_length = 0;
	guint i = 0, chosen;
	GString *result;

    if (g_file_get_contents("ui/names", &file_contents, &file_length, NULL)) {
        lines = g_strsplit(file_contents, "\n", -1);

		chosen = g_random_int_range(0, 100);

        for (line = lines; *line; line ++) {
            if (i == chosen) {
				result = g_string_new(*line);
				break;
			}
			i ++;
        }

        g_strfreev(lines);
        g_free(file_contents);
    } else {
        g_print("ERROR: Cannot read names file.\n");
    }

    return result;
}
