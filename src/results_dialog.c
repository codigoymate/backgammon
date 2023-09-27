#include <results_dialog.h>

void result_dialog_free(ResultsDialog *dialog) {
	g_free(dialog);
}

static void result_dialog_destroy(GtkWindow *window, gpointer data) {
	ResultsDialog *dialog;
	dialog = (ResultsDialog *) data;
	result_dialog_free(dialog);
}

static void result_dialog_ok_clicked(GtkButton *button, gpointer data) {
	ResultsDialog *dialog;
	dialog = (ResultsDialog *) data;
	gtk_widget_destroy(GTK_WIDGET(dialog->window)); 
}

ResultsDialog *results_dialog_new(Backgammon *bg, Player *winner, guint winner_score) {
	ResultsDialog *dialog;
	GtkBuilder *builder;
	GString *str = NULL;

	dialog = (ResultsDialog *) g_malloc(sizeof(ResultsDialog));
	dialog->bg = bg;
	dialog->winner = winner;
	dialog->winner_score = winner_score;

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "ui/results.glade", NULL);

	dialog->window = GTK_WINDOW(gtk_builder_get_object(builder, "results-window"));
	gtk_window_set_transient_for(dialog->window, GTK_WINDOW(bg->window));

	dialog->winner_label = GTK_LABEL(gtk_builder_get_object(builder, "winner-label"));
	dialog->total_pl1_label = GTK_LABEL(gtk_builder_get_object(builder, "total-pl1-label"));
	dialog->total_pl2_label = GTK_LABEL(gtk_builder_get_object(builder, "total-pl2-label"));

	str = g_string_new("");
	g_string_append_printf(str, "%s gana la ronda con %u puntos",
			dialog->winner->name->str, dialog->winner_score);
	gtk_label_set_text(dialog->winner_label, str->str);

	str = g_string_assign(str, "");
	g_string_append_printf(str, "%s: %u", bg->player[0].name->str, bg->player[0].score);
	gtk_label_set_text(dialog->total_pl1_label, str->str);

	str = g_string_assign(str, "");
	g_string_append_printf(str, "%s: %u", bg->player[1].name->str, bg->player[1].score);
	gtk_label_set_text(dialog->total_pl2_label, str->str);

	g_string_free(str, TRUE);

	dialog->ok_button = GTK_BUTTON(gtk_builder_get_object(builder, "ok-button"));
	g_signal_connect(dialog->ok_button, "clicked", G_CALLBACK(result_dialog_ok_clicked), dialog);

	g_signal_connect(dialog->window, "destroy", G_CALLBACK(result_dialog_destroy), dialog);

	g_object_unref(builder);

	return dialog;
}

void results_dialog_show(ResultsDialog *dialog) {
	gtk_widget_show_all(GTK_WIDGET(dialog->window));
}
