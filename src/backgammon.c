/**
 * @file backgammon.c
 * @brief Implementation of backgammon.h
 * @date 2023-08-14
 * 
 * @author Javier Candales
 * @copyright Copyright (c) 2023
 * 
 */

#include <backgammon.h>
#include <player.h>
#include <new_dialog.h>
#include <utils.h>
#include <results_dialog.h>

#include <undo.h>
#include <double_dice.h>

/**
 * @brief Frees the Backgammon instance.
 * 
 * @param bg Backgammon instance
 */
void bg_free(Backgammon *bg);

/**
 * @brief Checks if either of the players has removed all 15 of their pieces from the board.
 * 
 * @param bg Backgammon instance
 * @return gboolean True when one of the players has removed all their pieces.
 */
gboolean bg_check_end_game(Backgammon *bg);

/**
 * @brief Ends the round or game by determining the winner and the number of points. Calculates the results and displays the results dialog.
 * 
 * @param bg Backgammon instance
 */
void bg_end_game(Backgammon *bg);

/**
 * @brief Determines the points earned by the winner.
 * 1: When the opponent has removed some pieces from the board.
 * 2: (Gammon) When the opponent has not removed any pieces.
 * 3: (Backgammon) When the opponent still has pieces in the winner's territory or in the winner's prison.
 * 
 * @param bg Backgammon instance
 * @param win_dir Direction of the winner
 * @return guint The points earned by the winner
 */
guint get_winner_points(Backgammon *bg, gint win_dir);

/**
 * @brief Occurs when the main window is closed. Frees the Backgammon instance.
 * 
 * @param window Main window instance
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
 * @param menu_item New game menu item
 * @param data Backgammon instance
 */
static void new_game_menu_item_activate(GtkMenuItem *menu_item, gpointer data) {
	new_dialog_show(new_dialog_new((Backgammon *)data));
}

/**
 * @brief Occurs when clicking the "Next Turn" button.
 * Typically associated with the human player. Switches to the next player's turn.
 * 
 * @param button Button instance
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
 * @brief Occurs when clicking the "Undo" button
 * 
 * @param button Button instance
 * @param data Backgammon instance
 */
static void undo_button_clicked(GtkWidget *button, gpointer data) {
	Backgammon *bg;
	bg = (Backgammon *)data;

	undo_restore(bg);
	bg->status = S_MOVE_PIECES;

	board_redraw(bg->board);

	bg_next_step(bg);
}

/**
 * @brief Occurs when clicking the "Double" button
 * 
 * @param button Button instance
 * @param data Backgammon instance
 */
static void double_button_clicked(GtkWidget *button, gpointer bg) {
	double_perform(bg);
}

/**
 * @brief Creates an instance of Backgammon.
 * Initializes the GTK environment.
 * Creates the board.
 * Sets the game state to S_NOT_PLAYING.
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return Backgammon* Backgammon instance
 */
Backgammon *bg_new(int argc, char *argv[]) {
	Backgammon *bg;
	GtkBuilder *builder;
	GtkCssProvider *css_provider;
	GdkScreen *screen;

	bg = (Backgammon *)g_malloc(sizeof(Backgammon));

	// Default names

	bg->player[0].name = g_string_new(g_getenv("USER"));
	bg->player[1].name = randomize_name();

	bg->max_score = 15;

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

	bg->undo_button = GTK_BUTTON(gtk_builder_get_object(builder, "undo-button"));
	gtk_widget_set_sensitive(GTK_WIDGET(bg->undo_button), FALSE);

	bg->double_button = GTK_BUTTON(gtk_builder_get_object(builder, "double-button"));
	gtk_widget_set_sensitive(GTK_WIDGET(bg->double_button), FALSE);

	// Players information
	bg->player_name_label[0] = GTK_LABEL(gtk_builder_get_object(builder, "pl1-name-label"));
	bg->player_name_label[1] = GTK_LABEL(gtk_builder_get_object(builder, "pl2-name-label"));

	bg->steps_label[0] = GTK_LABEL(gtk_builder_get_object(builder, "pl1-steps-label"));
	bg->steps_label[1] = GTK_LABEL(gtk_builder_get_object(builder, "pl2-steps-label"));

	bg->score_label[0] = GTK_LABEL(gtk_builder_get_object(builder, "pl1-score-label"));
	bg->score_label[1] = GTK_LABEL(gtk_builder_get_object(builder, "pl2-score-label"));

	// Double
	bg->double_image[0] = GTK_IMAGE(gtk_builder_get_object(builder, "pl1-double-image"));
	bg->double_image[1] = GTK_IMAGE(gtk_builder_get_object(builder, "pl2-double-image"));

	bg->double_pixbuf[0] = gdk_pixbuf_new_from_file_at_scale("ui/2.png", DDICE_SIZE, DDICE_SIZE, TRUE, NULL);
	bg->double_pixbuf[1] = gdk_pixbuf_new_from_file_at_scale("ui/4.png", DDICE_SIZE, DDICE_SIZE, TRUE, NULL);
	bg->double_pixbuf[2] = gdk_pixbuf_new_from_file_at_scale("ui/8.png", DDICE_SIZE, DDICE_SIZE, TRUE, NULL);
	bg->double_pixbuf[3] = gdk_pixbuf_new_from_file_at_scale("ui/16.png", DDICE_SIZE, DDICE_SIZE, TRUE, NULL);
	bg->double_pixbuf[4] = gdk_pixbuf_new_from_file_at_scale("ui/32.png", DDICE_SIZE, DDICE_SIZE, TRUE, NULL);
	bg->double_pixbuf[5] = gdk_pixbuf_new_from_file_at_scale("ui/64.png", DDICE_SIZE, DDICE_SIZE, TRUE, NULL);

	//gtk_image_set_from_pixbuf(bg->double_image[0], bg->double_pixbuf[0]);

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

	g_signal_connect(
		bg->undo_button,
		"clicked",
		G_CALLBACK(undo_button_clicked), bg
	);

	g_signal_connect(
		bg->double_button,
		"clicked",
		G_CALLBACK(double_button_clicked), bg
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

	if (bg_check_end_game(bg)) {
		bg_end_game(bg);
		return ;
	}

	if (bg_current_player(bg)->ia) {
		g_timeout_add(DELAYED_FUNC_TIMEOUT, ia_delayed_func, bg);
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
 * @return Player* Instance of the current player
 */
Player *bg_current_player(Backgammon *bg) {
	return &bg->player[bg->player_turn];
}

/**
 * @brief Returns the opponent of the current player.
 * 
 * @param bg Backgammon instance
 * @return Player* Instance of the opponent
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
 * @param data Value of the position
 * @return Player* Instance of the player
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

/**
 * @brief Checks if either of the players has removed all 15 of their pieces from the board.
 * 
 * @param bg Backgammon instance
 * @return gboolean True when one of the players has removed all their pieces.
 */
gboolean bg_check_end_game(Backgammon *bg) {
	return bg->board->goal[0].data == 15 || bg->board->goal[0].data == -15 ||
		bg->board->goal[1].data == 15 || bg->board->goal[1].data == -15;
}

/**
 * @brief Ends the round or game by determining the winner and the number of points. Calculates the results and displays the results dialog.
 * 
 * @param bg Backgammon instance
 */
void bg_end_game(Backgammon *bg) {
	Player *winner;
	guint score;
	ResultsDialog *dialog;

	if (bg->status == S_END_ROUND) return;

	bg->status = S_END_ROUND;

	if (bg->board->goal[0].data == -15 || bg->board->goal[0].data == 15) {
		winner = bg->player[0].direction == -1 ?
					&bg->player[0] : &bg->player[1];
	} else {
		winner = bg->player[0].direction == 1 ?
					&bg->player[0] : &bg->player[1];
	}

	// Get points: 1 normal; 2 gammon; 3 backgammon
	score = get_winner_points(bg, winner->direction);

	// Multiply for double dice
	score *= bg->player[0].double_points;
	score *= bg->player[1].double_points;

	// Add socore to winner
	winner->score += score;

#ifdef BG_DEBUG
	g_print("Winner: %s\n", winner->name->str);
	g_print("Score: %u\n\n", score);
#endif

	dialog = results_dialog_new(bg, winner, score);
	results_dialog_show(dialog);
}

/**
 * @brief Determines the points earned by the winner.
 * 1: When the opponent has removed some pieces from the board.
 * 2: (Gammon) When the opponent has not removed any pieces.
 * 3: (Backgammon) When the opponent still has pieces in the winner's territory or in the winner's prison.
 * 
 * @param bg Backgammon instance
 * @param win_dir Direction of the winner
 * @return guint The points earned by the winner
 */
guint get_winner_points(Backgammon *bg, gint win_dir) {
	Board *b;
	guint points, i;
	b = bg->board;

	points = 1;

	// Check Gammon
	if (win_dir == -1) {
		if (b->goal[1].data) return points;
	} else {
		if (b->goal[0].data) return points;
	}

	points ++;

	// Check backgammon
	if (win_dir == -1) {
		if (b->prison[0]) return points + 1;
		for (i = 0; i < 6; i ++) {
			if (b->places[i].data * win_dir < 0) return points + 1;
		}
	} else {
		if (b->prison[1]) return points + 1;
		for (i = 18; i < 24; i ++) {
			if (b->places[i].data * win_dir < 0) return points + 1;
		}
	}

	return points;
}
