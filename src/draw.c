#include <draw.h>

void draw_triangle(cairo_t *cr, Place place, gint w, gint h) {

	gint ybase, ytop;

	ybase = place.id >= 12 ? h : 0;
	ytop = place.id >= 12 ? h * (1.0 - TRIANGLE_HEIGHT) : h * TRIANGLE_HEIGHT;

	cairo_move_to(cr, w * place.x, ybase);
	cairo_line_to(cr, w * place.x + w * PLACE_SIZE, ybase);
	cairo_line_to(cr, w * place.x + w * PLACE_SIZE / 2, ytop);
	cairo_close_path(cr);
}

void draw_colored_triangle(cairo_t *cr, Place place, guchar color,
			gint w, gint h) {
	if (color) {COLOR_TRIANGLE_1_FACE(cr);}
	else {COLOR_TRIANGLE_2_FACE(cr);}
	draw_triangle(cr, place, w, h);
	cairo_fill(cr);

	if (color) {COLOR_TRIANGLE_1_BORDER(cr);}
	else {COLOR_TRIANGLE_2_BORDER(cr);}
	cairo_set_line_width(cr, 2.0);
	draw_triangle(cr, place, w, h);

	cairo_stroke(cr);
	
}

void draw_piece(cairo_t *cr, gdouble x, gdouble y, gint color, gint w, gint h) {

	if (color == BLACK) {COLOR_PIECE_BLACK_FACE(cr);}
	else COLOR_PIECE_WHITE_FACE(cr);

	cairo_arc(cr, w * x, w * y, PIECE_SIZE * w / 2, 0, G_PI * 2);
	cairo_fill(cr);

	if (color == BLACK) {COLOR_PIECE_BLACK_BORDER(cr);}
	else COLOR_PIECE_WHITE_BORDER(cr);

	cairo_set_line_width(cr, 2.0);
	cairo_arc(cr, w * x, w * y, PIECE_SIZE * w / 2, 0, G_PI * 2);
	cairo_stroke(cr);
}

void draw_piece_group(cairo_t *cr, Backgammon *bg, Place place, gint w, gint h) {
	gdouble y;
	guint count, p;
	char text[3];
	count = place.data < 0 ? place.data * -1 : place.data;
	y = place.id < 12 ? place.y + PIECE_SIZE / 2 : place.y - 0.05;
	for (p = 0; p < count; p ++) {
		if (p >= 4) break;
		draw_piece(cr, place.x + PLACE_SIZE / 2, y,
			bg_player_by_data(bg, place.data)->piece,
			w, h);
		if (place.id < 12) y += PIECE_SIZE;
		else y -= PIECE_SIZE;
	}

	if (count <= 4) return ;
	if (bg_player_by_data(bg, place.data)->piece == WHITE) { COLOR_PIECE_BLACK_FACE(cr); }
	else COLOR_PIECE_WHITE_FACE(cr);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 0.027 * w);

	if (place.id < 12) y = 0.4;
	else y = 0.64;

	cairo_move_to(cr, (place.x + PLACE_SIZE / 2 - 0.009) * w, y * h);

	g_snprintf(text, sizeof(text), "%i", count);
    cairo_show_text(cr, text);
}

void draw_mark(cairo_t *cr, Place place, gint w, gint h) {
	gdouble ybase, ytop;
	if (place.mark) {
		ybase = place.id < 12 ? MARK_HEIGHT : 1.0 - MARK_HEIGHT;
		ytop = place.id < 12 ? 0.0 : 1.0;

		COLOR_MARK(cr);
		cairo_move_to(cr, w * place.x, ybase * h);
		cairo_line_to(cr, w * place.x + w * PLACE_SIZE, ybase * h);
		cairo_line_to(cr, w * place.x + w * PLACE_SIZE / 2, ytop * h);
		cairo_close_path(cr);

		cairo_stroke(cr);
	}
}

void draw_prison(cairo_t *cr, Backgammon *bg, gint prison, gint w, gint h) {
	gdouble y;
	gint count;
	gchar text[3];

	Board *board;

	board = bg->board;

	if (!board->prison[prison]) return ;

	y = prison ?  0.4 : 0.15;
	draw_piece(cr, PLACE_SIZE * 7 - PLACE_SIZE / 2, y,
			//board->prison[prison],
			bg_player_by_data(bg, board->prison[prison])->piece,
			w, h);

	count = board->prison[prison];
	count = count < 0 ? count * -1 : count;

	if (count < 2) return ;
	if (bg_player_by_data(bg, board->prison[prison])->piece == WHITE) { COLOR_PIECE_BLACK_FACE(cr); }
	else COLOR_PIECE_WHITE_FACE(cr);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 0.027 * w);

	y += 0.011;
	cairo_move_to(cr, (PLACE_SIZE * 7 - PLACE_SIZE / 2 - 0.009) * w, y * w);

	g_snprintf(text, sizeof(text), "%i", count);
    cairo_show_text(cr, text);
}

void draw_goal(cairo_t *cr, Backgammon *bg, gint goal, gint w, gint h) {
	gdouble y, x;
	gint count;
	guint piece, i;

	Board *board = bg->board;

	if (!board->goal[goal].data) return ;

	y = goal ?  0.535 : 0.0;
	x = w * PLACE_SIZE * 13;

	piece = bg_player_by_data(bg, board->goal[goal].data)->piece;
	count = board->goal[goal].data;
	if (count < 0) count *= -1;

	for (i = 0; i < count; i ++) {

		// Piece color
		if (piece == BLACK) { COLOR_PIECE_BLACK_FACE(cr); }
		else COLOR_PIECE_WHITE_FACE(cr);

		cairo_rectangle(cr, x, y * w, PIECE_SIZE * w, PIECE_HEIGHT * w);
		cairo_fill(cr);

		if (piece == BLACK) { COLOR_PIECE_BLACK_BORDER(cr); }
		else COLOR_PIECE_WHITE_BORDER(cr);

		cairo_rectangle(cr, x, y * w, PIECE_SIZE * w, PIECE_HEIGHT * w);

		if (goal) y -= PIECE_HEIGHT;
		else y += PIECE_HEIGHT;

		cairo_stroke(cr);
	}
	
}
