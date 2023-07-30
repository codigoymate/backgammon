#include <player.h>

#include <backgammon.h>

void human_play_func(void *bgp) {
	Backgammon *bg = (Backgammon *) bgp;
	switch(bg->status) {
	case S_ROLL_DICE:
		bg->board->enable_dice = TRUE;
		bg->board->enable_places = FALSE;
		gtk_label_set_text(bg->action_label, "Lanzar los dados");
		break;
	case S_MOVE_PIECES:
		bg->board->enable_dice = FALSE;
		bg->board->enable_places = TRUE;
		gtk_label_set_text(bg->action_label, "Mover fichas");
		break;
	case S_END_TURN:
		break;
	}
}

void ia_play_func(void *bg) {

}
