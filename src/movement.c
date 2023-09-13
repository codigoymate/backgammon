/**
 * @file movement.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of movement.h
 * @date 2023-09-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <movement.h>

/**
 * @brief Creates a new instance of Movement
 * 
 * @param src source
 * @param dest destination
 * @param prison_src source as prison
 * @param goal_dest destination as goal
 * @return Movement* a new instance of Movement
 */
Movement *movement_new(gint src, gint dest, gboolean prison_src, gboolean goal_dest) {
	Movement *m = (Movement *) g_malloc(sizeof(Movement));
	m->dest = dest;
	m->src = src;
	m->prison_src = prison_src;
	m->goal_dest = goal_dest;
	return m;
}

/**
 * @brief Searches for possible movements for the current player
 * from prison
 * @param bg Backgammon instance
 * @return GList* list with possible movements
 */
GList *scan_prison_movements(Backgammon *bg) {
	guint i, dice_value;
	gint destiny, value;
	GList *list = NULL;

	for (i = 0; i < (bg->board->dice[0] == bg->board->dice[1] ? 4 : 2); i++) {
		if (bg->board->consumed_dice[i]) continue;
		dice_value = bg->board->dice[i % 2];

		if (bg_current_player(bg)->direction == -1)
			destiny = 24 - dice_value;
		else destiny = dice_value - 1;

		// If the destination has opponent's pieces
		if (bg->board->places[destiny].data * bg_current_player(bg)->direction < 0) {
			value = bg->board->places[destiny].data;
			value *= value < 0 ? -1 : 1;

			// If there are two or more opponent's pieces
			if (value != 1) continue;
		}

		list = g_list_append(list, movement_new(-1, destiny, TRUE, FALSE));
	}

	return list;
}

/**
 * @brief Searches for possible movements for the current player
 * in the case that there is a possibility to move a piece
 * to the goal.
 * 
 * @param bg Backgammon instance
 * @param list current list of movements
 * @param src source of the piece on the board
 * @param dice_value value of the dice
 * @return GList* the current list
 */
GList *scan_goal_movement(Backgammon *bg, GList *list, guint src, guint dice_value) {
	if (bg_current_player(bg)->direction == -1) {
		if (src < 6) {
			if (dice_value - src == 1)
				list = g_list_append(list, movement_new(src, -1, FALSE, TRUE));
		}
	} else {
		if (src > 17) {
			if (dice_value + src == 24)
				list = g_list_append(list, movement_new(src, -1, FALSE, TRUE));
		}
	}
	return list;
}

/**
 * @brief Searches for possible movements for the current player.
 * Set results in board instance.
 * 
 * @param bg Backgammon instance
 */
void scan_movements(Backgammon *bg) {
	guint i, d, dice_value;
	gint cdir, destiny, value;
	Board *board;

	board = bg->board;
	cdir = bg_current_player(bg)->direction;

	// Clean current movements
	clean_movements(bg);

	// If there are pieces in prison ...
	if (cdir == -1 && board->prison[1]) {
		board->movements = scan_prison_movements(bg);
		return;
	}

	if (cdir == 1 && board->prison[0]) {
		board->movements = scan_prison_movements(bg);
		return;
	}

	for (i = 0; i < 24; i ++) {
		// Check if there are pieces and if they belong to the current player
		if (board->places[i].data * cdir <= 0) continue;

		for (d = 0; d < (board->dice[0] == board->dice[1] ? 4 : 2); d ++) {
			if (board->consumed_dice[d]) continue;

			dice_value = board->dice[d % 2];
			destiny = i + dice_value * cdir;

			// Check goal selection
			if (bg_all_pieces_in_territory(bg)) {
				// All pieces must be in the current territory
				board->movements = scan_goal_movement(bg, board->movements, i, dice_value);
			}

			if (destiny < 0 || destiny > 23) continue;

			// If the destination has opponent's pieces
			if (board->places[destiny].data * cdir < 0) {
				value = board->places[destiny].data;
				value *= value < 0 ? -1 : 1;

				// If there are two or more opponent's pieces
				if (value != 1) continue;
			}

			// Add movement to list
			board->movements = g_list_append(board->movements,
					movement_new(i, destiny, FALSE, FALSE));
		}
	}

}

/**
 * @brief Clean current movements in Board instance.
 * 
 * @param bg Backgammon's instance.
 */
void clean_movements(Backgammon *bg) {
	g_list_foreach(bg->board->movements, (GFunc)g_free, NULL);
	g_list_free(bg->board->movements);
	bg->board->movements = NULL;
}

/**
 * @brief Moves a game piece.
 * There must be pieces at the source position (from the current player).
 * Removes one piece from the source and adds it to the destination.
 * If the destination has an opponent's piece, the current player's prison captures an opponent's piece.
 * 
 * @param bg Backgammon instance
 * @param movement the register movement
 */
void move_piece(Backgammon *bg, Movement *m) {
	gint len, i;

	// Move from any place ...
	if (m->src != -1) {

		// TODO: to goal movement ...

		// Calculate length
		len = m->dest - m->src;
		if (len < 0) len *= -1;

		// Deactivate the die according to the distance
		for (i = 0; i < (bg->board->dice[0] == bg->board->dice[1] ? 4 : 2); i++) {
			if (bg->board->consumed_dice[i]) continue;
			if (bg->board->dice[i % 2] == len) {
				bg->board->consumed_dice[i] = TRUE;
				break;
			}
		}

		// Remove from source
		bg->board->places[m->src].data -= bg_current_player(bg)->direction;

		// If the destination is not an enemy
		if (bg->board->places[m->dest].data * bg_current_player(bg)->direction >= 0) {
			bg->board->places[m->dest].data += bg_current_player(bg)->direction;
		} else {
			// If it's an enemy...

			// Put the piece in prison
			bg->board->prison[bg_current_player(bg)->direction == -1 ? 0 : 1] +=
					bg_opponent(bg)->direction;

			// Replace the current piece
			bg->board->places[m->dest].data *= -1;
		}
	} else { // Move from prison
		// Calculate length
		if (bg_current_player(bg)->direction == 1) len = m->dest + 1;
		else len = 24 - m->dest;

		// Deactivate the die according to the distance
		for (i = 0; i < (bg->board->dice[0] == bg->board->dice[1] ? 4 : 2); i++) {
			if (bg->board->consumed_dice[i]) continue;
			if (bg->board->dice[i % 2] == len) {
				bg->board->consumed_dice[i] = TRUE;
				break;
			}
		}

		// Remove from source (prison)
		bg->board->prison[bg_current_player(bg)->direction == 1 ? 0 : 1] 
					-= bg_current_player(bg)->direction;

		// If the destination is not an enemy
		if (bg->board->places[m->dest].data * bg_current_player(bg)->direction >= 0) {
			bg->board->places[m->dest].data += bg_current_player(bg)->direction;
		} else {
			// If it's an enemy...

			// Put the piece in prison
			bg->board->prison[bg_current_player(bg)->direction == -1 ? 0 : 1] +=
					bg_opponent(bg)->direction;

			// Replace the current piece
			bg->board->places[m->dest].data *= -1;
		}
		
	}

	// Next step
	bg_next_step(bg);
}

