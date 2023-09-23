/**
 * @file new_dialog.h
 * @author Javier Candales (javier_candales@yahoo.com.ar)
 * @brief "New Game" Dialog module
 * @date 2023-09-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef NEW_DIALOG_H
#define NEW_DIALOG_H

#include <gtk/gtk.h>
#include <backgammon.h>

/**
 * @brief NewDialog data type
 * 
 */
typedef struct new_dialog_t {
	GtkWindow *window;
	GtkEntry *pl1_entry, *pl2_entry;
	GtkComboBoxText *pl1_combo, *pl2_combo;
	GtkButton *clockwise_button, *piece_button;
	GtkImage *clockwise_image, *piece_image;
	GtkAdjustment *score_adj;
	Backgammon *bg;
	gboolean clockwise, white;
} NewDialog;

/**
 * @brief creates the "New Game" instance
 * 
 * @param bg Backgammon instance
 * @return NewDialog* NewDialog instance
 */
NewDialog *new_dialog_new(Backgammon *bg);

/** 
 * @brief Show the "New Game" dialog
 * 
 * @param dialog NewDialog instance
 */
void new_dialog_show(NewDialog *dialog);

#endif
