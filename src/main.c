#include <backgammon.h>

int main(int argc, char *argv[]) {
	Backgammon *bg = bg_new(argc, argv);
	bg_run(bg);

	return 0;
}
