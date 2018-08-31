#include "run.h"

void continue_when_possible(char *pidfile) {
  int mypid = getpid();
  int pidRead = read_pid(pidfile);

  if (pidRead == 0) {
    add_pid(pidfile);
    return;
  }

  add_pid(pidfile);

  for(;;) {
    if ((pidRead = read_pid(pidfile)) == mypid) {
      return;
    }
    if (read_pid(pidfile) == 0) {
      // file is gone!
      add_pid(pidfile);
      return;
    }
    else {
      LOG("Wait!\n");
    }
    usleep(1000 * 1000 * 1);
  } /* for end */

}

void all_done(char *pidfile) {
  remove_pid(pidfile, getpid());
}

int read_pid (char *pidfile) {
  FILE *fp =fopen(pidfile,"r");
  if (!fp) return 0;

  int pid = 0;
  fscanf(fp,"%d", &pid);
  fclose(fp);

  return pid;
}

int add_pid (char *pidfile) {
  FILE *fp = fopen(pidfile, "a"); // append at the end
  if (!fp) ERRORF("Can't open or create %s.\n", pidfile);

  int pid = getpid();
  if (!fprintf(fp,"%d\n", pid)) ERROR("Failed writing pid to file.\n")

  fclose(fp);

  return pid;
}

void remove_pid(char *pidfile, int pid) {
  char fbuf[PIDFILE_BUF_MAX];
  memset(fbuf, 0, sizeof(fbuf));
  int fsize = _read_all(pidfile, fbuf);

  /*
  First, check if the line (null terminated) contains pid.
  Second, if so, move pointer to next line. if not, copy that line and move pointer to next line.
  Do these steps until we meet End Of File.
  */

  FILE *fp_write = fopen(pidfile, "w");

  for (int i = 0; fbuf[i] != EOF; ++ i) {
    if (fbuf[i] == '\n') fbuf[i] = 0x00;
  }

  char * curLocation = fbuf;

  while ((curLocation - fbuf < fsize)) {
    if (atoi(curLocation) == pid) {
      // The pid. pass this line.
      LOGF("Remove %d\n", pid)
      curLocation += strlen(curLocation) + 1;
    }
    else if (*curLocation) {
      // This buffer is filled with 0 because we did memset.
      // So it is sure that if the value of curLocation is not zero,
      // it is a valid character.
      fprintf(fp_write, "%s\n", curLocation);
      LOGF("Left pid: %s\n", curLocation)
      curLocation += strlen(curLocation) + 1;
    }
    else {
      break;
    } /* End of if */
  } /* End of while */

  fclose(fp_write);
}


int _read_all(char *filePath, char *outBuffer) {
  FILE *fp_read = fopen(filePath, "r+");
  if (!fp_read) ERRORF("Error opening pid file: Can't open or create %s.\n", filePath)

  // get file size
  fseek(fp_read, 0, SEEK_END);
  int fsize = (int)ftell(fp_read);
  if (fsize == 0) ERROR("No pid file.\n") /* There were no pid file. */
  fseek(fp_read, 0, SEEK_SET);

  char *fbuf = outBuffer;

  // read file and save it to buffer
  if (fread(fbuf, fsize, 1, fp_read) < 1) ERROR("Config: Failed reading pid file.\n")
  fclose(fp_read);

  return fsize;
}
