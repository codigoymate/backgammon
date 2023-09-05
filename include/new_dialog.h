#ifndef NEW_DIALOG_H
#define NEW_DIALOG_H

#include <gtk/gtk.h>
#include <backgammon.h>

typedef struct new_dialog_t {
	GtkWindow *window;
	GtkEntry *pl1_entry, *pl2_entry;
	GtkComboBoxText *pl1_combo, *pl2_combo;
	GtkButton *clockwise_button, *piece_button;
	GtkImage *clockwise_image, *piece_image;
	Backgammon *bg;
	gboolean clockwise, white;
} NewDialog;

NewDialog *new_dialog_new(Backgammon *bg);
void new_dialog_show(NewDialog *dialog);

#endif
