/**
 * @file undo.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief Undo module for save and restore board
 * @date 2023-09-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UNDO_H
#define UNDO_H

#include <glib.h>

/**
 * @brief Undo data type stores board status
 * 
 */
typedef struct undo_t {
	gint places[24];
	gint prison[2];
	gint goal[2];
} Undo;

/**
 * @brief Backup the board to undo later
 * 
 * @param bgp Backgammon instance
 */
void undo_backup(void *bgp);

/**
 * @brief Restore board from backup
 * 
 * @param bgp Backgammon instance
 */
void undo_restore(void *bgp);

#endif
