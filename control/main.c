#include "serial.h"
#include "run.h"
#include <signal.h> //  our new library

volatile sig_atomic_t flag = 0;

void sig_handler(int sig){ // can be called asynchronously
	flag = sig; // set flag
}

int main(int argc, const char *argv[]) {
	signal(SIGINT, sig_handler);

	continue_when_possible(PIDFILE_PATH); /* PIDFILE_PATH at run.h */

	bool success = main_wrapper(argc, argv);

	all_done(PIDFILE_PATH);

	return success ? 0 : 1;
}
