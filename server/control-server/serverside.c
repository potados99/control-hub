#include "serverside.h"
#include "serial.h"

extern int socketfd;
extern int serialfd;
extern const char *tempDir;

void action(char *recieved) {
	char pid[PIDBUF_SIZE] = {0,};
	char cmd[CMDBUF_SIZE] = {0,};
	char feedback[CMDBUF_SIZE] = {0,};
	int resfd;

	divide(recieved, pid, DIVIDER, cmd);

	resfd = open_res_pipe(pid);

	serial_send(serialfd, TERMINATE, cmd, feedback, sizeof(feedback)); // send command and recieve feedback

        ipc_write(resfd, TERMINATE, feedback); // pass the feedback to client

        close(resfd);
}

int open_res_pipe(const char *pid) {
	char pidfifo[32] = {0,};

	join(pidfifo, tempDir, pid);
	return open_pipe(pidfifo, O_WRONLY);
}

bool check_socket(uint16_t port0) {
	int rc = 1;
	uint16_t port = port0;

        if (socketfd == -1 || rc) {
        	socketfd = -1;
	        rc = 1;

	        if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) ERROR("Could not create socket.\n")

	        struct sockaddr_in name;
	        name.sin_family = AF_INET;
	        name.sin_port = htons (port);
	        name.sin_addr.s_addr = htonl (INADDR_ANY);
	        rc = bind (socketfd, (struct sockaddr *) &name, sizeof (name));
        }

        return (socketfd != -1 && rc == 0);
}

void sig_handler(int sig) {
	done();
	printf("\nExiting program.\n");
	exit(0);
}

void done() {
	if (socketfd != -1) {
		close(socketfd);
	}

	if (serialfd != -1) {
		close(serialfd);
	}
}
