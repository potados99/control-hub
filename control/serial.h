#ifndef serial_h
#define seiral_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include "macros.h"

#define COMMAND_MAX 64

bool send_command(const char *command);

#endif
