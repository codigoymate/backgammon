#include <utils.h>

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
