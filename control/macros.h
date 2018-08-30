#ifndef marcos_h
#define marcos_h

#include <stdio.h>
#include <stdlib.h>

typedef enum bool {
	TRUE = 1,
	FALSE = 0
} bool;

#define ERROR(x) { fprintf(stderr, "[ERROR] %s", x); exit(1); }
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#ifdef VERBOSE
#define LOG(x) fprintf(stdout, "%s", x);
#define LOGF(x, ...) fprintf(stdout, x, __VA_ARGS__);
#else
#define LOG(x);
#define LOGF(x, ...);
#endif

#endif
