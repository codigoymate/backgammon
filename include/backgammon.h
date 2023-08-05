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

Backgammon *backgammon_new(int argc, char *argv[]);
void backgammon_run(Backgammon *bg);

void backgammon_next_step(Backgammon *bg);
void backgammon_next_turn(Backgammon *bg);
Player *backgammon_current_player(Backgammon *bg);

void backgammon_move_piece(Backgammon *bg, gint source, gint destiny);
gboolean backgammon_player_can_move(Backgammon *bg);

#endif
