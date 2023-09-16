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
