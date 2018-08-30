#ifndef run_h
#define run_h

#include <stdio.h>
#include <unistd.h>
#include "macros.h"

#define PIDFILE_PATH "/run/control/control.pid"
#define PIDFILE_BUF_MAX 128

void continue_when_possible();

void all_done();

int read_pid();

void dequeue_pid();

#endif
