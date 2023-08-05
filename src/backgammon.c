#include <backgammon.h>
#include <utils.h>

#include <player.h>

void backgammon_free(Backgammon *bg);

static void backgammon_on_window_destroyed(GtkApplicationWindow *window, gpointer data) {
	backgammon_free(data);
}

static void new_game_menu_item_activate(GtkMenuItem *menu_item, gpointer data) {
	Backgammon *bg = (Backgammon *) data;

	if (bg->status != S_NOT_PLAYING) {
		if (!question(GTK_WIDGET(bg->window), "¿Finalizar partida actual?")) return;
	}

	bg->player[0].play_func = human_play_func;
	bg->player[0].name = g_string_new("Humano");
	bg->player[0].piece = BLACK;
	bg->player[1].play_func = human_play_func;
	bg->player[1].name = g_string_new("IA");
	bg->player[1].piece = WHITE;

	backgammon_next_step(bg);

}

Backgammon *backgammon_new(int argc, char *argv[]) {
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
	g_signal_connect(bg->window, "destroy", G_CALLBACK(backgammon_on_window_destroyed), bg);

	bg->turn_label = GTK_LABEL(gtk_builder_get_object(builder, "turn-label"));
	bg->action_label = GTK_LABEL(gtk_builder_get_object(builder, "action-label"));

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

	bg->board = board_new(builder, bg);

	g_object_unref(builder);

	bg->player_turn = -1;
	bg->status = S_NOT_PLAYING;

	return bg;
}

void backgammon_run(Backgammon *bg) {
	gtk_widget_show_all(GTK_WIDGET(bg->window));
	gtk_main();
}

void backgammon_free(Backgammon *bg) {
	board_free(bg->board);
	g_string_free(bg->player[0].name, TRUE);
	g_string_free(bg->player[1].name, TRUE);
	g_free(bg);
	gtk_main_quit();
}

void backgammon_next_step(Backgammon *bg) {

	if (bg->status == S_NOT_PLAYING) {
		bg->player_turn = -1;
		backgammon_next_turn(bg);
	}

	backgammon_current_player(bg)->play_func(bg);
}

void backgammon_next_turn(Backgammon *bg) {

	GString *str;

	bg->player_turn ++;
	if (bg->player_turn >= 2) bg->player_turn = 0;

	str = g_string_new("Turno de ");
	str = g_string_append(str, backgammon_current_player(bg)->name->str);
	gtk_label_set_text(bg->turn_label, str->str);

	g_string_free(str, TRUE);

	bg->status = S_ROLL_DICE;
}

Player *backgammon_current_player(Backgammon *bg) {
	return &bg->player[bg->player_turn];
}

void backgammon_move_piece(Backgammon *bg, gint source, gint destiny) {
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
	bg->board->places[source].data -= backgammon_current_player(bg)->piece;

	// Si el destino no es enemigo
	if (bg->board->places[destiny].data * backgammon_current_player(bg)->piece >= 0) {
		bg->board->places[destiny].data += backgammon_current_player(bg)->piece;
	} else {
		// Si es enemigo ...
		// TODO: toma prisionero
		bg->board->places[destiny].data = backgammon_current_player(bg)->piece;
	}
}

gboolean backgammon_player_can_move(Backgammon *bg) {
	gint i;
	Board *b;

	b = bg->board;

	for (i = 0; i < (b->dice[0] == b->dice[1] ? 4 : 2); i ++) {
		if (!b->consumed_dice[i]) return TRUE;
	}

	return FALSE;
}
