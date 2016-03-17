/*
 * serial.c
 *
 *  Created on: Oct 27, 2014
 *      Author: Marco Vedovati
 */
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "serial_linux.h"

//#define SERIAL_LOG(...) fprintf(stderr, __VA_ARGS__)
#define SERIAL_LOG(...)

#define TIMEOUT_SEC 6

static int serial_fd = -1;

static int32_t my_get_uart_baudrate(int32_t s)
{
  switch (s)
  {
    case 9600:
      return B9600;
    case 19200:
      return B19200;
    case 38400:
      return B38400;
    case 57600:
      return B57600;
    case 115200:
      return B115200;
    case 230400:
      return B230400;
    case 460800:
      return B460800;
    case 500000:
      return B500000;
    case 576000:
      return B576000;
    case 921600:
      return B921600;
    case 1000000:
      return B1000000;
    case 1152000:
      return B1152000;
    case 1500000:
      return B1500000;
    case 3000000:
      return B3000000;
    default:
      return B0;
   }
}

int32_t serial_linux_set_speed(int32_t fd, struct termios *ti, int32_t speed)
{
  cfsetospeed(ti, my_get_uart_baudrate(speed));
  return tcsetattr(fd, TCSANOW, ti);
}

int
serial_linux_init(serial_if_cfg_t * p_cfg)
{
  struct termios ti;
  int fd = open(p_cfg->chardev, O_RDWR | O_NOCTTY);

  SERIAL_LOG("%s opening: %s\n", __FUNCTION__, p_cfg->chardev);

  if (fd < 0)
  {
    perror("Can't open serial port");
    return -1;
  }
  
  serial_fd = fd;

  tcflush(fd, TCIOFLUSH);

  if (tcgetattr(fd, &ti) < 0)
  {
    perror("Can't get port settings");
    return -1;
  }

  cfmakeraw(&ti);
  ti.c_cflag |= CLOCAL;

  // Set 1 stop bit & no parity (8-bit data already handled by cfmakeraw)
  ti.c_cflag &= ~(CSTOPB | PARENB);

  if (p_cfg->flowctrl)
  {
    ti.c_cflag |= CRTSCTS;
  }
  else
  {
    ti.c_cflag &= ~CRTSCTS;
  }

  //FOR READS:  set timeout time w/ no minimum characters needed (since we read only 1 at at time...)
  ti.c_cc[VMIN] = 0;
  ti.c_cc[VTIME] = TIMEOUT_SEC * 10;

  if (tcsetattr(fd, TCSANOW, &ti) < 0)
  {
    perror("Can't set port settings");
    return -1;
  }
  tcflush(fd, TCIOFLUSH);

  /* Set actual baudrate */
  if (serial_linux_set_speed(fd, &ti, p_cfg->baudrate) < 0)
  {
    perror("Can't set baud rate");
    return -1;
  }

  return fd;
}

uint8_t
serial_linux_getb(void)
{
    uint8_t rxbyte = 0xFF;
    int     rxlen;

    if ((rxlen = read(serial_fd, &rxbyte, 1))  < 0)
    {
        printf("%s failed!\n", __FUNCTION__);
        return 0;
    }

    SERIAL_LOG("%s RX: %02X\n", __FUNCTION__, rxbyte);

    return rxbyte;
}

void
serial_linux_putb(uint8_t byte)
{

    if (write(serial_fd, &byte, 1) <= 0)
    {
        printf("%s failed!\n", __FUNCTION__);
    }
    
    SERIAL_LOG("%s TX: %02X\n", __FUNCTION__, byte);
}


uint32_t
serial_linux_bytes_available(void)
{
    int32_t bytes_avail;
   
    ioctl(serial_fd, FIONREAD, &bytes_avail);
    
    if (bytes_avail < 0)
    {
        bytes_avail = 0;
    }

    return bytes_avail;
}
