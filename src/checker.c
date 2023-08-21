/**
 * @file checker.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of checker.h
 * @date 2023-08-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <checker.h>

#include <board.h>

/**
 * @brief Searches for possible movements from a selected prison
 * considering the value of the destination and the dice.
 * 
 * @param bg Backgammon instance
 */
void check_prison_selection(Backgammon *bg) {
	guint i, destiny, dice_value;
	gint value;
	if (bg->board->prison_sel != -1) {
		board_clear_marks(bg->board);

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

			bg->board->places[destiny].mark = TRUE;
		}
	}
}

/**
 * @brief Searches for possible movements for the current player.
 * Takes into account whether the destination is a friendly or enemy piece.
 * Also considers consumed movements.
 * 
 * This check is done on a selected position.
 * 
 * @param bg Backgammon instance
 */
void check_selection(Backgammon *bg) {
	guint i;
	gint dice_value, destiny, value;

	if (bg->board->selected == -1) {
		check_prison_selection(bg);
		return;
	}

	board_clear_marks(bg->board);

	for (i = 0; i < (bg->board->dice[0] == bg->board->dice[1] ? 4 : 2); i++) {

		if (bg->board->consumed_dice[i]) continue;

		dice_value = bg->board->dice[i % 2];
		destiny = bg->board->selected + dice_value * bg_current_player(bg)->direction;
		if (destiny < 0 || destiny > 23) continue;

		// If the destination has opponent's pieces
		if (bg->board->places[destiny].data * bg_current_player(bg)->direction < 0) {
			value = bg->board->places[destiny].data;
			value *= value < 0 ? -1 : 1;

			// If there are two or more opponent's pieces
			if (value != 1) continue;
		}

		bg->board->places[destiny].mark = TRUE;

	}
}
