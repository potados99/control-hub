#include "setting.h"

int main(int argc, const char *argv[]) {
	char buffer[SETTING_LENG_MAX] = {0,};

	if (argc < 3) {
	    fprintf(stderr, "Arguments not enough.\n");
	    fprintf(stderr, "Usage: $EXCUTABLE [section] [key].\n");
	    exit(1);
	}
	get_setting(argv[1], argv[2], buffer);

	printf("%s\n", buffer);

}
