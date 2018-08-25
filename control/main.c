#include "serial.h"

int main(int argc, const char *argv[]) {
	if (argc < ARGS_MAX - 1) {
		fprintf(stderr, "Arguments not enough.\n");
		fprintf(stderr, "Usage: $EXCUTABLE [section]\n");
		exit(1);
	}

	if (argc > ARGS_MAX) ERROR("Too many arguments.\n")

	char cmdBuff[CMDBUFF_MAX] = {0,};
	memset (cmdBuff, 0, CMDBUFF_MAX);

	for (int i = 1; i < argc; ++ i) {
		if (strlen(argv[i]) > 0)
			strcat(cmdBuff, argv[i]);
		strcat(cmdBuff
	}

	if (! send_command(cmdBuff)) ERROR("Failed getting response.\n")
}
