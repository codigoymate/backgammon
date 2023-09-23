/**
 * @file dice.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Module related to dice
 * @date 2023-09-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef DICE_H
#define DICE_H

#include <glib.h>
#include <cairo.h>

// Drawing constants
#define DICE_SIZE					0.065
#define DOT_SIZE					0.10
#define COLOR_DICE_FACE(cr)			cairo_set_source_rgb(cr, 1.0, 1.0, 1.0)
#define COLOR_DICE_BORDER(cr)		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0)
#define COLOR_DICE_DOT(cr)			cairo_set_source_rgb(cr, 0.0, 0.0, 0.0)
#define COLOR_DICE_DISABLE(cr)		cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.75)

/**
 * @brief Chooses random values for dice[0] and dice[1].
 * 
 * @param dice Set of dice
 * @param consumed Dice consumption flags
 */
void dice_roll(guint dice[], gboolean consumend[]);

/**
 * @brief Draws the dice.
 * 
 * @param cr Cairo context
 * @param dice Set of dice
 * @param consumed Dice consumption flags
 * @param w Drawing area width
 * @param h Drawing area height
 */
void dice_draw(cairo_t *cr, guint dice[], gboolean consumed[], gint w, gint h);

#endif
