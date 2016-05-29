/*
 * serial.h
 *
 *  Created on: Oct 27, 2014
 *      Author: Marco Vedovati
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "serialtype.h"

extern serial_if_t arm_serial_if;
extern serial_if_cfg_t arm_serial_if_cfg;

int         serial_arm_init(serial_if_cfg_t * p_cfg);

uint8_t     serial_arm_getb(void);

void        serial_arm_putb(uint8_t byte);

uint32_t    serial_arm_bytes_available(void);

#endif /* SERIAL_H_ */
