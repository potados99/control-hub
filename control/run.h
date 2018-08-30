#ifndef run_h
#define run_h

#include "macros.h"
#include "rsyslog.h"

#define PIDFILE_PATH "/run/control/control.pid"
#define PIDFILE_BUF_MAX 128

void continue_when_possible();

void all_done();

void dequeue_pid();

#endif
