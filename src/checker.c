#include <checker.h>

#include <board.h>

void check_selection(Backgammon *bg) {
	guint i;
	gint dice_value, destiny, value;

	if (bg->board->selected == -1) return ;

	board_clear_marks(bg->board);

	for (i = 0; i < (bg->board->dice[0] == bg->board->dice[1] ? 4 : 2); i ++) {

		if (bg->board->consumed_dice[i]) continue;

		dice_value = bg->board->dice[i % 2];
		destiny = bg->board->selected + dice_value * bg_current_player(bg)->direction;
		if (destiny < 0 || destiny > 23) continue;

		// Si el destino tiene fichas de oponente
		if (bg->board->places[destiny].data * bg_current_player(bg)->direction < 0) {
			value = bg->board->places[destiny].data;
			value *= value < 0 ? -1 : 1;

			// Si hay dos o mas fichas de oponente
			if (value != 1) continue;
		}

		bg->board->places[destiny].mark = TRUE;

	}
	
}
