#include <new_dialog.h>

#include <utils.h>

void update_clockwise(NewDialog *dialog) {
	if (dialog->clockwise) {
		gtk_button_set_image(dialog->clockwise_button, 
			GTK_WIDGET(gtk_image_new_from_file("ui/clockwise.png")));
		gtk_image_set_from_file(dialog->clockwise_image,
			"ui/counter-clockwise.png");
	} else {
		gtk_button_set_image(dialog->clockwise_button, 
			GTK_WIDGET(gtk_image_new_from_file("ui/counter-clockwise.png")));
		gtk_image_set_from_file(dialog->clockwise_image,
			"ui/clockwise.png");
	}
}

void update_piece(NewDialog *dialog) {
	if (dialog->white) {
		gtk_button_set_image(dialog->piece_button, 
			GTK_WIDGET(gtk_image_new_from_file("ui/white.png")));
		gtk_image_set_from_file(dialog->piece_image,
			"ui/black.png");
	} else {
		gtk_button_set_image(dialog->piece_button, 
			GTK_WIDGET(gtk_image_new_from_file("ui/black.png")));
		gtk_image_set_from_file(dialog->piece_image,
			"ui/white.png");
	}
}

void new_dialog_free(NewDialog *dialog) {
	gtk_widget_destroy(GTK_WIDGET(dialog->window));
	//g_free(dialog);
}

static void new_dialog_destroy(GtkWidget *widget, gpointer data) {
	new_dialog_free((NewDialog *) data);
}

static void clockwise_button_click(GtkWidget *widget, gpointer data) {
	NewDialog *dialog;
	dialog = (NewDialog *) data;

	if (dialog->clockwise) dialog->clockwise = FALSE;
	else dialog->clockwise = TRUE;

	update_clockwise(dialog);
}

static void piece_button_click(GtkWidget *widget, gpointer data) {
	NewDialog *dialog;
	dialog = (NewDialog *) data;

	if (dialog->white) dialog->white = FALSE;
	else dialog->white = TRUE;

	update_piece(dialog);
}

static void new_dialog_start_button_clicked(GtkWidget *widget, gpointer data) {
	NewDialog *dialog;
	Backgammon *bg;
	gint index;

	dialog = (NewDialog *) data;
	bg = dialog->bg;

	if (bg->status != S_NOT_PLAYING) {
		if (!question(GTK_WIDGET(bg->window), "¿Finalizar partida actual?")) {
			new_dialog_free((NewDialog *) data);
			return;
		}
	}


	bg->player[0].name = g_string_assign(bg->player[0].name,
		gtk_entry_get_text(dialog->pl1_entry)
	);
	bg->player[1].name = g_string_assign(bg->player[1].name,
		gtk_entry_get_text(dialog->pl2_entry)
	);

	index = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog->pl1_combo));
	if (index == 0) bg->player[0].play_func = human_play_func;
	else bg->player[0].play_func = ia_play_func;

	index = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog->pl2_combo));
	if (index == 0) bg->player[1].play_func = human_play_func;
	else bg->player[1].play_func = ia_play_func;

	if (dialog->white) {
		bg->player[0].piece = WHITE;
		bg->player[1].piece = BLACK;
	} else {
		bg->player[0].piece = BLACK;
		bg->player[1].piece = WHITE;
	}

	if (dialog->clockwise) {
		bg->player[0].direction = -1;
		bg->player[1].direction = 1;
	} else {
		bg->player[0].direction = 1;
		bg->player[1].direction = -1;
	}

	bg->status = S_NOT_PLAYING;
	board_init(bg->board);
	bg_next_step(bg);

	board_redraw(bg->board);

	new_dialog_free((NewDialog *) data);
}

NewDialog *new_dialog_new(Backgammon *bg) {
	NewDialog *dialog;
	GtkBuilder *builder;
	dialog = (NewDialog *) g_malloc(sizeof(NewDialog));
	dialog->bg = bg;

	dialog->clockwise = FALSE;
	dialog->white = FALSE;

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "ui/new-dialog.glade", NULL);
	dialog->window = GTK_WINDOW(gtk_builder_get_object(builder, "new-dialog"));
	gtk_window_set_transient_for(dialog->window, GTK_WINDOW(bg->window));

	g_signal_connect(dialog->window, "destroy", G_CALLBACK(new_dialog_destroy), dialog);

	dialog->pl1_entry = GTK_ENTRY(gtk_builder_get_object(builder, "pl1_entry"));
	dialog->pl2_entry = GTK_ENTRY(gtk_builder_get_object(builder, "pl2_entry"));

	dialog->pl1_combo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "pl1_combo"));
	dialog->pl2_combo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "pl2_combo"));

	dialog->clockwise_button = GTK_BUTTON(gtk_builder_get_object(builder, "clockwise-button"));
	g_signal_connect(dialog->clockwise_button, "clicked", 
		G_CALLBACK(clockwise_button_click), dialog);

	dialog->piece_button = GTK_BUTTON(gtk_builder_get_object(builder, "piece-button"));
	g_signal_connect(dialog->piece_button, "clicked", 
		G_CALLBACK(piece_button_click), dialog);

	dialog->clockwise_image = GTK_IMAGE(gtk_builder_get_object(builder, "clockwise-image"));
	dialog->piece_image = GTK_IMAGE(gtk_builder_get_object(builder, "piece-image"));

	g_signal_connect(
		GTK_BUTTON(gtk_builder_get_object(builder, "start-button")),
		"clicked", G_CALLBACK(new_dialog_start_button_clicked), dialog
	);

	update_clockwise(dialog);
	update_piece(dialog);

	g_object_unref(builder);

	return dialog;
}

void new_dialog_show(NewDialog *dialog) {
	gtk_widget_show_all(GTK_WIDGET(dialog->window));
}
