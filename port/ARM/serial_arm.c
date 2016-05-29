/*
 * serial.c
 *
 *  Created on: Oct 27, 2014
 *      Author: Marco Vedovati
 */
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#if _DARWIN_C_SOURCE
#error bingo
#endif
#include "serial_arm.h"

//#define SERIAL_LOG(...) fprintf(stderr, __VA_ARGS__)
#define SERIAL_LOG(...)

#define TIMEOUT_SEC 6

serial_if_t arm_serial_if =
{
    .p_cfg       = &arm_serial_if_cfg,
    .init        = serial_arm_init,
    .putb        = serial_arm_putb,
    .getb        = serial_arm_getb,
    .bytes_available = serial_arm_bytes_available,
};

serial_if_cfg_t arm_serial_if_cfg =
{
    .chardev  = "",
    .baudrate = 115200,
    .flowctrl = 0,
    .parity   = 0,
    .stopbits = 1,
    .bytesize = 1
};


int
serial_arm_init(serial_if_cfg_t * p_cfg)
{
    (void)p_cfg;
    return -1;
}

uint8_t
serial_arm_getb(void)
{
    uint8_t rxbyte = 0xFF;

    return rxbyte;
}

void
serial_arm_putb(uint8_t byte)
{
    (void)byte;
}


uint32_t
serial_arm_bytes_available(void)
{
    int32_t bytes_avail = 0;


    return bytes_avail;
}
