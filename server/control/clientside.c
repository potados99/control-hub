#include "clientside.h"

extern int resfd;
extern char resFifoPath[32];

void on_response(char *recieved) {
	printf("%s\n", recieved);
}

void sig_handler(int sig) {
        done();
        printf("\nExiting program.\n");
        exit(0);
}

void done() {
        if (resfd != -1) {
                close(resfd);
		remove_pipe(resFifoPath);
        }
}
