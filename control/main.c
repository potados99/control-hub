#include "serial.h"

int main(int argc, const char *argv[]) {
        if (argc < 2) {
            fprintf(stderr, "Arguments not enough.\n");
            fprintf(stderr, "Usage: $EXCUTABLE [arg1] [arg2].\n");
            exit(1);
        }

	if (argc - 1 > COMMAND_MAX) {
		fprintf(stderr, "Too many arguments. Max is 3.\n");
		exit(1);
	}

	char cmdBuff[CMDBUFF_MAX] = {0,};
	for (int i = 0; i < sizeof(cmdBuff); ++ i) {
		cmdBuff[i] = 0x00;
	}

	for (int i = 1; i < argc; ++ i) {
		if (i > COMMAND_MAX) break;

		strcat(cmdBuff, argv[i]);
	        strcat(cmdBuff, " ");
	}
        strcat(cmdBuff, "\n");

	bool success = send_command(cmdBuff);
	if (!success) ERROR("Faild getting response.\n")
	
	return success;
}
