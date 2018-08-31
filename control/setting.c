#include "setting.h"

void get_setting(const char *_section, const char *_key, char *_val_out) {
	// read file
	char fbuff[FILE_BUFFER_MAX];
	memset(fbuff, 0, sizeof(fbuff));

	char pathBuf[SETTING_LENG_MAX];
	memset(pathBuf, 0, sizeof(pathBuf));

	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;

	strcat(pathBuf, homedir);
	strcat(pathBuf, "/");
	strcat(pathBuf, CONFIG_FILE_PATH);

	int fsize = read_file(pathBuf, fbuff);
	int pos = 0;

	bool inTargetSection = FALSE;

	while (pos < fsize) {
		switch (fbuff[pos]) {
			case SECTION_OPEN:
			inTargetSection = _get_section(fbuff, &pos, _section);
			break;

			case SPACE:
			ERROR("Config: Space not allowed\n")
			break;

			case LINE_FEED:
			break;

			default:
			if (is_alpha(fbuff[pos]) && inTargetSection && _get_key_value(fbuff, &pos, _key, _val_out)) return;
			break;
		} /* switch end */
		++ pos;
	} /* while end */
	printf("Key not found.\n");
}

int read_file(char *_filePath, char *_file_out) {
	// opening file for reading
	FILE *fp;
	fp = fopen(_filePath, "rb");

	// if fopen returned NULL
	if (fp == NULL) ERRORF("Config: File doesn't exist: %s\n", _filePath);

	// get file size
	fseek(fp, 0, SEEK_END);
	int fileSize = (int)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (fileSize == 0) ERROR("File size is zero.\n")

	char buffer[FILE_BUFFER_MAX];

	// read file and save it to buffer
	if (fread(buffer, fileSize, 1, fp) < 1) {
		fclose(fp);
		ERROR("Config: Failed reading file\n")
	}

	// done with file
	fclose(fp);

	snprintf(_file_out, FILE_BUFFER_MAX, "%s", buffer);
	return fileSize;
}

bool _get_section(char *_fbuff, int *pos, const char *_section) {
	char *begin = _fbuff + *pos + 1; /* the opening brace + 1 */
	char *end = strchr(begin, SECTION_CLOSE);
	if (end == NULL) ERROR("Config: Section not closed\n")

	int stringLength = (int)end - (int)begin;
	if (stringLength > SETTING_LENG_MAX) ERROR("Config: Section name is too long\n")

	char curSection[SETTING_LENG_MAX] = {0,};
	memcpy(curSection, begin, stringLength + 1);
	curSection[strlen(curSection) - 1] = 0x00; /* make it null-terminated */

	bool inTargetSection = (strcmp(curSection, _section) == 0) ? TRUE : FALSE;

	*pos += stringLength;

	return inTargetSection;
}

bool _get_key_value(char *_fbuff, int *pos, const char *_key, char *_val_out) {
	char *begin = _fbuff + *pos;
	char *end = strchr(begin, LINE_FEED);
	if (end == NULL) ERROR("Config: Line not completed\n")

	int stringLength = (int)end - (int)begin;
	if (stringLength > SETTING_LENG_MAX) ERROR("Config: Key and value line is too long\n")

	char curKeyValLine[SETTING_LENG_MAX*2 + 1] = {0,};
	memcpy(curKeyValLine, begin, stringLength + 1);
	curKeyValLine[strlen(curKeyValLine) - 1] = 0x00; /* make it null-terminated */

	LOGF("Current line: %s\n",curKeyValLine)

	char key[SETTING_LENG_MAX];
	memset(key, 0, sizeof(key));
	char val[SETTING_LENG_MAX];
	memset(val, 0, sizeof(val));

	if (! _split(curKeyValLine, EQUAL, key, val)) ERROR("Config: Failed getting key and value from string.\n")

	*pos += stringLength;

	LOGF("Key: %s, Value: %s\n", key, val)

	if (!strcmp(key, _key)) {
		snprintf(_val_out, SETTING_LENG_MAX, "%s", val);
		return TRUE;
	}
	else {
		return FALSE;
	}
}

bool _split(char *_origin, char _delim, char *_out1, char *_out2) {
	if (_origin == NULL) return FALSE;

	// _origin MUST be a NULL-terminated string.
	int count = 0;
	for (int i = 0; i < strlen(_origin); ++ i) {
		if (_origin[i] == _delim) ++ count;
		if (count > 1) return FALSE;
	}
	// when EQUAL(_delim) apears more than once.

	char *keyBegin = _origin;
	char *keyEnd = strchr(keyBegin, _delim);
	if (keyEnd == NULL) return FALSE;

	char *valBegin = keyEnd + 1; // after delim
	char *valEnd = strchr(valBegin, 0x00); /* end of _origin MUST be NULL */
	if (valEnd == NULL) return FALSE;

	memcpy(_out1, keyBegin, keyEnd - keyBegin + 1);
	memcpy(_out2, valBegin, valEnd - valBegin + 1);

	_out1[keyEnd - keyBegin] = 0x00;
	_out2[valEnd - valBegin] = 0x00;
	return TRUE;
}

bool is_alpha(char target) {
	return ((target >= 'a' && target <= 'z') || (target >= 'A' && target <= 'Z'));
}
