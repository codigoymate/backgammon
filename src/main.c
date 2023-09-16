#include <backgammon.h>

int main(int argc, char *argv[]) {
	Backgammon *bg = backgammon_new(argc, argv);
	backgammon_run(bg);

	return 0;
}
