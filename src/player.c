#include <player.h>

#include <backgammon.h>

void human_play_func(void *bgp) {
	Backgammon *bg = (Backgammon *) bgp;
	switch(bg->status) {
	case S_ROLL_DICE:
		gtk_label_set_text(bg->action_label, "Lanzar los dados");
	case S_MOVE_PIECES:
	case S_END_TURN:
		
	}
}

void ia_play_func(void *bg) {

}
