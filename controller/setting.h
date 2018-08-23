#ifndef setting_h
#define setting_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_BUFFER_MAX 1024
#define SETTING_KEY_LENG_MAX 64
#define SETTING_VAL_LENG_MAX 64


void get_setting(const char *_section, const char *_key, char *_returnVal);
int read_file(const char *_filePath, char *_dest);




#endif
