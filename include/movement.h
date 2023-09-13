/**
 * @file movement.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Module for scanning possible moves and move pieces
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <gtk/gtk.h>
#include <backgammon.h>

/**
 * @brief Represents a possible move
 * 
 */
typedef struct movement_t {
    gint src, dest;
    gboolean prison_src, goal_dest;
} Movement;

/**
 * @brief Creates a new instance of Movement
 * 
 * @param src source
 * @param dest destination
 * @param prison_src source as prison
 * @param goal_dest destination as goal
 * @return Movement* a new instance of Movement
 */
Movement *movement_new(gint src, gint dest, gboolean prison_src, gboolean goal_dest);

/**
 * @brief Searches for possible moves for the current player.
 * Set results in board instance.
 * 
 * @param bg Backgammon instance
 */
void scan_movements(Backgammon *bg);

/**
 * @brief Clean current movements in Board instance.
 * 
 * @param bg Backgammon's instance.
 */
void clean_movements(Backgammon *bg);

/**
 * @brief Moves a game piece.
 * 
 * @param bg Backgammon instance
 * @param movement the register movement
 */
void move_piece(Backgammon *bg, Movement *movement);

#endif
