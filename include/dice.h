#ifndef DICE_H
#define DICE_H

#include <glib.h>
#include <cairo.h>

#define DICE_SIZE					0.065
#define DOT_SIZE					0.10
#define COLOR_DICE_FACE(cr)			cairo_set_source_rgb(cr, 1.0, 1.0, 1.0)
#define COLOR_DICE_BORDER(cr)		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0)
#define COLOR_DICE_DOT(cr)			cairo_set_source_rgb(cr, 0.0, 0.0, 0.0)
#define COLOR_DICE_DISABLE(cr)		cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.75)


void dice_roll(guint dice[], gboolean consumend[]);
void dice_draw(cairo_t *cr, guint dice[], gboolean consumed[], gint w, gint h);

#endif
