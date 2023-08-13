#ifndef BACKGAMMON_H
#define BACKGAMMON_H

#define S_NOT_PLAYING	0
#define S_ROLL_DICE		1
#define S_MOVE_PIECES	2
#define S_END_TURN		3

#include <gtk/gtk.h>
#include <board.h>
#include <player.h>

typedef struct backgammon_t {
	GtkApplicationWindow *window;
	GtkLabel *turn_label, *action_label;
	GtkButton *end_turn_button;
	Board *board;
	gint player_turn, status;
	Player player[2];
} Backgammon;

Backgammon *bg_new(int argc, char *argv[]);
void bg_run(Backgammon *bg);

void bg_next_step(Backgammon *bg);
void bg_next_turn(Backgammon *bg);
Player *bg_current_player(Backgammon *bg);
Player *bg_opponent(Backgammon *bg);
Player *bg_player_by_data(Backgammon *bg, gint data);

void bg_move_piece(Backgammon *bg, gint source, gint destiny);
gboolean bg_player_can_move(Backgammon *bg);

#endif
