/**
 * @file double_dice.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of double_dice.h
 * @date 2023-10-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <double_dice.h>

#include <backgammon.h>
#include <math.h>

/**
 * @brief Sets the Pixbuf based on the number of points.
 * If points is 1, then it removes the image.
 * 
 * @param bg Backgammon instance
 * @param img GtkImage instance
 * @param points Number of points
 */
void set_double_dice_image(Backgammon *bg, GtkImage *img, guint points) {
	if (points < 2)
		gtk_image_set_from_pixbuf(img, NULL);
	else {
		// 0 --> 2
		// 1 --> 4
		// 2 --> 8
		// 3 --> 16
		// 4 --> 32
		// 5 --> 64
		gtk_image_set_from_pixbuf(img,
			bg->double_pixbuf[(gint)(log(points) / log(2)) - 1]
		);
	}
}

/**
 * @brief Doubles the dice for the current player.
 * 
 * @param bg Backgammon instance
 */
void double_perform(void *bgp) {
	Backgammon *bg;
	guint points;

	bg = (Backgammon *) bgp;

	// If the player has already doubled
	if (bg_current_player(bg)->double_points != 1) return;

	// Gets the opponent's double and multiplies it by two
	points = bg_opponent(bg)->double_points;
	points *= 2;

	// Maximum reached. Cannot double anymore
	if (points > 64) return;

	// Sets the points
	bg_opponent(bg)->double_points = 1;
	bg_current_player(bg)->double_points = points;

	// Disables the button
	gtk_widget_set_sensitive(GTK_WIDGET(bg->double_button), FALSE);

	// Set points
	if (bg->player[0].direction == -1) {
		set_double_dice_image(bg, bg->double_image[0], bg->player[0].double_points);
		set_double_dice_image(bg, bg->double_image[1], bg->player[1].double_points);
	} else {
		set_double_dice_image(bg, bg->double_image[1], bg->player[0].double_points);
		set_double_dice_image(bg, bg->double_image[0], bg->player[1].double_points);
	}
}
