/**
 * @file draw.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Functions for drawing game elements using the Cairo context
 * @date 2023-09-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef DRAW_H
#define DRAW_H

#include <backgammon.h>

/**
 * @brief Draws a triangle with orientation based on the place index and color.
 * 
 * @param cr Cairo context
 * @param place The place representing the triangle
 * @param color The color of the triangle
 * @param w Width of the drawing area
 * @param h Height of the drawing area
 */
void draw_colored_triangle(cairo_t *cr, Place place, guchar color,
			gint w, gint h);

/**
 * @brief Draws the set of pieces at a specific place.
 * 
 * @param cr Cairo context
 * @param bg Backgammon instance
 * @param place The place where the pieces are located
 * @param w Width of the drawing area
 * @param h Height of the drawing area
 */
void draw_piece_group(cairo_t *cr, Backgammon *bg, Place place, gint w, gint h);

/**
 * @brief Draws a target mark at a specific place.
 * 
 * @param cr Cairo context
 * @param place The destination place for the mark
 * @param w Width of the drawing area
 * @param h Height of the drawing area
 */
void draw_mark(cairo_t *cr, Place place, gint w, gint h);

/**
 * @brief Draws the pieces in a prison.
 * 
 * @param cr Cairo context
 * @param bg Backgammon instance
 * @param prison The prison to be drawn
 * @param w Width of the drawing area
 * @param h Height of the drawing area
 */
void draw_prison(cairo_t *cr, Backgammon *bg, gint prison, gint w, gint h);

/**
 * @brief Draws the pieces in the goal area.
 * 
 * @param cr Cairo context
 * @param bg Backgammon instance
 * @param goal The goal area to be drawn
 * @param w Width of the drawing area
 * @param h Height of the drawing area
 */
void draw_goal(cairo_t *cr, Backgammon *bg, gint goal, gint w, gint h);

#endif
