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
#include <utils.h>

#include <player.h>

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
	Backgammon *bg = (Backgammon *)data;

	if (bg->status != S_NOT_PLAYING) {
		if (!question(GTK_WIDGET(bg->window), "End the current game?")) return;
	}

	bg->player[0].play_func = human_play_func;
	bg->player[0].name = g_string_new("Human");
	bg->player[0].piece = BLACK;
	bg->player[0].direction = 1;
	bg->player[1].play_func = human_play_func;
	bg->player[1].name = g_string_new("AI");
	bg->player[1].piece = WHITE;
	bg->player[1].direction = -1;

	board_init(bg->board);
	board_redraw(bg->board);

	bg_next_step(bg);
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

	bg_current_player(bg)->play_func(bg);
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
 * @brief Moves a game piece.
 * There must be pieces at the source position (from the current player).
 * Removes one piece from the source and adds it to the destination.
 * If the destination has an opponent's piece, the current player's prison captures an opponent's piece.
 * 
 * @param bg Backgammon instance
 * @param source starting position
 * @param destiny destination position
 */
void bg_move_piece(Backgammon *bg, gint source, gint destiny) {
	gint len, i;
	// Distance traveled
	len = destiny - source;
	if (len < 0) len *= -1;
	// Deactivate the die according to the distance
	for (i = 0; i < (bg->board->dice[0] == bg->board->dice[1] ? 4 : 2); i++) {
		if (bg->board->consumed_dice[i]) continue;
		if (bg->board->dice[i % 2] == len) {
			bg->board->consumed_dice[i] = TRUE;
			break;
		}
	}

	// If the destination is not an enemy
	if (bg->board->places[destiny].data * bg_current_player(bg)->direction >= 0) {
		// Remove from source
		bg->board->places[source].data -= bg_current_player(bg)->direction;

		bg->board->places[destiny].data += bg_current_player(bg)->direction;
	} else {
		// If it's an enemy...

		// Put the piece in prison
		bg->board->prison[bg_current_player(bg)->direction == -1 ? 0 : 1] +=
				bg_opponent(bg)->direction;

		// Replace the current piece
		bg->board->places[destiny].data *= -1;
	}
}

/**
 * @brief Moves a game piece from selected prison.
 * 
 * There must be pieces at the prison (from the current player).
 * Removes one piece from the prison and adds it to the destination.
 * If the destination has an opponent's piece, the current player's prison captures an opponent's piece.
 * 
 * @param bg Backgammon instance
 * @param destiny Destination position
 */
void bg_move_from_prison(Backgammon *bg, gint destiny) {
	gint len, i;
	// Distance traveled
	if (bg_current_player(bg)->direction == 1) len = destiny + 1;
	else len = 23 - destiny;

	// Deactivate the die according to the distance
	for (i = 0; i < (bg->board->dice[0] == bg->board->dice[1] ? 4 : 2); i++) {
		if (bg->board->consumed_dice[i]) continue;
		if (bg->board->dice[i % 2] == len) {
			bg->board->consumed_dice[i] = TRUE;
			break;
		}
	}

	// If the destination is not an enemy
	if (bg->board->places[destiny].data * bg_current_player(bg)->direction >= 0) {
		// Remove from source

		bg->board->prison[bg_current_player(bg)->direction == 1 ? 0 : 1] 
					-= bg_current_player(bg)->direction;

		bg->board->places[destiny].data += bg_current_player(bg)->direction;
	} else {
		// If it's an enemy...

		// Put the piece in prison
		bg->board->prison[bg_current_player(bg)->direction == -1 ? 0 : 1] +=
				bg_opponent(bg)->direction;

		// Replace the current piece
		bg->board->places[destiny].data *= -1;
	}
}

/**
 * @brief Indicates if the current player can make a move.
 * 
 * @param bg Backgammon instance
 * @return gboolean true if the player can move
 */
gboolean bg_player_can_move(Backgammon *bg) {
	gint i;
	Board *b;

	b = bg->board;

	for (i = 0; i < (b->dice[0] == b->dice[1] ? 4 : 2); i++) {
		if (!b->consumed_dice[i]) return TRUE;
	}

	return FALSE;
}
