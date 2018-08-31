#include "serial.h"
#include "run.h"

int main(int argc, const char *argv[]) {
	LOG("Flag 1\n")

	continue_when_possible();

	LOG("Flag suc-continue\n")

int cnt = 0;
	for (;;) {
		printf("%s, %d\n", "doing my job!", cnt);
		usleep(1000 * 1000 * 1);

		if (cnt++ > 10) {
			dequeue_pid(getpid());
			return 0;
		}
	}



	if (argc < ARGS_MAX - 1) {
		fprintf(stderr, "Arguments not enough.\n");
		fprintf(stderr, "Usage: $EXCUTABLE [arg1] ...\n");
		exit(1);
	}

	if (argc - 1 > ARGS_MAX) {
		fprintf(stderr, "Too many arguments. Max is %d.\n", ARGS_MAX);
		exit(1);
	}

	char cmdBuff[CMDBUFF_MAX] = {0,};
	memset(cmdBuff, 0, CMDBUFF_MAX);

	for (int i = 1; i < argc; ++ i) {
		if (i > ARGS_MAX) break;

		strcat(cmdBuff, argv[i]);
		if (i < argc - 1) strcat(cmdBuff, " ");
	}
	strcat(cmdBuff, "\n");

	bool success = send_command(cmdBuff);
	if (!success) ERROR("Faild getting response.\n")

	dequeue_pid(getpid());

	return success;
}
