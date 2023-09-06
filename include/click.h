/**
 * @file click.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief On board click events
 * @date 2023-09-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef CLICK_H
#define CLICK_H

#include <gtk/gtk.h>

/**
 * @brief Occurs when a click is made on the board
 * 
 * @param drw DrawingArea component
 * @param event Mouse button event
 * @param data User data (Backgammon)
 * @return gboolean TRUE if drawing success
 */
gboolean board_on_click(GtkDrawingArea *drw, GdkEventButton *event, gpointer data);

#endif
