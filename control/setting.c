#include "setting.h"

#define CONFIG_FILE_PATH "/home/potados/.control/config.txt"
#define SECTION_OPEN '['
#define SECTION_CLOSE ']'
#define SPACE ' '
#define EQUAL '='
#define LINE_FEED '\n'

#define ERROR(x) fprintf(stderr, "%s", x);
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

void get_setting(const char *_section, const char *_key, char *_dest) {
	// read file
	char fbuff[FILE_BUFFER_MAX];
	int fsize = read_file(CONFIG_FILE_PATH, fbuff);
	int pos = 0;

	int expectingVal = 0;
	int inTargetSection = 0;
	int inTargetKey = 0;

	char curSection[SETTING_KEY_LENG_MAX];
	char curKey[SETTING_KEY_LENG_MAX];

	while (pos < fsize) {
		switch (fbuff[pos]) {

			case SECTION_OPEN: {
				char *begin = fbuff + pos + 1;
               	 		char *end = strchr(begin, SECTION_CLOSE);

	                	if (end == NULL) ERROR("config.txt: Section not closed\n")

                		int stringLength = (int)end - (int)begin;

				if (stringLength > SETTING_KEY_LENG_MAX) ERROR("Config: Section name is too long\n")

				memcpy(curSection, begin, stringLength);
				curSection[strlen(curSection) - 1] = '\0';

				if (!strcmp(curSection, _section))
					inTargetSection = 1;
				else
					inTargetSection = 0;

				pos += stringLength + 1;

				}
				break;

			case EQUAL:
				expectingVal = 1;
				break;

			case SPACE:
				break;

			case LINE_FEED:
				expectingVal = 0;
				break;

			default: {
				if (! (fbuff[pos] == '/' ||fbuff[pos] >= 'A' && fbuff[pos] <= 'Z' || fbuff[pos] >= 'a' && fbuff[pos] <= 'z')) break; /* process only when it is a character */

				char *begin = fbuff + pos;
                                char *end; /* detect end of word */
				char *endWithEqual = strchr(begin, EQUAL) - 0;
				char *endWithLF = strchr(begin, LINE_FEED) - 0;

				if (endWithLF == NULL && endWithEqual == NULL) end = NULL;
				else if (endWithLF != NULL && endWithEqual == NULL) end = endWithLF;
				else if (endWithLF == NULL && endWithEqual != NULL) end = endWithEqual;
				else end = MIN(endWithLF, endWithEqual);

                                if (end == NULL) ERROR("Config: Key or value is broken\n")

                                int stringLength = (int)end - (int)begin;
				if (stringLength > FILE_BUFFER_MAX) ERROR("Config: Key or value is too long\n")

                                char curStr[FILE_BUFFER_MAX];
				for (register int i = 0; i < FILE_BUFFER_MAX; ++ i) {
					curStr[i] = '\0';
				}
                                memcpy(curStr, begin, stringLength);
				curStr[strlen(curStr)] = '\0';

				if (expectingVal == 1 && inTargetSection == 1 && inTargetKey == 1) {
					snprintf(_dest, SETTING_VAL_LENG_MAX, "%s", curStr);
					return;
				}
				else {
					if (strcmp(curStr, _key) == 0) {
						inTargetKey = 1;
                                        	snprintf(curKey, SETTING_KEY_LENG_MAX, "%s", curStr);
					}
					else {
						inTargetKey = 0;
					}
				}

                                pos += stringLength - 1;
				}
				break;


		} /* switch end */
		++ pos;
	} /* while end */
	printf("Key not found.");
}


int read_file(const char *_filePath, char *_dest) {
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

