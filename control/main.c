#include "setting.h"

int main(int argc, const char *argv[]) {
	char buffer[SETTING_VAL_LENG_MAX] = {0,};

	get_setting("device", "port", buffer);

	printf("%s\n", buffer);

}
