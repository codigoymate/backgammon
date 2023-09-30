/**
 * @file undo.c
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Implementation of undo.h
 * @date 2023-09-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <undo.h>

#include <backgammon.h>

/**
 * @brief Backup the board to undo later
 * 
 * @param bgp Backgammon instance
 */
void undo_backup(void *bgp) {
	Backgammon *bg;
	guint i;
	bg = (Backgammon *) bgp;

	for (i = 0; i < 24; i ++) bg->undo.places[i] = bg->board->places[i].data;
	for (i = 0; i < 2; i ++) bg->undo.prison[i] = bg->board->prison[i];
	for (i = 0; i < 2; i ++) bg->undo.goal[i] = bg->board->goal[i].data;
}

/**
 * @brief Restore board from backup
 * 
 * @param bgp Backgammon instance
 */
void undo_restore(void *bgp) {
	Backgammon *bg;
	guint i;
	bg = (Backgammon *) bgp;
	// Restore dice
	for (i = 0; i < 4; i ++) bg->board->consumed_dice[i] = FALSE;

	// Restore board
	for (i = 0; i < 24; i ++) bg->board->places[i].data = bg->undo.places[i];
	for (i = 0; i < 2; i ++) bg->board->prison[i] = bg->undo.prison[i];
	for (i = 0; i < 2; i ++) bg->board->goal[i].data = bg->undo.goal[i];
}
