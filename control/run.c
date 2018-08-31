#include "run.h"

void continue_when_possible() {
  LOG("Flag 2\n")

  int pid = read_pid(PIDFILE_PATH);
  LOGF("Flag 3: pid read: %d\n", pid)


  for(;;) {
    usleep(1000 * 1000 * 1);
    LOG("Flag 4: in loop\n")

    if (pid == 0) {
      // No pid file. I am the only one. So, create one and continue.
      write_pid(PIDFILE_PATH);
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

void all_done() {
  dequeue_pid(getpid());
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

int write_pid (char *pidfile) {
  FILE *f;
  int fd;
  int pid;

  if ( ((fd = open(pidfile, O_RDWR|O_CREAT, 0644)) == -1)
  || ((f = fdopen(fd, "r+")) == NULL) ) {
    fprintf(stderr, "Can't open or create %s.\n", pidfile);
    return 0;
  }

  pid = getpid();
  if (!fprintf(f,"%d\n", pid)) {
    close(fd);
    return 0;
  }

  fflush(f);
  close(fd);

  return pid;
}

void dequeue_pid(int pid) {
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
