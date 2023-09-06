/**
 * @file click.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of click.h
 * @date 2023-09-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <click.h>

#include <backgammon.h>
#include <dice.h>
#include <checker.h>
#include <board.h>

/**
 * @brief Occurs when a click is made on the dice set.
 * @param bg Backgammon instance
 */
void dice_click(Backgammon *bg);

/**
 * @brief Occurs when a click is made on a place on the board.
 * 
 * @param bg Backgammon instance
 * @param place Target place
 */
void place_click(Backgammon *bg, Place *place);

/**
 * @brief Occurs when a click is made on a prison.
 * 
 * @param bg Backgammon instance
 * @param prison Target prison
 */
void prison_click(Backgammon *bg, gint prison);

/**
 * @brief Occurs when a click is made on the board.
 * Checks and calls functions for places, dice set, and prison clicked.
 * 
 * @param drw DrawingArea component
 * @param event Mouse button event
 * @param data User data (Backgammon)
 * @return gboolean TRUE if drawing is successful
 */
gboolean board_on_click(GtkDrawingArea *drw, GdkEventButton *event, gpointer data) {
	gdouble x, y, px, py, w, h;
	gint i;
	Backgammon *bg = (Backgammon *) data;

	w = (gdouble) gtk_widget_get_allocated_width(GTK_WIDGET(drw));
	h = (gdouble) gtk_widget_get_allocated_height(GTK_WIDGET(drw));

	// Aspect ratio
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
		if (bg->board->enable_places) {
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
	}

	return TRUE;
}

/**
 * @brief Occurs when a click is made on the dice set.
 * On dice click, changes the game state to MOVE PIECES (Human player).
 * @param bg Backgammon instance
 */
void dice_click(Backgammon *bg) {
	dice_roll(bg->board->dice, bg->board->consumed_dice);
	board_redraw(bg->board);

	bg->status = S_MOVE_PIECES;
	bg_next_step(bg);
}

/**
 * @brief Occurs when a click is made on a place on the board.
 * 
 * @param bg Backgammon instance
 * @param place Target place
 */
void place_click(Backgammon *bg, Place *place) {
	Board *b = bg->board;
	gint value;

	// If the selected place has a mark
	if (place->mark) {
		// If no place is currently selected, move from prison
		if (b->selected == -1) bg_move_from_prison(bg, place->id);
		else bg_move_piece(bg, b->selected, place->id);
		b->selected = -1;
		b->prison_sel = -1;
		board_clear_marks(b);

		// Check if there are still valid moves
		if (!bg_player_can_move(bg)) {
			bg->status = S_END_TURN;
			bg_next_step(bg);
		}
	} else { // If the selected place is not marked
		// Clear other marks
		board_clear_marks(b);

		// No pieces on the selected place
		if (!place->data) {
			// Deselect
			b->selected = -1;
			b->prison_sel = -1;
			board_redraw(b);
			return;
		}

		// If it's the opponent's pieces ...
		if (place->data * bg_current_player(bg)->direction < 0) {
			// Deselect
			b->selected = -1;
			b->prison_sel = -1;
			board_redraw(b);
			return;
		}

		// If there are pieces in the opponent's prison ...
		if (bg_current_player(bg)->direction == -1)
			value = b->prison[1];
		else
			value = b->prison[0];

		if (value) {
			b->selected = -1;
			b->prison_sel = -1;
			board_redraw(b);
			return;
		}

		// Select the place
		b->selected = place->id;

		// Check valid destinations based on the dice roll
		check_selection(bg);
	}

	board_redraw(b);
}

/**
 * @brief Occurs when a click is made on a prison.
 * 
 * @param bg Backgammon instance
 * @param prison Target prison
 */
void prison_click(Backgammon *bg, gint prison) {
	bg->board->selected = -1;

	if (bg->board->prison[prison] * bg_current_player(bg)->direction <= 0) {
		board_clear_marks(bg->board);
	} else {
		bg->board->prison_sel = prison;
		check_selection(bg);
	}

	board_redraw(bg->board);
}
