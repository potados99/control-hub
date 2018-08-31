#ifndef serial_h
#define serial_h

#include <termios.h>
#include <fcntl.h>

#include "macros.h"

#define SECTION_KEY "device"
#define PORT_KEY "port"
#define BDRT_KEY "baudrate"

#define ARGS_MAX 3
#define CMDBUFF_MAX 64
#define MAX_RETRY 1048576 /* 2^20 */

bool send_command(const char *command);

void set_interface_attribs(int fd, int speed, int parity);
void set_blocking(int fd, int should_block);

#endif
