#ifndef read_h
#define read_h

#include "ipc.h"
#include "strutils.h"
#include "serial.h"

#include <netinet/in.h>
#include <signal.h>

#define REQ_PIPE_NAME "req"

#define PIDBUF_SIZE 16
#define CMDBUF_SIZE 32

void action(char *recieved);

int open_res_pipe(const char *pid);

bool check_socket(uint16_t port0);

void sig_handler(int sig);
void done(void);

#endif /* read_h */
