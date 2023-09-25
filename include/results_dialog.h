#ifndef RESULTS_DIALOG_H
#define RESULTS_DIALOG_H

#include <gtk/gtk.h>
#include <backgammon.h>
#include <player.h>

typedef struct results_dialog_t {
	GtkWindow *window;
	GtkButton *ok_button;
	GtkLabel *winner_label, *total_pl1_label, *total_pl2_label;
	Player *winner;
	guint winner_score;
	Backgammon *bg;
} ResultsDialog;

ResultsDialog *results_dialog_new(Backgammon *bg, Player *winner, guint winner_score);
void results_dialog_show(ResultsDialog *dialog);

#endif
