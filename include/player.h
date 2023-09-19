/**
 * @file player.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Definition of the structure for a generic player
 * @date 2023-09-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef PLAYER_H
#define PLAYER_H

#include <glib.h>

/**
 * @brief Structure for a generic player that associates the name,
 * piece, and direction on the board.
 * It contains a function pointer to change behavior
 * based on whether it's a human or AI player.
 * 
 */
typedef struct player_t {
	GString *name;
	gint piece;
	gint direction;
	void (*play_func)(void *);
} Player;

/**
 * @brief Function assigned to a player to configure them as a human player.
 * 
 * @param bg Backgammon instance
 */
void human_play_func(void *bg);

/**
 * @brief Function assigned to a player to configure them as an artificial intelligence.
 * 
 * @param bg Backgammon instance
 */
void ia_play_func(void *bg);

#endif
