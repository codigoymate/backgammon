#include <board.h>
#include <cairo.h>
#include <dice.h>

#include <backgammon.h>
#include <draw.h>
#include <click.h>

static gboolean board_on_draw(GtkWidget *area, cairo_t *cr, gpointer data) {
	gdouble w, h;
	gint i;

	Backgammon *bg;
	Board *board;

	bg = (Backgammon *) data;
	board = bg->board;

	w = gtk_widget_get_allocated_width(area);
	h = gtk_widget_get_allocated_height(area);

	// Relación de aspecto
	if (h > w * BOARD_RATIO) {
		h = w * BOARD_RATIO;
	} else {
		w = h * (1.0 / BOARD_RATIO);
	}

	// Fondo
	COLOR_BACKGROUND(cr);
	cairo_paint(cr);

	// Triangulos, fichas y marcas
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

	// Selección
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

	// Dados
	dice_draw(cr, board->dice, board->consumed_dice, w, h);

	// Prisiones
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

Board *board_new(GtkBuilder *builder, void *bg) {
	
	Board *board = (Board *) g_malloc(sizeof(Board));

	board->drawing_area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "board-area"));
	g_signal_connect(board->drawing_area, "draw", G_CALLBACK(board_on_draw), bg);
	gtk_widget_set_events(
		GTK_WIDGET(board->drawing_area),
		GDK_BUTTON_PRESS_MASK);
	g_signal_connect(board->drawing_area, "button-press-event", G_CALLBACK(board_on_click), bg);

	board_reset(board);

	return board;
}

void board_free(Board *board) {
	g_free(board);
}

void board_init(Board *board) {
	/*
	board->places[0].data = 2;
	board->places[5].data = -5;
	board->places[7].data = -3;
	board->places[11].data = 5;
	board->places[12].data = -5;
	board->places[16].data = 3;
	board->places[18].data = 5;
	board->places[23].data = -2;
	*/

	board->places[4].data = -2;
	board->places[20].data = 2;
}

void board_reset(Board *board) {
	guint i;
	gdouble x = 1.0 - PLACE_SIZE * 2;

	// Acomoda las piezas
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

void board_clear_marks(Board *board) {
	guint i;
	for (i = 0; i < 24; i ++)
		board->places[i].mark = FALSE;

	board->goal[0].mark = FALSE;
	board->goal[1].mark = FALSE;
}

void board_redraw(Board *board) {
	gtk_widget_queue_draw(GTK_WIDGET(board->drawing_area));
}
