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
#include <board.h>
#include <movement.h>

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
 * @brief Occurs when a click is made on a goal on the board.
 * 
 * @param bg Backgammon instance
 * @param goal Target goal
 */
void goal_click(Backgammon *bg, Place *goal);

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

		// Goal
		for (i = 0; i < 2; i ++) {
			px = bg->board->goal[i].x;
			py = bg->board->goal[i].y;
			if (x > px && x < px + PLACE_SIZE &&
				y > py && y < py + TRIANGLE_HEIGHT) {
					goal_click(bg, &bg->board->goal[i]);
					break;
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
	GList *iter;
	Movement *m;
	guint cdir;

	cdir = bg_current_player(bg)->direction;

	// If the selected place has a mark
	if (place->mark) {
		if (b->selected != -1) {
			// Place origin selected
			for (iter = b->movements; iter; iter = iter->next) {
				m = (Movement *) iter->data;
				if (m->src == b->selected && m->dest == place->id) {
					// Clear selections
					b->prison_sel = -1;
					b->selected = -1;
					board_clear_marks(b);

					// Move piece
					move_piece(bg, m);
					break;
				}
			}
		} else {
			// Prison origin selected
			for (iter = b->movements; iter; iter = iter->next) {
				m = (Movement *) iter->data;
				if (m->prison_src && m->dest == place->id) {
					// Clear selections
					b->prison_sel = -1;
					b->selected = -1;
					board_clear_marks(b);

					// Move piece
					move_piece(bg, m);
					break;
				}
			}
		}
	} else { // If the selected place is not marked

		// Clear all marks
		board_clear_marks(b);

		// Unselect prisons
		b->prison_sel = -1;

		// Select the place
		b->selected = place->id;

		// Find movements by source
		for (iter = b->movements; iter; iter = iter->next) {
			m = (Movement *)iter->data;
			if (m->src == place->id) {
				// Mark the destiny
				if (m->goal_dest) {
					if (cdir == -1) b->goal[0].mark = TRUE;
					else b->goal[1].mark = TRUE;
				} else {
					b->places[m->dest].mark = TRUE;
				}
			}
		}
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
	GList *iter;
	Movement *m;
	bg->board->selected = -1;

	// If not prisoners ...
	if (bg->board->prison[prison] * bg_current_player(bg)->direction <= 0) {
		board_clear_marks(bg->board);
	} else {
		// Select the prison
		bg->board->prison_sel = prison;
		
		// Mark all possible movements
		for (iter = bg->board->movements; iter; iter = iter->next) {
			m = (Movement *) iter->data;
			if (m->prison_src) {
				bg->board->places[m->dest].mark = TRUE;
			}
		}
	}

	board_redraw(bg->board);
}

/**
 * @brief Occurs when a click is made on a goal on the board.
 * 
 * @param bg Backgammon instance
 * @param goal Target goal
 */
void goal_click(Backgammon *bg, Place *goal) {
	GList *iter;
	Movement *m;

	// Goal must be marked
	if (!goal->mark) return ;

	// Find the movements
	for (iter = bg->board->movements; iter; iter = iter->next) {
		m = (Movement *) iter->data;
		if (m->goal_dest) {
			// Clear selections
			bg->board->prison_sel = -1;
			bg->board->selected = -1;
			board_clear_marks(bg->board);

			// Move piece
			move_piece(bg, m);
			break;
		}
	}
	board_redraw(bg->board);
}
