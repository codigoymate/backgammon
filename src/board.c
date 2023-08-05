#include <board.h>
#include <cairo.h>
#include <dice.h>

#include <backgammon.h>
#include <checker.h>

void board_redraw(Board *board);
void dice_click(Backgammon *bg);
void place_click(Backgammon *bg, Place *place);

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

void draw_piece_group(cairo_t *cr, Place place, gint w, gint h) {
	gdouble y;
	guint count, p;
	char text[3];
	count = place.data < 0 ? place.data * -1 : place.data;
	y = place.id < 12 ? place.y + PIECE_SIZE / 2 : place.y - 0.05;
	for (p = 0; p < count; p ++) {
		if (p >= 4) break;
		draw_piece(cr, place.x + PLACE_SIZE / 2, y, place.data, w, h);
		if (place.id < 12) y += PIECE_SIZE;
		else y -= PIECE_SIZE;
	}

	if (count <= 4) return ;
	if (place.data < 0) { COLOR_PIECE_BLACK_FACE(cr); }
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

static gboolean board_on_draw(GtkWidget *area, cairo_t *cr, gpointer data) {
	gdouble w, h;
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

	// Triangulos, fichas y marcas
	for (i = 0; i < 24; i ++) {
		draw_colored_triangle(cr, board->places[i], i % 2, w, h);
		draw_piece_group(cr, board->places[i], w, h);
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

	// Dados
	dice_draw(cr, board->dice, w, h);

	return TRUE;
}

static gboolean board_on_click(GtkDrawingArea *drw, GdkEventButton *event, gpointer data) {
	gdouble x, y, px, py;
	gint i;
	Backgammon *bg = (Backgammon *) data;

	if (event->type == GDK_BUTTON_PRESS && event->button == 1) {

		x = event->x / (gdouble) gtk_widget_get_allocated_width(GTK_WIDGET(drw));
		y = event->y / (gdouble) gtk_widget_get_allocated_height(GTK_WIDGET(drw));

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
	}

	return TRUE;
}

void dice_click(Backgammon *bg) {
	dice_roll(bg->board->dice, bg->board->consumed_dice);
	board_redraw(bg->board);

	bg->status = S_MOVE_PIECES;
	backgammon_next_step(bg);
}

void place_click(Backgammon *bg, Place *place) {
	
	Board *b = bg->board;

	// Si el lugar a seleccionar tiene una marca
	if (place->mark) {
		backgammon_move_piece(bg, b->selected, place->id);
		b->selected = -1;
		board_clear_marks(b);

		// Comprueba si sigue habiendo movimientos
		if (!backgammon_player_can_move(bg)) {
			/*backgammon_next_turn(bg);*/
			bg->status = S_END_TURN;
			backgammon_next_step(bg);

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
		if (place->data * backgammon_current_player(bg)->piece < 0) {
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

	board->places[0].data = 2;
	board->places[5].data = -5;
	board->places[7].data = -3;
	board->places[11].data = 5;
	board->places[12].data = -5;
	board->places[16].data = 3;
	board->places[18].data = 5;
	board->places[23].data = -2;

	//dice_roll(board->dice);
	board->dice[0] = 1;
	board->dice[1] = 1;

	board->enable_dice = FALSE;
	board->enable_places = FALSE;
}

void board_clear_marks(Board *board) {
	guint i;
	for (i = 0; i < 24; i ++)
		board->places[i].mark = FALSE;
}

void board_redraw(Board *board) {
	gtk_widget_queue_draw(GTK_WIDGET(board->drawing_area));
}
