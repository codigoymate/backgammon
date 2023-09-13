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
 * @brief Checks if the dice value allows moving the piece to the goal.
 * Put a mark to current goal if checks.
 * @param bg Backgammon instance.
 * @param dice_value The value of the dice.
 */
void check_goal_selection(Backgammon *bg, gint dice_value) {
	gint selected;
	selected = bg->board->selected;

	if (bg_current_player(bg)->direction == -1) {
		if (selected < 6) {
			if (dice_value - selected == 1) bg->board->goal[0].mark = TRUE;
		}
	} else {
		if (selected > 17) {
			if (dice_value + selected == 24) bg->board->goal[1].mark = TRUE;
		}
	}
}
