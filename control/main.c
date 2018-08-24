#include "serial.h"

int main(int argc, const char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Arguments not enough.\n");
		fprintf(stderr, "Usage: $EXCUTABLE [section] [key].\n");
		exit(1);
	}

	char cmdBuff[COMMAND_MAX] = {0,};
	memset (cmdBuff, 0, COMMAND_MAX);

	for (int i = 1; i < argc; ++ i) {
		if (strlen(argv[i]) > 0)
		strcat(cmdBuff, argv[i]);
	}

	send_command(cmdBuff);
}
