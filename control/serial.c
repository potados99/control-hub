#include "serial.h"
#include "setting.h"

#define SECTION_KEY "device"
#define PORT_KEY "port"
#define BDRT_KEY "baudrate"

#define SLEEP_PER_CAHR 2000

bool send_command(const char *command) {
  char port[SETTING_LENG_MAX] = {0,};
  get_setting(SECTION_KEY, PORT_KEY, port);

  int baudrate = 0;
  char baudrateStr[SETTING_LENG_MAX] = {0,};
  get_setting(SECTION_KEY, BDRT_KEY, baudrateStr);
  baudrate = atoi(baudrateStr);

  int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) ERROR("Error opening port.\n")

  set_interface_attribs (fd, baudrate, 0);  /* set speed to 9,600 bps, 8n1 (no parity) */
  set_blocking (fd, 0);

  LOGF("Write [%s]\n", command)

  /* simple output */
  int wlen = write(fd, command, strlen(command));
  if (wlen != strlen(command)) ERROR("Error from writing.\n")
  tcdrain(fd);

  char buf[CMDBUFF_MAX];
  memset(buf, 0, CMDBUFF_MAX);

  char *bufptr = buf; /* for count */

  /* read characters into our string buffer until we get a CR or NL */
  while ((nbytes = read(fd, bufptr, sizeof(buf) + (buffer - bufptr) - 1)) > 0)
  {
    bufptr += nbytes;
    if (bufptr[-1] == '\n' || bufptr[-1] == '\r') {
      break;
    }
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
