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

#define PIDFILE_PATH "/run/control/control.pid"
#define PIDFILE_BUF_MAX 128

void continue_when_possible();

void all_done();

int read_pid();

int write_pid (char *pidfile);

void dequeue_pid(int pid);

#endif
