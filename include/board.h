/**
 * @file board.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Board definitions of the Backgammon game.
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef BOARD_H
#define BOARD_H

// Piece types
#define BLACK               1
#define WHITE               0

// Some measures
#define BOARD_RATIO         0.55
#define TRIANGLE_HEIGHT     0.43
#define PLACE_SIZE          0.071428571
#define PIECE_SIZE          0.06
#define PIECE_HEIGHT        0.015
#define MARK_HEIGHT         0.03

// Board colors
#define COLOR_BACKGROUND(cr)                cairo_set_source_rgb(cr, 0.4, 0.2, 0.0)
#define COLOR_BAR(cr)                       cairo_set_source_rgb(cr, 0.2, 0.1, 0.0)
#define COLOR_TRIANGLE_1_BORDER(cr)         cairo_set_source_rgba(cr, 0.6, 0.0, 0.0, 0.5)
#define COLOR_TRIANGLE_1_FACE(cr)           cairo_set_source_rgba(cr, 0.4, 0.0, 0.0, 0.5)
#define COLOR_TRIANGLE_2_BORDER(cr)         cairo_set_source_rgba(cr, 0.0, 0.6, 0.0, 0.5)
#define COLOR_TRIANGLE_2_FACE(cr)           cairo_set_source_rgba(cr, 0.0, 0.4, 0.0, 0.5)
#define COLOR_PIECE_WHITE_BORDER(cr)        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0)
#define COLOR_PIECE_WHITE_FACE(cr)          cairo_set_source_rgb(cr, 0.8, 0.8, 0.8)
#define COLOR_PIECE_BLACK_BORDER(cr)        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0)
#define COLOR_PIECE_BLACK_FACE(cr)          cairo_set_source_rgb(cr, 0.2, 0.2, 0.2)

#define COLOR_SELECTION(cr)                 cairo_set_source_rgb(cr, 1.0, 1.0, 0.2)
#define COLOR_MARK(cr)                      cairo_set_source_rgb(cr, 0.0, 1.0, 0.0)

#include <gtk/gtk.h>

/**
 * @brief Represents a place where pieces are stacked. If the value of data is
 * 1, it represents the pieces counterclockwise, and with -1 clockwise. When
 * data is 0, there are no pieces.
 * 
 */
typedef struct place_t {
	guint id;
	gdouble x, y;
	gint data;
	gboolean mark;
} Place;

/**
 * @brief Contains information about the board
 * 
 */
typedef struct board_t {
	GtkDrawingArea *drawing_area;
	Place places[24], goal[2];
	gint prison[2];
	gint selected, prison_sel;
	guint dice[2];
	gboolean consumed_dice[4];
	gboolean enable_dice, enable_places;

	GList *movements;
} Board;

/**
 * @brief Creates an instance of Board
 * 
 * @param builder GtkBuilder for the DrawingArea
 * @param bg Backgammon instance
 * @return Board* New instance of Board
 */
Board *board_new(GtkBuilder *builder, void *bg);

/**
 * @brief Frees the board from memory
 * 
 * @param board Board instance
 */
void board_free(Board *board);

/**
 * @brief Resets the board values
 * 
 * @param board Board instance
 */
void board_reset(Board *board);

/**
 * @brief Initializes the board by arranging the pieces according to the rules
 * 
 * @param board Board instance
 */
void board_init(Board *board);

/**
 * @brief Clears the triangular destination marks from all places
 * 
 * @param board Backgammon instance
 */
void board_clear_marks(Board *board);

/**
 * @brief Redraws the board
 * 
 * @param board Backgammon instance
 */
void board_redraw(Board *board);

#endif
