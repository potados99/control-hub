#ifndef run_h
#define run_h

#include <unistd.h>
#include <string.h>
#include "macros.h"
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#define PIDFILE_PATH "/var/run/control/control.pid"
#define PIDFILE_BUF_MAX 128

void continue_when_possible();

void all_done();

int read_pid();

int add_pid (char *pidfile);

void remove_pid(char *pidfile, int pid);

#endif
