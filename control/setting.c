#include "setting.h"

#define CONFIG_FILE_PATH "/home/potados/.control/config.txt"
#define SECTION_OPEN '['
#define SECTION_CLOSE ']'
#define SPACE ' '
#define EQUAL '='
#define LINE_FEED '\n'

#define ERROR(x) fprintf(stderr, "%s", x);
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

void get_setting(const char *_section, const char *_key, char *_val_out) {
	// read file
	char fbuff[FILE_BUFFER_MAX];
	int fsize = read_file(CONFIG_FILE_PATH, fbuff);
	int pos = 0;

	bool inTargetSection = false;

	while (pos < fsize) {
		switch (fbuff[pos]) {

			case SECTION_OPEN:
					inTargetSection = _get_section(fbuff, &pos, _section);
				break;

			case SPACE:
				fprintf(stderr, "Space not allowed");
				break;

			default:
				if (_get_key_value(fbuff, &pos, _key, _val_out)) return;
				break;
		} /* switch end */
		++ pos;
	} /* while end */
	printf("Key not found.");
}


int read_file(const char *_filePath, char *_file_out) {
	char buffer[FILE_BUFFER_MAX];

        // opening file for reading
        FILE *fp;
        fp = fopen(_filePath, "r");

        // if fopen returned NULL
        if (fp == NULL) ERROR("Config: File doesn't exist\n");

        // get file size
        fseek(fp, 0, SEEK_END);
        int fileSize = (int)ftell(fp);
        fseek(fp, 0, SEEK_SET);

	if (fileSize == 0) ERROR("File size is zero.\n")

        // read file and save it to buffer
        if (fread(buffer, fileSize, 1, fp) < 1) fclose(fp); ERROR("Config: Failed reading file\n")

        // done with file
        fclose(fp);

	snprintf(_dest, FILE_BUFFER_MAX, "%s", buffer);
	return fileSize;
}


bool _get_section(char *_fbuff, int *pos, const char *_section) {
	char *begin = _fbuff + *pos + 1;
	char *end = strchr(begin, SECTION_CLOSE);
	if (end == NULL) ERROR("Config: Section not closed\n")

	int stringLength = (int)end - (int)begin;
	if (stringLength > SETTING_LENG_MAX) ERROR("Config: Section name is too long\n")

	char curSection[SETTING_LENG_MAX] = 0;
	memcpy(curSection, begin, stringLength);
	curSection[strlen(curSection) - 1] = '\0'; /* make it null-terminated */

	bool inTargetSection = (strcmp(curSection, _section) == 0) ? true : false;

	*pos += stringLength + 1;

	return inTargetSection;
}


bool _get_key_value(char *_fbuff, int *pos, const char *_key, char *_val_out) {
	char *begin = _fbuff + *pos + 1;
	char *end = strchr(begin, LINE_FEED);
	if (end == NULL) ERROR("Config: Line not completed\n")

	int stringLength = (int)end - (int)begin;
	if (stringLength > SETTING_LENG_MAX) ERROR("Config: Key and value line is too long\n")

	char curKeyValLine[SETTING_LENG_MAX*2 + 1] = 0;
	memcpy(curKeyValLine, begin, stringLength);
	curKeyValLine[strlen(curSection) - 1] = 0x00; /* make it null-terminated */

	char val[SETTING_LENG_MAX] = 0;
	char key[SETTING_LENG_MAX] = 0;

	if (! _split_by_token(curKeyValLine, EQUAL, key, val)) ERROR("Config: Failed getting key and value from string.\n")

	*pos += stringLength + 1;

	if (!strcmp(key, _key)) {
		snprintf(_val_out, SETTING_LENG_MAX, "%s", val);
		return true;
	}
	else {
		return false;
	}
}

bool _split(char *_origin, char _delim, char *_out1, char *_out2) {
	if (_origin == NULL) return false;
	if (_origin[strlen(_origin) - 1] != 0x00) return false;
	// _origin MUST be a NULL-terminated string.
	int count 0;
	for (int i = 0; i < strlen(_origin); ++ i) {
		if (_origin[i] == _delim) ++ count;
		if (count > 1) return false;
	}
	// when EQUAL(_delim) apears more than once.

	char *keyBegin = _origin + 1;
	char *keyEnd = strchr(keyBegin, _delim);
	if (keyEnd == NULL) return false;

	char *valBegin = keyBegin + 1;
	char *valEnd = strchr(valBegin, 0x00); /* end of _origin MUST be NULL */
	if (valEnd == NULL) return false;

	memcpy(_out1, keyBegin, keyEnd - keyBegin);
	memcpy(_out2, valBegin, valEnd - valBegin);

	_out1[strlen(_out1) - 1] = 0x00;
	_out2[strlen(_out2) - 1] = 0x00;

	return true;
}
