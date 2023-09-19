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
	}
	if (bg->status == S_MOVE_PIECES) {

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
					ia_play_func(bg);
					break;
				}
				i ++;
			}
		}
	}
	if (bg->status == S_END_TURN) {
		gtk_label_set_text(bg->action_label, "Turno finalizado");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), TRUE);
	}
}
