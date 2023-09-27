/**
 * @file player.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of player.h
 * @date 2023-09-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <player.h>

#include <backgammon.h>
#include <movement.h>
#include <dice.h>

/**
 * @brief Update player information:
 * name, step count, and score
 * 
 * @param bg Backgammon instance
 * @param player player instance
 */
void player_update(void *bgp) {
	Backgammon *bg = (Backgammon *) bgp;
	gchar *number;
	Player *player1;
	Player *player2;

	if (bg->player[0].direction == -1) {
		player1 = &bg->player[0];
		player2 = &bg->player[1];
	} else {
		player1 = &bg->player[1];
		player2 = &bg->player[0];
	}

	// Player name
	gtk_label_set_text(bg->player_name_label[0], player1->name->str);
	gtk_label_set_text(bg->player_name_label[1], player2->name->str);

	// Steps
	number = g_strdup_printf("%i pasos", player_count_steps(bg, player1));
	gtk_label_set_text(bg->steps_label[0], number);
	g_free(number);
	number = g_strdup_printf("%i pasos", player_count_steps(bg, player2));
	gtk_label_set_text(bg->steps_label[1], number);
	g_free(number);

	// Score
	number = g_strdup_printf("%i puntos", player1->score);
	gtk_label_set_text(bg->score_label[0], number);
	g_free(number);
	number = g_strdup_printf("%i puntos", player2->score);
	gtk_label_set_text(bg->score_label[1], number);
	g_free(number);
}

/**
 * @brief Counts the number of steps the player needs to win
 * 
 * @param bg Instance of Backgammon
 * @param player Target player
 * @return guint number of remaining steps to win
 */
guint player_count_steps(void *bgp, Player *player) {
	Backgammon *bg;
	gint i, j, sum, val;
	bg = (Backgammon *) bgp;

	sum = 0;

	// Prison
	val = bg->board->prison[player->direction == -1 ? 1 : 0];
	if (val < 0) val *= -1;
	sum += val * 24;

	if (player->direction == -1) {
		for (i = 0; i < 24; i ++) {
			val = bg->board->places[i].data;
			if (val * player->direction < 0) continue;

			if (val < 0) val *= -1;
			sum += val * (i + 1);
		}
	} else {
		i = 0;
		for (j = 23; j >= 0; j --) {
			val = bg->board->places[j].data;
			if (val * player->direction < 0) { i ++; continue; }

			if (val < 0) val *= -1;
			sum += val * (i + 1);
			i ++;
		}
	}
	return sum;
}

/**
 * @brief Function assigned to a player to configure them as a human player.
 * 
 * @param bgp Backgammon instance
 */
void human_play_func(void *bgp) {
	Backgammon *bg = (Backgammon *) bgp;

	if (bg->status == S_ROLL_DICE) {
		bg->board->enable_dice = TRUE;
		bg->board->enable_places = FALSE;
		gtk_label_set_text(bg->action_label, "Lanzar dados");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);
	}
	if (bg->status == S_MOVE_PIECES) {

		// Scan possible movements
		scan_movements(bg);

		// If the player can't move
		if (bg->board->movements == NULL) {
			bg->status = S_END_TURN;
		}

#ifdef BG_DEBUG
		g_print("Movements: %u\n\n", g_list_length(bg->board->movements));
#endif

		bg->board->enable_dice = FALSE;
		bg->board->enable_places = TRUE;
		gtk_label_set_text(bg->action_label, "Mover fichas");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);
	}
	if (bg->status == S_END_TURN) {
		bg->board->enable_dice = FALSE;
		bg->board->enable_places = FALSE;
		gtk_label_set_text(bg->action_label, "Turno finalizado");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), TRUE);
	}
}

/**
 * @brief Function assigned to a player to configure them as an artificial intelligence.
 * 
 * @param bgp Backgammon instance
 */
void ia_play_func(void *bgp) {
	guint choice, i;
	GList *iter;
	Backgammon *bg = (Backgammon *) bgp;

	bg->board->enable_dice = FALSE;
	bg->board->enable_places = FALSE;

	if (bg->status == S_ROLL_DICE) {
		gtk_label_set_text(bg->action_label, "Lanzar dados");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);
		
		dice_roll(bg->board->dice, bg->board->consumed_dice);

		bg->status = S_MOVE_PIECES;

		board_redraw(bg->board);
		
		bg_next_step(bg);
	} else if (bg->status == S_MOVE_PIECES) {

		// Scan possible movements
		scan_movements(bg);

		// If the player can't move
		if (bg->board->movements == NULL) {
			bg->status = S_END_TURN;
		} else {

#ifdef BG_DEBUG
			g_print("Movements: %u\n\n", g_list_length(bg->board->movements));
#endif

			gtk_label_set_text(bg->action_label, "Mover fichas");
			gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);

			choice = g_random_int_range(0, g_list_length(bg->board->movements));
			i = 0;
			for (iter = bg->board->movements; iter; iter = iter->next) {
				if (choice == i) {
					move_piece(bg, (Movement *) iter->data);
					board_redraw(bg->board);
					break;
				}
				i ++;
			}
		}
		bg_next_step(bg);
	} else if (bg->status == S_END_TURN) {
		gtk_label_set_text(bg->action_label, "Turno finalizado");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), TRUE);
	}
}

/**
 * @brief Function called during the delay generated for AI.
 * 
 * @param data Instance of Backgammon
 * @return gboolean True to remove the timer
 */
gboolean ia_delayed_func(gpointer data) {
	Backgammon *bg = (Backgammon *) data;

	bg_current_player(bg)->play_func(bg);

	return G_SOURCE_REMOVE;
}
