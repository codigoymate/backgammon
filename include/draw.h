#ifndef DRAW_H
#define DRAW_H

#include <backgammon.h>

void draw_colored_triangle(cairo_t *cr, Place place, guchar color,
			gint w, gint h);
void draw_piece_group(cairo_t *cr, Backgammon *bg, Place place, gint w, gint h);
void draw_mark(cairo_t *cr, Place place, gint w, gint h);
void draw_prison(cairo_t *cr, Board *board, gint prison, gint w, gint h);

#endif
