#include "serial.h"
#include "setting.h"

#define SECTION_KEY "device"
#define PORT_KEY "port"
#define BDRT_KEY "baudrate"

#define MAX_RETRY 100

bool send_command(const char *command) {
  // getting port from setting
  char port[SETTING_LENG_MAX];
  memset(port, 0, sizeof(port));
  get_setting(SECTION_KEY, PORT_KEY, port);

  // getting baudrate from setting
  char baudrateStr[SETTING_LENG_MAX] = {0,};
  memset(baudrateStr, 0, sizeof(baudrateStr));
  get_setting(SECTION_KEY, BDRT_KEY, baudrateStr);
  int baudrate = atoi(baudrateStr);
  if (baudrate == 0) ERROR("Error from baudrate. Not a number.")

  // opening port
  int fd = 0;
  int retryCnt = 0;
  while ((fd = open(port, O_RDWR | O_NOCTTY | O_SYNC)) < 0) {
    if (retryCnt ++ > MAX_RETRY) ERROR("Error opening port. Max retry exceeded.")
  }

  set_interface_attribs (fd, baudrate, 0); /* set speed to 9,600 bps, 8n1 (no parity) */
  set_blocking (fd, 0);

  LOGF("Write [%s]\n", command)

  // writing
  int wlen = write(fd, command, strlen(command));
  if (wlen != strlen(command)) ERROR("Error from writing.\n")
  tcdrain(fd);

  // ready for read
  char buf[CMDBUFF_MAX];
  memset(buf, 0, sizeof(buf));
  char *bufptr = buf; /* for count */
  int nbytes = 0;

  // read until '\n' or '\r'
  while ((nbytes = read(fd, bufptr, sizeof(buf) - (bufptr - buf) - 1)) > 0)
  {
    bufptr += nbytes;
    if (bufptr[-1] == '\n' || bufptr[-1] == '\r') break;
  } *bufptr = '\0';

  close(fd);

  LOGF("Read [%s]\n", buf)

  fprintf(stdout, "%s", buf); /* print only first line to console */

  return TRUE;
}


void set_interface_attribs (int fd, int speed, int parity)
{
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) ERROR("Error from tcgetattr.\n")

  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break
  // as \000 chars
  tty.c_iflag &= ~IGNBRK;         // disable break processing
  tty.c_lflag = 0;                // no signaling chars, no echo,
  // no canonical processing
  tty.c_oflag = 0;                // no remapping, no delays
  tty.c_cc[VMIN]  = 0;            // read doesn't block
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr (fd, TCSANOW, &tty) != 0) ERROR("Error from tcsetattr.\n")
}

void set_blocking (int fd, int should_block) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) ERROR("Error from tggetattr.\n")

  tty.c_cc[VMIN]  = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  if (tcsetattr (fd, TCSANOW, &tty) != 0) ERROR("Error setting term attributes.\n")
}
