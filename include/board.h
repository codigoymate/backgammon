#ifndef BOARD_H
#define BOARD_H

#define BLACK								1
#define WHITE								-1

#define BOARD_RATIO							0.55
#define TRIANGLE_HEIGHT						0.43
#define PLACE_SIZE							0.071428571
#define PIECE_SIZE							0.06
#define COLOR_BACKGROUND(cr)				cairo_set_source_rgb(cr, 0.4, 0.2, 0.0)
#define COLOR_BAR(cr)						cairo_set_source_rgb(cr, 0.2, 0.1, 0.0)
#define COLOR_TRIANGLE_1_BORDER(cr)			cairo_set_source_rgba(cr, 0.6, 0.0, 0.0, 0.5)
#define COLOR_TRIANGLE_1_FACE(cr)			cairo_set_source_rgba(cr, 0.4, 0.0, 0.0, 0.5)
#define COLOR_TRIANGLE_2_BORDER(cr)			cairo_set_source_rgba(cr, 0.0, 0.6, 0.0, 0.5)
#define COLOR_TRIANGLE_2_FACE(cr)			cairo_set_source_rgba(cr, 0.0, 0.4, 0.0, 0.5)
#define COLOR_PIECE_WHITE_BORDER(cr)		cairo_set_source_rgb(cr, 1.0, 1.0, 1.0)
#define COLOR_PIECE_WHITE_FACE(cr)			cairo_set_source_rgb(cr, 0.8, 0.8, 0.8)
#define COLOR_PIECE_BLACK_BORDER(cr)		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0)
#define COLOR_PIECE_BLACK_FACE(cr)			cairo_set_source_rgb(cr, 0.2, 0.2, 0.2)

#include <gtk/gtk.h>


typedef struct board_t {
	GtkDrawingArea *drawing_area;
	gint data[24]; guint dice[2];
} Board;

Board *board_new(GtkBuilder *builder);
void board_free(Board *board);
void board_reset(Board *board);

#endif
