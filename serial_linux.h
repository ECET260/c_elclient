/*
 * serial.h
 *
 *  Created on: Oct 27, 2014
 *      Author: Marco Vedovati
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "serialtype.h"

int         serial_linux_init(serial_if_cfg_t * p_cfg);

uint8_t     serial_linux_getb(void);

void        serial_linux_putb(uint8_t byte);

uint32_t    serial_linux_bytes_available(void);

#endif /* SERIAL_H_ */
