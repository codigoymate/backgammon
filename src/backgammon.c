#include <backgammon.h>
#include <utils.h>

#include <player.h>

void bg_free(Backgammon *bg);

static void bg_on_window_destroyed(GtkApplicationWindow *window, gpointer data) {
	bg_free(data);
}

static void new_game_menu_item_activate(GtkMenuItem *menu_item, gpointer data) {
	Backgammon *bg = (Backgammon *) data;

	if (bg->status != S_NOT_PLAYING) {
		if (!question(GTK_WIDGET(bg->window), "¿Finalizar partida actual?")) return;
	}

	bg->player[0].play_func = human_play_func;
	bg->player[0].name = g_string_new("Humano");
	bg->player[0].piece = BLACK;
	bg->player[0].direction = 1;
	bg->player[1].play_func = human_play_func;
	bg->player[1].name = g_string_new("IA");
	bg->player[1].piece = WHITE;
	bg->player[1].direction = -1;

	board_init(bg->board);
	board_redraw(bg->board);

	bg_next_step(bg);

}

static void end_turn_button_clicked(GtkWidget *button, gpointer data) {
	Backgammon *bg;
	bg = (Backgammon *) data;

	bg_next_turn(bg);
	bg_next_step(bg);
}

Backgammon *bg_new(int argc, char *argv[]) {
	Backgammon *bg;
	GtkBuilder *builder;
	GtkCssProvider *css_provider;
	GdkScreen *screen;

	bg = (Backgammon *) g_malloc(sizeof(Backgammon));

	gtk_init(&argc, &argv);

	css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(css_provider, "ui/styles.css", NULL);

	screen = gdk_screen_get_default();
	gtk_style_context_add_provider_for_screen(screen,
		GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "ui/main-window.glade", NULL);

	bg->window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(builder, "main-window"));
	g_signal_connect(bg->window, "destroy", G_CALLBACK(bg_on_window_destroyed), bg);

	bg->turn_label = GTK_LABEL(gtk_builder_get_object(builder, "turn-label"));
	bg->action_label = GTK_LABEL(gtk_builder_get_object(builder, "action-label"));

	bg->end_turn_button = GTK_BUTTON(gtk_builder_get_object(builder, "end-turn-button"));
	gtk_widget_set_sensitive(GTK_WIDGET(bg->end_turn_button), FALSE);

	g_signal_connect(
		gtk_builder_get_object(builder, "new-game-menu-item"),
		"activate",
		G_CALLBACK(new_game_menu_item_activate), bg
	);

	g_signal_connect(
		gtk_builder_get_object(builder, "quit-menu-item"),
		"activate",
		G_CALLBACK(gtk_main_quit), NULL
	);

	g_signal_connect(
		bg->end_turn_button,
		"clicked",
		G_CALLBACK(end_turn_button_clicked), bg
	);

	bg->board = board_new(builder, bg);

	g_object_unref(builder);

	bg->player_turn = -1;
	bg->status = S_NOT_PLAYING;

	return bg;
}

void bg_run(Backgammon *bg) {
	gtk_widget_show_all(GTK_WIDGET(bg->window));
	gtk_main();
}

void bg_free(Backgammon *bg) {
	board_free(bg->board);
	g_string_free(bg->player[0].name, TRUE);
	g_string_free(bg->player[1].name, TRUE);
	g_free(bg);
	gtk_main_quit();
}

void bg_next_step(Backgammon *bg) {

	if (bg->status == S_NOT_PLAYING) {
		bg->player_turn = -1;
		bg_next_turn(bg);
	}

	bg_current_player(bg)->play_func(bg);
}

void bg_next_turn(Backgammon *bg) {

	GString *str;

	bg->player_turn ++;
	if (bg->player_turn >= 2) bg->player_turn = 0;

	str = g_string_new("Turno de ");
	str = g_string_append(str, bg_current_player(bg)->name->str);
	gtk_label_set_text(bg->turn_label, str->str);

	g_string_free(str, TRUE);

	bg->status = S_ROLL_DICE;
}

Player *bg_current_player(Backgammon *bg) {
	return &bg->player[bg->player_turn];
}

Player *bg_opponent(Backgammon *bg) {
	return &bg->player[!bg->player_turn];
}

Player *bg_player_by_data(Backgammon *bg, gint data) {
	Player *player;
	player = bg_current_player(bg);
	if (player->direction * data > 0) return player;
	
	return bg_opponent(bg);
}

void bg_move_piece(Backgammon *bg, gint source, gint destiny) {
	gint len, i;
	// Distancia recorrida
	len = destiny - source;
	if (len < 0) len *= -1;
	// Desactiva el dado según la distancia
	for (i = 0; i < (bg->board->dice[0] == bg->board->dice[1] ? 4 : 2); i ++) {
		if (bg->board->consumed_dice[i]) continue;
		if (bg->board->dice[i % 2] == len) {
			bg->board->consumed_dice[i] = TRUE;
			break;
		}
	}

	// Descuenta source
	bg->board->places[source].data -= bg_current_player(bg)->direction;

	// Si el destino no es enemigo
	if (bg->board->places[destiny].data * bg_current_player(bg)->direction >= 0) {
		bg->board->places[destiny].data += bg_current_player(bg)->direction;
	} else {
		// Si es enemigo ...
		// Coloca la ficha en prision
		bg->board->prison[bg->player_turn] += bg_current_player(bg)->direction * -1;
		bg->board->places[destiny].data = bg_current_player(bg)->direction;
	}
}

gboolean bg_player_can_move(Backgammon *bg) {
	gint i;
	Board *b;

	b = bg->board;

	for (i = 0; i < (b->dice[0] == b->dice[1] ? 4 : 2); i ++) {
		if (!b->consumed_dice[i]) return TRUE;
	}

	return FALSE;
}
