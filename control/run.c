#include "run.h"
#include "serial.h"

extern volatile sig_atomic_t flag;

int main_wrapper(int argc, const char *argv[]) {
  #ifdef TEST
  for (int cnt = 0; cnt < 3; ++ cnt) {
    printf("%s, %d\n", "Test: Doing my job!", cnt);
    usleep(1000 * 1000 * 1);
  }
  all_done(PIDFILE_PATH);
  exit(0);
  #endif

  if (argc < ARGS_MAX - 1) {
    fprintf(stderr, "Arguments not enough.\n");
    fprintf(stderr, "Usage: $EXCUTABLE [arg1] ...\n");
    ERROR("Args\n")
  }

  if (argc - 1 > ARGS_MAX) {
    fprintf(stderr, "Too many arguments. Max is %d.\n", ARGS_MAX);
    ERROR("Args\n")
  }

  char cmdBuff[CMDBUFF_MAX] = {0,};
  memset(cmdBuff, 0, CMDBUFF_MAX);

  for (int i = 1; i < argc; ++ i) {
    if (i > ARGS_MAX) break;

    strcat(cmdBuff, argv[i]);
    if (i < argc - 1) strcat(cmdBuff, " ");
  }
  strcat(cmdBuff, "\n");

  bool success = send_command(cmdBuff);
  if (!success) ERROR("Faild getting response.\n")

  return success;
}

void continue_when_possible(char *pidfile) {
  int mypid = getpid(); /* To prevent overhead, save it to variable. */
  int pidRead = read_pid(pidfile);

  add_pid(pidfile);

  if (pidRead == 0) return; /* No one here. It's my turn. */

  time_t t_start = time(NULL);
  time_t t_cur;
  for(;;) {
    ///////////////////////////
    if (flag) {
      all_done(PIDFILE_PATH);
      LOGF("Exit with %d.\n", flag)
      exit(flag);
    }
    ///////////////////////////

    t_cur = time(NULL);
    pidRead = read_pid(pidfile);

    if (pidRead == mypid) return; /* Yeah let's go. */
    else if (pidRead == 0) { /* File is being modified.(or deleted.) Do nothing. */
      /*
      File seems to be under modification.
      If this status continues for 2 seconds, it is sure that the file is gone.
      */
      if (t_cur - t_start >= APP_TIMEOUT) {
        add_pid(PIDFILE_PATH);
        return;
      }
    }
    else {
      /*
      Other's turn.
      Wait for up to 2 seconds.
      */
      if (t_cur - t_start >= APP_TIMEOUT) {
        // Wait only for 2 seconds.
        t_start = time(NULL);
        remove_pid(PIDFILE_PATH, pidRead);
      }
    } /* End of if */
  } /* End of for */
}

void all_done(char *pidfile) {
  remove_pid(pidfile, getpid());
}

int read_pid (char *pidfile) {
  FILE *fp =fopen(pidfile,"r");
  if (!fp) return 0; /* Having no file is not an exception. */

  int pid = 0;
  fscanf(fp,"%d", &pid);
  fclose(fp);

  return pid;
}

int add_pid (char *pidfile) {
  FILE *fp = fopen(pidfile, "a"); // append at the end
  if (!fp) ERRORF("add_pid: Can't open or create %s.\n", pidfile);

  int pid = getpid();
  if (!fprintf(fp,"%d\n", pid)) ERROR("add_pid: Failed writing pid to file.\n")

  fclose(fp);

  return pid;
}

void remove_pid(char *pidfile, int pid) {
  char fbuf[PIDFILE_BUF_MAX];
  memset(fbuf, 0, sizeof(fbuf));
  int fsize = _read_all(pidfile, fbuf);
  if (fsize == 0) ERROR("remove_pid: Pid file is empty. File must have been modified by other processes.\n");

  LOG("Successfully read pid file.\n")

  /*
  First, check if the line (null terminated) contains pid.
  Second, if so, move pointer to next line. if not, copy that line and move pointer to next line.
  Do these steps until we meet End Of File.
  */

  FILE *fp_write = fopen(pidfile, "w+");

  if (!fp_write) ERRORF("remove_pid: Failed opening pid file for writing: Can't open or create %s\n", pidfile)

  for (int i = 0; fbuf[i] != EOF; ++ i) {
    if (!fbuf[i]) break; /* For safety. */
    if (fbuf[i] == '\n') fbuf[i] = 0x00; /* Make line null-terminated. */
  }

  char * curLocation = fbuf; /* Setup a pointer to indicate current location. */
  while (curLocation - fbuf < fsize) {
    if (atoi(curLocation) == pid) {
      // The pid. pass this line.

      LOGF("Pid file: Remove %d\n", pid)

      curLocation += strlen(curLocation) + 1;
    }
    else if (*curLocation) {
      /*
      This buffer is filled with 0 because we did memset.
      So it is sure that if the value of curLocation is not zero,
      it is a valid character.
      */

      if (!fprintf(fp_write, "%s\n", curLocation)) ERROR("remove_pid: Failed writing pid to file.\n")

      LOGF("Pid file: Left %s\n", curLocation)

      curLocation += strlen(curLocation) + 1;
    }
    else {
      break;
    } /* End of if */
  } /* End of while */

  LOG("Successfully wrote pid file.\n")

  fclose(fp_write);
}

int _read_all(char *filePath, char *outBuffer) {
  // Important: the outBuffer must be allocated and wrote 0.

  FILE *fp_read = fopen(filePath, "r+");
  if (!fp_read) ERRORF("_read_all: Failed opening file for reading: Can't open or create %s\n", filePath)

  // Get file size
  fseek(fp_read, 0, SEEK_END);
  int fsize = (int)ftell(fp_read);
  if (fsize == 0) {
    fclose(fp_read);
    return 0;
  }
  fseek(fp_read, 0, SEEK_SET);

  // Read file and save it to buffer
  char *fbuf = outBuffer;
  if (fread(fbuf, fsize, 1, fp_read) < 1) ERRORF("_read_all: Failed reading file: %s\n", filePath)

  fclose(fp_read);

  return fsize;
}
