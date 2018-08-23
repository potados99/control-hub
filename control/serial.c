#include "serial.h"
#include "setting.h"

#define SECTION_KEY "device"
#define PORT_KEY "port"
#define BDRT_KEY "boadrate"

bool send_command(const char *command) {
        char port[SETTING_LENG_MAX] = {0,};
        get_setting(SECTION_KEY, PORT_KEY, port);

	int boadrate = 0;
	char boadrateStr[SETTING_LENG_MAX] = {0,};
	get_setting(SECTION_KEY, BDRT_KEY, boadrateStr);
	boadrate = atoi(boadrateStr);

  //  char byte;
    int fd = open(port, O_RDWR);
    write(fd, command, 64);
 //   ssize_t size = read(fd, &byte, 1);
 //   printf("Read byte %c\n", byte);
}
