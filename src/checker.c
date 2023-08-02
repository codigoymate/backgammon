#include <checker.h>

#include <board.h>

void check_selection(Backgammon *bg) {
	guint i;
	gint dice_value, destiny, value;

	if (bg->board->selected == -1) return ;

	board_clear_marks(bg->board);

	for (i = 0; i < 2; i ++) {
		dice_value = bg->board->dice[i];
		destiny = bg->board->selected + dice_value * (bg->player_turn ? -1 : 1);
		if (destiny < 0 || destiny > 23) continue;

		// Si el destino tiene fichas de oponente
		if (bg->board->places[destiny].data * backgammon_current_player(bg)->piece < 0) {
			value = bg->board->places[destiny].data;
			value *= value < 0 ? -1 : 1;

			// Si hay dos o mas fichas de oponente
			if (value != 1) continue;
		}

		bg->board->places[destiny].mark = TRUE;

	}
	
}
