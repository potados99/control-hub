#include "run.h"

void continue_when_possible() {
  int pid = read_pid(PIDFILE_PATH);

  for(;;) {
    if (pid == 0) {
      // No pid file. I am the only one. So, create one and continue.
      write_pid(PIDFILE_PATH);
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

 /* It seems to be acceptable that we do not lock the pid file
  * if we run under Solaris. In any case, it is highly unlikely
  * that two instances try to access this file. And flock is really
  * causing me grief on my initial steps on Solaris. Some time later,
  * we might re-enable it (or use some alternate method).
  * 2006-02-16 rgerhards
  */

#if HAVE_FLOCK
  if (flock(fd, LOCK_EX|LOCK_NB) == -1) {
      fscanf(f, "%d", &pid);
      fclose(f);
      printf("Can't lock, lock is held by pid %d.\n", pid);
      return 0;
  }
#endif

  pid = getpid();
  if (!fprintf(f,"%d\n", pid)) {
      char errStr[1024];

      close(fd);
      return 0;
  }
  fflush(f);

#if HAVE_FLOCK
  if (flock(fd, LOCK_UN) == -1) {
      char errStr[1024];

      close(fd);
      return 0;
  }
#endif
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
    return 0;
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
