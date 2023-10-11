#include <backgammon.h>

#include <locale.h>
#include <libintl.h>

int main(int argc, char *argv[]) {
	Backgammon *bg;

	//setlocale(LC_ALL, "");
	bindtextdomain("backgammon", "./po");
	textdomain("backgammon");

	bg = bg_new(argc, argv);
	bg_run(bg);

	return 0;
}
