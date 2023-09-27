/**
 * @file results_dialog.h
 * @brief "Results" Dialog module
 * @date 2023-09-27
 * 
 * @author Javier Candales
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef RESULTS_DIALOG_H
#define RESULTS_DIALOG_H

#include <gtk/gtk.h>
#include <backgammon.h>
#include <player.h>

/**
 * @brief ResultDialog data type
 * 
 */
typedef struct results_dialog_t {
	GtkWindow *window;
	GtkButton *ok_button;
	GtkLabel *winner_label, *total_pl1_label, *total_pl2_label;
	Player *winner;
	guint winner_score;
	Backgammon *bg;
} ResultsDialog;

/**
 * @brief Creates a new instance of "ResultDialog."
 * 
 * @param bg Instance of Backgammon
 * @param winner Instance of the winning player
 * @param winner_score Score obtained by the winning player
 * @return ResultsDialog* ResultDialog instance
 */
ResultsDialog *results_dialog_new(Backgammon *bg, Player *winner, guint winner_score);

/**
 * @brief Shows the "ResultDialog."
 * 
 * @param dialog Instance of ResultDialog
 */
void results_dialog_show(ResultsDialog *dialog);

#endif
