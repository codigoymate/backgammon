/**
 * @file backgammon.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Main header for Backgammon
 * @date 2023-08-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef BACKGAMMON_H
#define BACKGAMMON_H

/**
 * @brief Game states
 */
#define S_NOT_PLAYING	0
#define S_ROLL_DICE		1
#define S_MOVE_PIECES	2
#define S_END_TURN		3
#define S_END_ROUND		4

#define DELAYED_FUNC_TIMEOUT		1000
#define DDICE_SIZE					58

#include <gtk/gtk.h>
#include <board.h>
#include <player.h>
#include <undo.h>

/**
 * @brief Main structure of the game
 * 
 */
typedef struct backgammon_t {
	GtkApplicationWindow *window;
	GtkLabel *turn_label, *action_label;
	GtkButton *end_turn_button, *undo_button, *double_button;
	GtkLabel *player_name_label[2],
				*steps_label[2],
				*score_label[2];
	GtkImage *double_image[2];
	GdkPixbuf *double_pixbuf[6];
	Board *board;
	Undo undo;
	gint player_turn, status, max_score;
	Player player[2];
} Backgammon;

/**
 * @brief Creates an instance of backgammon
 * 
 * @param argc argument count
 * @param argv argument vector
 * @return Backgammon* instance of Backgammon
 */
Backgammon *bg_new(int argc, char *argv[]);

/**
 * @brief Runs backgammon
 * 
 * @param bg backgammon instance
 */
void bg_run(Backgammon *bg);

/**
 * @brief Calls the next step. If the game is not in progress, it starts.
 * Then, the play_func associated with the current player is called.
 * 
 * @param bg Backgammon instance
 */
void bg_next_step(Backgammon *bg);

/**
 * @brief Switches to the next player's turn.
 * 
 * @param bg Backgammon instance
 */
void bg_next_turn(Backgammon *bg);

/**
 * @brief Returns the current player.
 * 
 * @param bg Backgammon instance
 * @return Player* instance of the current player
 */
Player *bg_current_player(Backgammon *bg);

/**
 * @brief Returns the opponent of the current player.
 * 
 * @param bg Backgammon instance
 * @return Player* instance of the opponent
 */
Player *bg_opponent(Backgammon *bg);

/**
 * @brief Returns the player who represents the piece(s) at the indicated position.
 * 
 * @param bg Backgammon instance
 * @param data value of the position
 * @return Player* instance of the player
 */
Player *bg_player_by_data(Backgammon *bg, gint data);

/**
 * @brief Checks if all the pieces of the current player are in their territory.
 * 
 * @param bg Backgammon instance
 * @return gboolean True if all the pieces are in the territory of the current player.
*/
gboolean bg_all_pieces_in_territory(Backgammon *bg);

#endif
