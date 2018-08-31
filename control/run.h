#ifndef run_h
#define run_h

#include "macros.h"

#ifdef TEST
#define PIDFILE_PATH "/var/run/control/control.pid"
#else
#define PIDFILE_PATH "/run/control/control.pid"
#endif

#define PIDFILE_BUF_MAX 128

int main_wrapper(int argc, const char *argv[]);

void continue_when_possible();
void all_done(char *pidfile);

int read_pid();
int add_pid (char *pidfile);
void remove_pid(char *pidfile, int pid);

int _read_all(char *filePath, char *outBuffer);

#endif
