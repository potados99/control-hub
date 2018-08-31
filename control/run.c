#include "run.h"

void continue_when_possible(char *pidfile) {
  LOG("Flag 2\n")

  int pid = read_pid(pidfile);
  LOGF("Flag 3: pid read: %d\n", pid)


  for(;;) {
    usleep(1000 * 1000 * 1);
    LOG("Flag 4: in loop\n")

    if (pid == 0) {
      // No pid file. I am the only one. So, create one and continue.
      add_pid(pidfile);
      LOG("Flag suc: write_pid\n")

      printf("pass!\n");
      return;
    }
    else {
      // pid file exists.
      if (pid == getpid()) {
        // Ooh ya let's go.
        return;
      }
      else {
        // Wait.
        LOG("Flag Wait!\n")

      }
    } /* if pid end */
  } /* for end */
}

void all_done(char *pidfile) {
  remove_pid(pidfile, getpid());
}

int read_pid (char *pidfile)
{
  FILE *f;
  int pid;

  if (!(f=fopen(pidfile,"r")))
  return 0;
  fscanf(f,"%d", &pid);
  fclose(f);
  return pid;
}

int add_pid (char *pidfile) {
  FILE *fp = fdopen(fd, "a"); // append at the end
  if (fp == NULL) ERRORF("Can't open or create %s.\n", pidfile);

  int pid = getpid();
  if (!fprintf(f,"%d\n", pid)) ERROR("Failed writing pid to file.\n")

  close(fd);

  return pid;
}

void remove_pid(char *pidfile, int pid) {
  FILE *f;
  int fileSize;
  char fbuff[PIDFILE_BUF_MAX];
  char *pfbuff;

  if ((f = fopen(PIDFILE_PATH, "r+")) == NULL) {
    fprintf(stderr, "Can't open or create %s.\n", PIDFILE_PATH);
    return;
  }

  // get file size
  fseek(f, 0, SEEK_END);
  fileSize = (int)ftell(f);
  fseek(f, 0, SEEK_SET);

  if (fileSize == 0) return; /* There were no pid file. */

  // read file and save it to buffer
  if (fread(fbuff, fileSize, 1, f) < 1) {
    fclose(f);
    ERROR("Config: Failed reading file\n")
  }

  pfbuff = fbuff;

  for (int i = 0; i < fileSize; ++ i) {
    if (fbuff[i] == '\n') fbuff[i] = 0x00;
  }

  while (pfbuff - fbuff < fileSize) {
    if (fprintf(f, "%s\n", pfbuff)) {
      pfbuff += strlen(pfbuff);
    }
    else {
      printf("Can't write pid , %s.\n", "d");
      fclose(f);
      return;
    }
  }


  fflush(f);
  fclose(f);


}
