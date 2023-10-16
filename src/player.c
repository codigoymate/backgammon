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
#include <utils.h>
#include <double_dice.h>

#include <libintl.h>

#define _(str)	gettext(str)

/**
 * @brief Human response to the opponent's request to double points
 * 
 * @param bg backgammon instance
 * @return gboolean TRUE: Positive response
 */
gboolean human_double_request(Backgammon *bg);

/**
 * @brief AI response to the opponent's request to double points
 * 
 * @param bg backgammon instance
 * @return gboolean TRUE: Positive response
 */
gboolean ia_double_request(Backgammon *bg);

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
	number = g_strdup_printf(_("Steps: %i"), player_count_steps(bg, player1));
	gtk_label_set_text(bg->steps_label[0], number);
	g_free(number);
	number = g_strdup_printf(_("Steps: %i"), player_count_steps(bg, player2));
	gtk_label_set_text(bg->steps_label[1], number);
	g_free(number);

	// Score
	number = g_strdup_printf(_("%i points"), player1->score);
	gtk_label_set_text(bg->score_label[0], number);
	g_free(number);
	number = g_strdup_printf(_("%i points"), player2->score);
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
 * @param bg Backgammon instance
 * @param double_request TRUE when the player is asked to double
 * @return boolean FALSE if opponent rejects the double
 */
gboolean human_play_func(void *bgp, gboolean double_request) {
	Backgammon *bg = (Backgammon *) bgp;

	if (double_request) return human_double_request(bg);

	if (bg->status == S_ROLL_DICE) {
		bg->board->enable_dice = TRUE;
		bg->board->enable_places = FALSE;
		gtk_label_set_text(bg->action_label, _("Throw dice"));
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(bg->undo_button), FALSE);

		if (bg_current_player(bg)->double_points == 1)
			gtk_widget_set_sensitive(GTK_WIDGET(bg->double_button), TRUE);
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
		gtk_label_set_text(bg->action_label, _("Move pieces"));
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(bg->double_button), FALSE);
	}
	if (bg->status == S_END_TURN) {
		bg->board->enable_dice = FALSE;
		bg->board->enable_places = FALSE;
		gtk_label_set_text(bg->action_label, _("End turn"));
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(bg->undo_button), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(bg->double_button), FALSE);
	}

	return TRUE;
}

/**
 * @brief Function assigned to a player to configure them as an artificial intelligence.
 * 
 * @param bg Backgammon instance
 * @param double_request TRUE when the player is asked to double
 * @return boolean FALSE if opponent rejects the double
 */
gboolean ia_play_func(void *bgp, gboolean double_request) {
	guint choice, i;
	GList *iter;
	gint player_steps, opponent_steps, diff;
	Backgammon *bg = (Backgammon *) bgp;

	if (double_request) return ia_double_request(bg);

	bg->board->enable_dice = FALSE;
	bg->board->enable_places = FALSE;

	if (bg->status == S_ROLL_DICE) {
		gtk_label_set_text(bg->action_label, _("Throw dice"));
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);

		// Double perform
		player_steps = player_count_steps(bg, bg_current_player(bg));
		opponent_steps = player_count_steps(bg, bg_opponent(bg));

		if (opponent_steps <= player_steps) {

			diff = player_steps - opponent_steps;

			if (diff <= 5)
				if (g_random_double_range(0.0, 1.0) < 0.05) double_perform(bg);

			if (diff > 5 && diff <= 30)
				if (g_random_double_range(0.0, 1.0) < 0.2) double_perform(bg);

			if (g_random_double_range(0.0, 1.0) < 0.3) double_perform(bg);

		}
		
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

			gtk_label_set_text(bg->action_label, _("Move pieces"));
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
		gtk_label_set_text(bg->action_label, _("End turn"));
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), TRUE);
	}

	return TRUE;
}

/**
 * @brief Function called during the delay generated for AI.
 * 
 * @param data Instance of Backgammon
 * @return gboolean True to remove the timer
 */
gboolean ia_delayed_func(gpointer data) {
	Backgammon *bg = (Backgammon *) data;

	bg_current_player(bg)->play_func(bg, FALSE);

	return G_SOURCE_REMOVE;
}

/**
 * @brief Human response to the opponent's request to double points
 * 
 * @param bg backgammon instance
 * @return gboolean TRUE: Positive response
 */
gboolean human_double_request(Backgammon *bg) {
	GString *msg;
	gboolean result;

	msg = g_string_new("");
	g_string_append_printf(msg, _("%s double score. Accept?"),
			bg_current_player(bg)->name->str);

	result = question(GTK_WIDGET(bg->window), msg->str);

	g_string_free(msg, TRUE);

	return result;
}

/**
 * @brief AI response to the opponent's request to double points
 * 
 * @param bg backgammon instance
 * @return gboolean TRUE: Positive response
 */
gboolean ia_double_request(Backgammon *bg) {
	gint player_steps, opponent_steps, diff;
	player_steps = player_count_steps(bg, bg_opponent(bg));
	opponent_steps = player_count_steps(bg, bg_current_player(bg));

	diff = player_steps - opponent_steps;

	if (diff < -20) return g_random_double_range(0.0, 1.0) < 0.1;
	
	if (diff >= -20 && diff < 10)
		return g_random_double_range(0.0, 1.0) < 0.4;
		

	return g_random_double_range(0.0, 1.0) < 0.9;

}
