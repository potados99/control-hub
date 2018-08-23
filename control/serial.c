#include "serial.h"
#include "setting.h"

#define SECTION_KEY "device"
#define PORT_KEY "port"
#define BDRT_KEY "boadrate"

bool send_command(const char *command) {
  char port[SETTING_LENG_MAX] = {0,};
  get_setting(SECTION_KEY, PORT_KEY, port);

  int boadrate = 0;
  char boadrateStr[SETTING_LENG_MAX] = {0,};
  get_setting(SECTION_KEY, BDRT_KEY, boadrateStr);
  boadrate = atoi(boadrateStr);

  int fd = open (port, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    error_message ("error %d opening %s: %s", errno, portname, strerror (errno));
    return;
  }

  set_interface_attribs (fd, B9600, 0);  // set speed to 9,600 bps, 8n1 (no parity)
  set_blocking (fd, 0);

  write (fd, command, strlen(command) + 1);           // send 7 character greeting

  usleep ((strlen(command) + 5) * 1000);             // sleep enough to transmit the 7 plus
  // receive 25:  approx 1000 uS per char transmit
  char buf [100];
  int n = read (fd, buf, sizeof buf);
}


int set_interface_attribs (int fd, int speed, int parity)
{
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0)
  {
    error_message ("error %d from tcgetattr", errno);
    return -1;
  }

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

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
  {
    error_message ("error %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}

void set_blocking (int fd, int should_block) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0)
  {
    error_message ("error %d from tggetattr", errno);
    return;
  }

  tty.c_cc[VMIN]  = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
  error_message ("error %d setting term attributes", errno);
}
