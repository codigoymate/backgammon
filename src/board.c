#include <board.h>
#include <cairo.h>
#include <dice.h>

#include <backgammon.h>

void draw_triangle(cairo_t *cr, gdouble x, guchar type, gint w, gint h) {

	gint ybase, ytop;

	ybase = type ? h : 0;
	ytop = type ? h * (1.0 - TRIANGLE_HEIGHT) : h * TRIANGLE_HEIGHT;

	cairo_move_to(cr, w * x, ybase);
	cairo_line_to(cr, w * x + w * PLACE_SIZE, ybase);
	cairo_line_to(cr, w * x + w * PLACE_SIZE / 2, ytop);
	cairo_close_path(cr);
}

void draw_colored_triangle(cairo_t *cr, gdouble x, guchar type, guchar color,
			gint w, gint h) {
	if (color) {COLOR_TRIANGLE_1_FACE(cr);}
	else {COLOR_TRIANGLE_2_FACE(cr);}
	draw_triangle(cr, x, type, w, h);
	cairo_fill(cr);

	if (color) {COLOR_TRIANGLE_1_BORDER(cr);}
	else {COLOR_TRIANGLE_2_BORDER(cr);}
	cairo_set_line_width(cr, 2.0);
	draw_triangle(cr, x, type, w, h);

	cairo_stroke(cr);
	
}

void draw_piece(cairo_t *cr, gdouble x, gdouble y, gint color, gint w, gint h) {

	if (color > 0) {COLOR_PIECE_BLACK_FACE(cr);}
	else COLOR_PIECE_WHITE_FACE(cr);

	cairo_arc(cr, w * x, w * y, PIECE_SIZE * w / 2, 0, G_PI * 2);
	cairo_fill(cr);

	if (color > 0) {COLOR_PIECE_BLACK_BORDER(cr);}
	else COLOR_PIECE_WHITE_BORDER(cr);

	cairo_set_line_width(cr, 2.0);
	cairo_arc(cr, w * x, w * y, PIECE_SIZE * w / 2, 0, G_PI * 2);
	cairo_stroke(cr);
}

void draw_piece_group(cairo_t *cr, Board *board, gdouble x, guint i, gint w, gint h) {
	gdouble y;
	guint count, p;
	char text[3];
	count = board->data[i] < 0 ? board->data[i] * -1 : board->data[i];
	y = i < 12 ? PIECE_SIZE / 2 : 0.55 - PIECE_SIZE / 2;
	for (p = 0; p < count; p ++) {
		if (p >= 4) break;
		draw_piece(cr, x, y, board->data[i], w, h);
		if (i < 12) y += PIECE_SIZE;
		else y -= PIECE_SIZE;
	}

	if (count <= 4) return ;
	if (board->data[i] < 0) { COLOR_PIECE_BLACK_FACE(cr); }
	else COLOR_PIECE_WHITE_FACE(cr);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 0.027 * w);

	if (i < 12) y = 0.4;
	else y = 0.64;

	x -= 0.009;

	cairo_move_to(cr, x * w, y * h);

	g_snprintf(text, sizeof(text), "%i", count);
    cairo_show_text(cr, text);
}

void draw_pieces(cairo_t *cr, Board *board, gint w, gint h) {
	guint i;
	gdouble x;

	x = PLACE_SIZE * 12 + PLACE_SIZE / 2;
	for (i = 0; i < 24; i ++) {
		draw_piece_group(cr, board, x, i, w, h);

		if (i < 12) x -= PLACE_SIZE;
		else x += PLACE_SIZE;

		if (i == 11 || i == 17) x += PLACE_SIZE;
		if (i == 5) x -= PLACE_SIZE;
	}

}

static gboolean board_on_draw(GtkWidget *area, cairo_t *cr, gpointer data) {
	gdouble w, h, x = 0;
	gint i;
	Board *board = (Board *) data;
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

	// Triangulos
	for (i = 1; i <= 12; i ++) {
		draw_colored_triangle(cr, x, 0, i % 2, w, h);
		draw_colored_triangle(cr, x, 1, i % 2 == 0, w, h);
		x += 1.0 / 14.0;
		if (i == 6) x += 1.0 / 14.0;
	}

	// Bars
	COLOR_BAR(cr);
	cairo_rectangle(cr, w * 0.0714 * 6, 0, w * 0.0714, h);
	cairo_rectangle(cr, w * 0.0714 * 13, 0, w * 0.0714, h);
	cairo_fill(cr);

	draw_pieces(cr, board, w, h);

	// Dados
	dice_draw(cr, board->dice, w, h);

	// Test
	/*cairo_rectangle(cr, w * 0.6, h * 0.436, w * 0.289, h * 0.12);
	cairo_stroke(cr);*/

	return TRUE;
}

static gboolean board_on_click(GtkDrawingArea *drw, GdkEventButton *event, gpointer data) {
	gdouble x, y;
	Backgammon *bg = (Backgammon *) data;

	if (event->type == GDK_BUTTON_PRESS && event->button == 1) {

		x = event->x / (gdouble) gtk_widget_get_allocated_width(GTK_WIDGET(drw));
		y = event->y / (gdouble) gtk_widget_get_allocated_height(GTK_WIDGET(drw));

		// Dice
		if (bg->board->enable_dice)
			if (x > 0.6 && x < 0.6 + 0.289 && y > 0.436 && y < 0.436 + 0.12) {
				dice_roll(bg->board->dice);
				gtk_widget_queue_draw(GTK_WIDGET(drw));
	
				bg->status = S_MOVE_PIECES;
				backgammon_next_step(bg);
			}
	}

	return TRUE;
}

Board *board_new(GtkBuilder *builder, void *bg) {
	
	Board *board = (Board *) g_malloc(sizeof(Board));

	board->drawing_area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "board-area"));
	g_signal_connect(board->drawing_area, "draw", G_CALLBACK(board_on_draw), board);
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

void board_reset(Board *board) {
	guint i;

	// Acomoda las piezas
	for (i = 0; i < 24; i ++)
		board->data[i] = 0;

	board->data[0] = 2;
	board->data[5] = -5;
	board->data[7] = -3;
	board->data[11] = 5;
	board->data[12] = -5;
	board->data[16] = 3;
	board->data[18] = 5;
	board->data[23] = -2;

	//dice_roll(board->dice);
	board->dice[0] = 1;
	board->dice[1] = 1;

	board->enable_dice = FALSE;
}
