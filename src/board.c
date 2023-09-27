/**
 * @file board.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of board.h
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <board.h>
#include <cairo.h>
#include <dice.h>

#include <backgammon.h>
#include <draw.h>
#include <click.h>

/**
 * @brief Occurs when drawing the board
 * 
 * @param area DrawingArea
 * @param cr Cairo context
 * @param data Backgammon instance
 * @return gboolean TRUE if there were no problems while drawing
 */
static gboolean board_on_draw(GtkWidget *area, cairo_t *cr, gpointer data) {
	gdouble w, h;
	gint i;

	Backgammon *bg;
	Board *board;

	bg = (Backgammon *) data;
	board = bg->board;

	w = gtk_widget_get_allocated_width(area);
	h = gtk_widget_get_allocated_height(area);

	// Aspect ratio
	if (h > w * BOARD_RATIO) {
		h = w * BOARD_RATIO;
	} else {
		w = h * (1.0 / BOARD_RATIO);
	}

	// Background
	COLOR_BACKGROUND(cr);
	cairo_paint(cr);

	// Triangles, pieces, and marks
	for (i = 0; i < 24; i ++) {
		draw_colored_triangle(cr, board->places[i], i % 2, w, h);
		draw_piece_group(cr, bg, board->places[i], w, h);
		draw_mark(cr, board->places[i], w, h);
	}

	// Bars
	COLOR_BAR(cr);
	cairo_rectangle(cr, w * 0.0714 * 6, 0, w * 0.0714, h);
	cairo_rectangle(cr, w * 0.0714 * 13, 0, w * 0.0714, h);
	cairo_fill(cr);

	// Selection
	if (board->selected != -1) {
		COLOR_SELECTION(cr);
		cairo_rectangle(cr, board->places[board->selected].x * w,
			board->places[board->selected].y * h,
			PLACE_SIZE * w, TRIANGLE_HEIGHT * h);
		cairo_stroke(cr);
	}

	if (board->prison_sel != -1) {
		COLOR_SELECTION(cr);
		if (board->prison_sel == 0)
			cairo_rectangle(cr, PLACE_SIZE * 6 * w,
				0.21 * h, PLACE_SIZE * w, PLACE_SIZE * w);
		else
			cairo_rectangle(cr, PLACE_SIZE * 6 * w,
				0.66 * h, PLACE_SIZE * w, PLACE_SIZE * w);
		cairo_stroke(cr);
	}

	// Dice
	dice_draw(cr, board->dice, board->consumed_dice, w, h);

	// Prisons
	draw_prison(cr, bg, 0, w, h);
	draw_prison(cr, bg, 1, w, h);

	// Goal
	draw_goal(cr, bg, 0, w, h);
	draw_goal(cr, bg, 1, w, h);

	// Goal marks
	draw_mark(cr, board->goal[0], w, h);
	draw_mark(cr, board->goal[1], w, h);

	return TRUE;
}

/**
 * @brief Creates an instance of Board
 * 
 * @param builder GtkBuilder for the DrawingArea
 * @param bg Backgammon instance
 * @return Board* New instance of Board
 */
Board *board_new(GtkBuilder *builder, void *bg) {

	Board *board = (Board *) g_malloc(sizeof(Board));

	board->drawing_area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "board-area"));
	g_signal_connect(board->drawing_area, "draw", G_CALLBACK(board_on_draw), bg);
	gtk_widget_set_events(
		GTK_WIDGET(board->drawing_area),
		GDK_BUTTON_PRESS_MASK);
	g_signal_connect(board->drawing_area, "button-press-event", G_CALLBACK(board_on_click), bg);

	board->movements = NULL;

	board_reset(board);

	return board;
}

/**
 * @brief Frees the board from memory
 * 
 * @param board Board instance
 */
void board_free(Board *board) {
	g_free(board);
}

/**
 * @brief Initializes the board values
 * 
 * @param board Board instance
 */
void board_init(Board *board) {
	guint i;

	for (i = 0; i < 23; i ++) board->places[i].data = 0;
	for (i = 0; i < 2; i ++) {
		board->goal[i].data = 0;
		board->prison[i] = 0;
	}

	// Test:
	
	board->goal[0].data = -13;
	board->goal[1].data = 13;
	

	/*board->places[0].data = 2;
	board->places[5].data = -5;
	board->places[7].data = -3;
	board->places[11].data = 5;
	board->places[12].data = -5;
	board->places[16].data = 3;
	board->places[18].data = 5;
	board->places[23].data = -2;*/

	
	board->places[0].data = -2;
	board->places[23].data = 2;

	for (i = 0; i < 4; i ++) board->consumed_dice[i] = FALSE;

}

/**
 * @brief Resets the board by arranging the pieces according to the rules
 * 
 * @param board Board instance
 */
void board_reset(Board *board) {
	guint i;
	gdouble x = 1.0 - PLACE_SIZE * 2;

	// Arrange the pieces
	for (i = 0; i < 24; i ++) {
		board->places[i].mark = FALSE;

		board->places[i].id = i;
		board->places[i].data = 0;
		board->places[i].x = x;
		board->places[i].y = i < 12 ? 0 : 1.0 - TRIANGLE_HEIGHT;
		if (i < 12) x -= PLACE_SIZE;
		else x += PLACE_SIZE;
		if (i == 5) x -= PLACE_SIZE;
		if (i == 11) x = 0;
		if (i == 17) x += PLACE_SIZE;
	}

	board->selected = -1;
	board->prison_sel = -1;

	board->prison[0] = 0;
	board->prison[1] = 0;

	board->goal[0].id = 0;
	board->goal[0].data = 0;
	board->goal[0].x = board->goal[1].x = 1.0 - PLACE_SIZE * 1;
	board->goal[0].y = 0;
	board->goal[0].mark = FALSE;

	board->goal[1].id = 23;
	board->goal[1].data = 0;
	board->goal[1].y = 1.0 - TRIANGLE_HEIGHT;
	board->goal[1].mark = FALSE;

	board->dice[0] = 1;
	board->dice[1] = 1;

	board->enable_dice = FALSE;
	board->enable_places = FALSE;
}

/**
 * @brief Clears the triangular destination marks from all places
 * 
 * @param board Backgammon instance
 */
void board_clear_marks(Board *board) {
	guint i;
	for (i = 0; i < 24; i ++)
		board->places[i].mark = FALSE;

	board->goal[0].mark = FALSE;
	board->goal[1].mark = FALSE;
}

/**
 * @brief Redraws the board
 * 
 * @param board Backgammon instance
 */
void board_redraw(Board *board) {
	gtk_widget_queue_draw(GTK_WIDGET(board->drawing_area));
}
