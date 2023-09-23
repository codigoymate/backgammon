/**
 * @file dice.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of dice.h
 * @date 2023-09-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <dice.h>

/**
 * @brief Chooses random values for dice[0] and dice[1].
 * Sets all 4 consumed dice flags to False.
 * 
 * @param dice Set of dice
 * @param consumed Dice consumption flags
 */
void dice_roll(guint dice[], gboolean consumed[]) {
	guint i;

	for (i = 0; i < 4; i ++) consumed[i] = FALSE;
	for (i = 0; i < 2; i ++) dice[i] = g_random_int_range(1, 7);
	
#ifdef BG_DEBUG
	g_print("Roll: ");
	for (i = 0; i < (dice[0] == dice[1] ? 4 : 2); i ++) {
		g_print("%i ", dice[i % 2]);
	}
	g_print("\n");
#endif
}

/**
 * @brief Draws a rounded rectangle.
 * 
 * @param cr Cairo context
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Rectangle width
 * @param height Rectangle height
 * @param radius Radius of rounded corners
 */
void draw_rounded_rectangle(cairo_t *cr, gdouble x, gdouble y,
			gdouble width, gdouble height, gdouble radius) {

    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -G_PI / 2.0, 0.0);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0.0, G_PI / 2.0);
    cairo_arc(cr, x + radius, y + height - radius, radius, G_PI / 2.0, G_PI);
    cairo_arc(cr, x + radius, y + radius, radius, G_PI, -G_PI / 2.0);
    cairo_close_path(cr);
}

/**
 * @brief Draws a single die.
 * 
 * @param cr Cairo context
 * @param value Die value
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Drawing area width
 * @param h Drawing area height
 */
void dice_draw_single(cairo_t *cr, guint value, gdouble x, gdouble y, gint w, gint h) {
	COLOR_DICE_FACE(cr);
	draw_rounded_rectangle(cr, x * w, y * h, DICE_SIZE * w, DICE_SIZE * w, DICE_SIZE * w * 0.18);
	cairo_fill(cr);
	COLOR_DICE_BORDER(cr);
	draw_rounded_rectangle(cr, x * w, y * h, DICE_SIZE * w, DICE_SIZE * w, DICE_SIZE * w * 0.18);
	cairo_stroke(cr);

	COLOR_DICE_DOT(cr);
	if (value % 2) {
		cairo_arc(cr, x * w + DICE_SIZE * w * 0.5, y * h + DICE_SIZE * w * 0.5,
				w * DICE_SIZE * DOT_SIZE, 0, G_PI * 2);
		cairo_fill(cr);
	}

	if (value >= 2) {
		cairo_arc(cr, x * w + DICE_SIZE * w * 0.2, y * h + DICE_SIZE * w * 0.2,
				w * DICE_SIZE * DOT_SIZE, 0, G_PI * 2);
		cairo_arc(cr, x * w + DICE_SIZE * w * 0.8, y * h + DICE_SIZE * w * 0.8,
				w * DICE_SIZE * DOT_SIZE, 0, G_PI * 2);
		cairo_fill(cr);
	}

	if (value >= 4) {
		cairo_arc(cr, x * w + DICE_SIZE * w * 0.2, y * h + DICE_SIZE * w * 0.8,
				w * DICE_SIZE * DOT_SIZE, 0, G_PI * 2);
		cairo_arc(cr, x * w + DICE_SIZE * w * 0.8, y * h + DICE_SIZE * w * 0.2,
				w * DICE_SIZE * DOT_SIZE, 0, G_PI * 2);
		cairo_fill(cr);
	}

	if (value == 6) {
		cairo_arc(cr, x * w + DICE_SIZE * w * 0.2, y * h + DICE_SIZE * w * 0.5,
				w * DICE_SIZE * DOT_SIZE, 0, G_PI * 2);
		cairo_arc(cr, x * w + DICE_SIZE * w * 0.8, y * h + DICE_SIZE * w * 0.5,
				w * DICE_SIZE * DOT_SIZE, 0, G_PI * 2);
		cairo_fill(cr);
	}
	
}

/**
 * @brief Draws the dice.
 * Draws 2 dice when values are different and 4 when they are the same.
 * Draws grayed-out dice when they are consumed.
 * 
 * @param cr Cairo context
 * @param dice Set of dice
 * @param consumed Dice consumption flags
 * @param w Drawing area width
 * @param h Drawing area height
 */
void dice_draw(cairo_t *cr, guint dice[], gboolean consumed[], gint w, gint h) {
	guint i;
	gdouble x = 0.6;
	for (i = 0; i < (dice[0] == dice[1] ? 4 : 2); i ++) {
		dice_draw_single(cr, dice[i % 2], x, 0.47 - DICE_SIZE / 2, w, h);

		if (consumed[i]) {
			COLOR_DICE_DISABLE(cr);
			draw_rounded_rectangle(cr, x * w, (0.47 - DICE_SIZE / 2) * h, DICE_SIZE * w, DICE_SIZE * w, DICE_SIZE * w * 0.18);
			cairo_fill(cr);
		}

		x += DICE_SIZE + 0.01;
	}
}
