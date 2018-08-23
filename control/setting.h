#ifndef setting_h
#define setting_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"

#define FILE_BUFFER_MAX 1024
#define SETTING_LENG_MAX 64

void get_setting(const char *_section, const char *_key, char *_val_out);
int read_file(const char *_filePath, char *_file_out);

bool _get_section(char *_fbuff, int *pos, const char *_section);
bool _get_key_value(char *_fbuff, int *pos, const char *_key, char *_val_out);

bool _split(char *_origin, char _delim, char *_out1, char *_out2);

#endif
