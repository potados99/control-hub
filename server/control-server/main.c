#include "serverside.h"

#define SOCKET_PORT_NUMBER 5555

int serialfd = -1;
int socketfd = -1;
const char *tempDir;

int main(int argc, const char * argv[]) {
	signal(SIGINT, sig_handler);

	if (! check_socket(SOCKET_PORT_NUMBER)) ERROR("Other instance is already running.\n")

	if (argc < 4) {
		printf("Too less arguments!\n");
		printf("Usage: [executable] [temp directory] [serial port] [baudrate]\n");
		return 1;
	}

	// get arguments
	tempDir = argv[1];
	const char *serialPort = argv[2];
	int baudrate = atoi(argv[3]);
	printf("Program loaded with arguments:\n");
	printf("  temp dir:	%s\n", tempDir);
	printf("  serial port:	%s\n", serialPort);
	printf("  baudrate:	%d\n\n", baudrate);

	// open serial port
	serialfd = serial_open(serialPort, baudrate);
	printf("Opened serial port at %s.\n", serialPort);

	// recreate req fifo pipe file
	char reqFifoPath[32] = {0,};
	join(reqFifoPath, tempDir, REQ_PIPE_NAME);
        remove_pipe(reqFifoPath);
        make_pipe(reqFifoPath);

	// open
        int listenfd = open_pipe(reqFifoPath, O_RDONLY | O_NONBLOCK);
	printf("Opened request pipe at %s.\n", reqFifoPath);

	// listen
	struct listenparam params;
	params.fd = listenfd;
	params.delim = TERMINATE;
	params.behavior = L_KEEP | L_CLBCK;
	params.callback = action;

	// empty serial buffer
	sleep(2);
	tcflush(serialfd, TCIOFLUSH);

	printf("Start listening.\n");
        ipc_listen(params);

        return 0;
}

