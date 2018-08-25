#include "serial.h"

int main(int argc, const char *argv[]) {
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
	memset(cmdbuff, 0, CMDBUFF_MAX);

	for (int i = 1; i < argc; ++ i) {
		if (i > ARGS_MAX) break;

		strcat(cmdBuff, argv[i]);
	        strcat(cmdBuff, " ");
	}
        strcat(cmdBuff, "\n");

	bool success = send_command(cmdBuff);
	if (!success) ERROR("Faild getting response.\n")

	return success;
}
