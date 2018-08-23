#ifndef serial_h
#define seiral_h

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include "macros.h"

#define COMMAND_MAX 3
#define CMDBUFF_MAX 64

bool send_command(const char *command);

int set_interface_attribs(int fd, int speed, int parity);
void set_blocking(int fd, int should_block);

#endif
