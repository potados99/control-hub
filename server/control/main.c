#include "clientside.h"

#define TMPDIR "/home/potados/.control"

int resfd = -1;
char resFifoPath[32];

int main(int argc, const char * argv[]) {
	signal(SIGINT, sig_handler);

        if (argc < 2) {
                printf("Not enough arguments!\n");
                return -1;
        }

	// pid and pid string
        int pid = getpid();
	char pidStr[16] = {0,};
	sprintf(pidStr, "%d", pid);

	// fifo pipe file path for recieving response
	join(resFifoPath, TMPDIR, pidStr);

	// same for sending request
	char reqFifoPath[32] = {0,};
	join(reqFifoPath, TMPDIR, REQ_PIPE_NAME);
        make_pipe(resFifoPath);

	// open
        resfd = open_pipe(resFifoPath, O_RDONLY | O_NONBLOCK);
        int reqfd = open_pipe(reqFifoPath, O_WRONLY | O_NONBLOCK);

	// write request
        char wbuf[64] = {0,};
	join(wbuf, pidStr, argv[1]);
        ipc_write(reqfd, TERMINATE, wbuf);
        close(reqfd);

	//listen
	struct listenparam params;
	params.fd = resfd;
	params.delim = TERMINATE;
	params.behavior = L_CLBCK;
	params.callback = on_response;

        ipc_listen(params);
        close(resfd);
        remove_pipe(resFifoPath);

        return 0;
}
