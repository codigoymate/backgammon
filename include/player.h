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
	gint piece, direction, score;
	void (*play_func)(void *);
	gboolean ia;
} Player;

/**
 * @brief Update players information:
 * name, step count, and score
 * 
 * @param bg Backgammon instance
 */
void player_update(void *bg);

/**
 * @brief Counts the number of steps the player needs to win
 * 
 * @param bg Instance of Backgammon
 * @param player Target player
 * @return guint number of remaining steps to win
 */
guint player_count_steps(void *bg, Player *player);

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

gboolean ia_delayed_func(gpointer data);

#endif
