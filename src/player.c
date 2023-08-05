#include <player.h>

#include <backgammon.h>

void human_play_func(void *bgp) {
	Backgammon *bg = (Backgammon *) bgp;
	switch(bg->status) {
	case S_ROLL_DICE:
		bg->board->enable_dice = TRUE;
		bg->board->enable_places = FALSE;
		gtk_label_set_text(bg->action_label, "Lanzar los dados");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);
		break;
	case S_MOVE_PIECES:
		bg->board->enable_dice = FALSE;
		bg->board->enable_places = TRUE;
		gtk_label_set_text(bg->action_label, "Mover fichas");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);
		break;
	case S_END_TURN:
		bg->board->enable_dice = FALSE;
		bg->board->enable_places = FALSE;
		gtk_label_set_text(bg->action_label, "Turno finalizado");
		gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), TRUE);
		break;
	}
}

void ia_play_func(void *bg) {

}
