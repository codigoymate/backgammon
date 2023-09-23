/**
 * @file backgammon.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of backgammon.h
 * @date 2023-08-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <backgammon.h>

#include <player.h>

#include <new_dialog.h>

/**
 * @brief Frees the Backgammon instance.
 * 
 * @param bg Backgammon instance
 */
void bg_free(Backgammon *bg);

/**
 * @brief Occurs when the main window is closed.
 * Frees the Backgammon instance.
 * 
 * @param window main window instance
 * @param data Backgammon instance
 */
static void bg_on_window_destroyed(GtkApplicationWindow *window, gpointer data) {
	bg_free(data);
}

/**
 * @brief Occurs when clicking on the "game"->"new game" menu item.
 * If the game state is S_NOT_PLAYING, ends the current game if the user agrees.
 * When starting a new game, resets the board and initializes the players.
 * 
 * @param menu_item new game menu item
 * @param data Backgammon instance
 */
static void new_game_menu_item_activate(GtkMenuItem *menu_item, gpointer data) {
	new_dialog_show(new_dialog_new((Backgammon *)data));
}

/**
 * @brief Occurs when clicking the "Next Turn" button.
 * Typically associated with the human player. Switches to the next player's turn.
 * 
 * @param button button instance
 * @param data Backgammon instance
 */
static void end_turn_button_clicked(GtkWidget *button, gpointer data) {
	Backgammon *bg;
	bg = (Backgammon *)data;

	gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);

	bg_next_turn(bg);
	bg_next_step(bg);
}

/**
 * @brief Creates an instance of Backgammon.
 * Initializes the GTK environment.
 * Creates the board.
 * Sets the game state to S_NOT_PLAYING.
 * 
 * @param argc argument count
 * @param argv argument vector
 * @return Backgammon* instance of Backgammon
 */
Backgammon *bg_new(int argc, char *argv[]) {
	Backgammon *bg;
	GtkBuilder *builder;
	GtkCssProvider *css_provider;
	GdkScreen *screen;

	bg = (Backgammon *)g_malloc(sizeof(Backgammon));
	bg->player[0].name = g_string_new("");
	bg->player[1].name = g_string_new("");

	gtk_init(&argc, &argv);

	css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(css_provider, "ui/styles.css", NULL);

	screen = gdk_screen_get_default();
	gtk_style_context_add_provider_for_screen(screen,
		GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "ui/main-window.glade", NULL);

	bg->window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(builder, "main-window"));
	g_signal_connect(bg->window, "destroy", G_CALLBACK(bg_on_window_destroyed), bg);

	bg->turn_label = GTK_LABEL(gtk_builder_get_object(builder, "turn-label"));
	gtk_label_set_text(bg->turn_label, "<F2> Comenzar");
	bg->action_label = GTK_LABEL(gtk_builder_get_object(builder, "action-label"));
	gtk_label_set_text(bg->action_label, "");

	bg->end_turn_button = GTK_BUTTON(gtk_builder_get_object(builder, "end-turn-button"));
	gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);

	// Players information
	bg->player_name_label[0] = GTK_LABEL(gtk_builder_get_object(builder, "pl1-name-label"));
	bg->player_name_label[1] = GTK_LABEL(gtk_builder_get_object(builder, "pl2-name-label"));

	bg->steps_label[0] = GTK_LABEL(gtk_builder_get_object(builder, "pl1-steps-label"));
	bg->steps_label[1] = GTK_LABEL(gtk_builder_get_object(builder, "pl2-steps-label"));

	bg->score_label[0] = GTK_LABEL(gtk_builder_get_object(builder, "pl1-score-label"));
	bg->score_label[1] = GTK_LABEL(gtk_builder_get_object(builder, "pl2-score-label"));

	g_signal_connect(
		gtk_builder_get_object(builder, "new-game-menu-item"),
		"activate",
		G_CALLBACK(new_game_menu_item_activate), bg
	);

	g_signal_connect(
		gtk_builder_get_object(builder, "quit-menu-item"),
		"activate",
		G_CALLBACK(gtk_main_quit), NULL
	);

	g_signal_connect(
		bg->end_turn_button,
		"clicked",
		G_CALLBACK(end_turn_button_clicked), bg
	);

	bg->board = board_new(builder, bg);

	g_object_unref(builder);

	bg->player_turn = -1;
	bg->status = S_NOT_PLAYING;

	return bg;
}

/**
 * @brief Runs Backgammon.
 * 
 * @param bg Backgammon instance
 */
void bg_run(Backgammon *bg) {
	gtk_widget_show_all(GTK_WIDGET(bg->window));
	gtk_main();
}

/**
 * @brief Frees the Backgammon instance.
 * Releases Backgammon and Board instances.
 * Stops the GTK application.
 * 
 * @param bg Backgammon instance
 */
void bg_free(Backgammon *bg) {
	board_free(bg->board);
	g_string_free(bg->player[0].name, TRUE);
	g_string_free(bg->player[1].name, TRUE);
	g_free(bg);
	gtk_main_quit();
}

/**
 * @brief Calls the next step. If the game is not in progress, it starts.
 * Then, the play_func associated with the current player is called.
 * 
 * @param bg Backgammon instance
 */
void bg_next_step(Backgammon *bg) {

	if (bg->status == S_NOT_PLAYING) {
		bg->player_turn = -1;
		bg_next_turn(bg);
	}

	player_update(bg);

	if (bg_current_player(bg)->ia) {
		g_timeout_add(2000, ia_delayed_func, bg);
	} else bg_current_player(bg)->play_func(bg);
}

/**
 * @brief Switches to the next player's turn.
 * Updates the status bar.
 * Sets the game state to S_ROLL_DICE.
 * 
 * @param bg Backgammon instance
 */
void bg_next_turn(Backgammon *bg) {
	GString *str;

	bg->player_turn++;
	if (bg->player_turn >= 2) bg->player_turn = 0;

	str = g_string_new("Turno de\n");
	str = g_string_append(str, bg_current_player(bg)->name->str);
	gtk_label_set_text(bg->turn_label, str->str);
	g_string_free(str, TRUE);

	bg->status = S_ROLL_DICE;
}

/**
 * @brief Returns the current player.
 * 
 * @param bg Backgammon instance
 * @return Player* instance of the current player
 */
Player *bg_current_player(Backgammon *bg) {
	return &bg->player[bg->player_turn];
}

/**
 * @brief Returns the opponent of the current player.
 * 
 * @param bg Backgammon instance
 * @return Player* instance of the opponent
 */
Player *bg_opponent(Backgammon *bg) {
	return &bg->player[!bg->player_turn];
}

/**
 * @brief Returns the player who represents the piece(s) at the indicated position.
 * The value of the position represents the player according to the direction:
 * v < 0: clockwise direction.
 * v > 0: counterclockwise direction.
 * The absolute value of the value is the number of pieces at that position.
 * 
 * @param bg Backgammon instance
 * @param data value of the position
 * @return Player* instance of the player
 */
Player *bg_player_by_data(Backgammon *bg, gint data) {
	Player *player;
	player = bg_current_player(bg);
	if (player->direction * data > 0) return player;

	return bg_opponent(bg);
}

/**
 * @brief Checks if all the pieces of the current player are in their territory.
 * 
 * @param bg Backgammon instance
 * @return gboolean True if all the pieces are in the territory of the current player.
*/
gboolean bg_all_pieces_in_territory(Backgammon *bg) {
	Player *cplayer;
	Board *board;
	guint i, start, end;

	cplayer = bg_current_player(bg);
	board = bg->board;

	if (cplayer->direction == -1) {
		if (board->prison[1]) return FALSE;
		start = 6; end = 23;
	} else {
		if (board->prison[0]) return FALSE;
		start = 0; end = 18;
	}
	
	for (i = start; i < end; i ++) {
		if (board->places[i].data * cplayer->direction > 0) return FALSE;
	}

	return TRUE;
}
