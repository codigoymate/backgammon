#ifndef PLAYER_H
#define PLAYER_H

#include <glib.h>

typedef struct player_t {
	GString *name;
	gint piece;
	gint direction;
	void (*play_func)(void *);
} Player;

void human_play_func(void *bg);
void ia_play_func(void *bg);

#endif
