#include <click.h>

#include <backgammon.h>
#include <dice.h>
#include <checker.h>
#include <board.h>

void dice_click(Backgammon *bg);
void place_click(Backgammon *bg, Place *place);
void prison_click(Backgammon *bg, gint prison);

gboolean board_on_click(GtkDrawingArea *drw, GdkEventButton *event, gpointer data) {
	gdouble x, y, px, py, w, h;
	gint i;
	Backgammon *bg = (Backgammon *) data;

	w = (gdouble) gtk_widget_get_allocated_width(GTK_WIDGET(drw));
	h = (gdouble) gtk_widget_get_allocated_height(GTK_WIDGET(drw));

	// Relación de aspecto
	if (h > w * BOARD_RATIO) {
		h = w * BOARD_RATIO;
	} else {
		w = h * (1.0 / BOARD_RATIO);
	}

	if (event->type == GDK_BUTTON_PRESS && event->button == 1) {

		x = event->x / w;
		y = event->y / h;

		// Dice
		if (bg->board->enable_dice)
			if (x > 0.6 && x < 0.6 + 0.289 && y > 0.436 && y < 0.436 + 0.12) {
				dice_click(bg);
			}

		// Places
		if (bg->board->enable_places)
			for (i = 0; i < 24; i ++) {
				px = bg->board->places[i].x;
				py = bg->board->places[i].y;
				if (x > px && x < px + PLACE_SIZE &&
					y > py && y < py + TRIANGLE_HEIGHT) {
						place_click(bg, &bg->board->places[i]);
						break;
					}
			}

		// Prison
		if (x > PLACE_SIZE * 6 && x < PLACE_SIZE * 6 + PLACE_SIZE) {
			if (y > 0.21 && y < 0.21 + PLACE_SIZE * 2) {
				prison_click(bg, 0);
			}
			if (y > 0.66 && y < 0.66 + PLACE_SIZE * 2) {
				prison_click(bg, 1);
			}
		}
	}

	return TRUE;
}

void dice_click(Backgammon *bg) {
	dice_roll(bg->board->dice, bg->board->consumed_dice);
	board_redraw(bg->board);

	bg->status = S_MOVE_PIECES;
	bg_next_step(bg);
}

void place_click(Backgammon *bg, Place *place) {
	
	Board *b = bg->board;

	// Si el lugar a seleccionar tiene una marca
	if (place->mark) {
		bg_move_piece(bg, b->selected, place->id);
		b->selected = -1;
		board_clear_marks(b);

		// Comprueba si sigue habiendo movimientos
		if (!bg_player_can_move(bg)) {
			/*backgammon_next_turn(bg);*/
			bg->status = S_END_TURN;
			bg_next_step(bg);

		}
	} else { // Si el lugar a seleccionar no está marcado
		// Borra el resto de las marcas
		board_clear_marks(b);

		// No hay fichas
		if (!place->data) {
			// Deselecciona
			b->selected = -1;
			board_redraw(b);
			return ;
		}

		// Si son las fichas de otro jugador ...
		if (place->data * bg_current_player(bg)->direction < 0) {
			// Deselecciona
			b->selected = -1;
			board_redraw(b);
			return;
		}

		// Selecciona el lugar
		b->selected = place->id;

		// Chequea los destinos según el resultado de los dados
		check_selection(bg);

	}

	board_redraw(b);

}

void prison_click(Backgammon *bg, gint prison) {
	g_print("prison %i\n", prison);
}
