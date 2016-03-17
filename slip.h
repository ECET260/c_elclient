#ifndef SLIP_H__
#define SLIP_H__

#include "serialtype.h"

int      slip_init(serial_if_t * p_serial);

uint32_t slip_tx_frame(uint8_t * p_data, uint32_t len);

void     slip_tx_end(void);

uint32_t slip_rx_frame(uint8_t * p_data);

#endif /* #ifndef SLIP_H__ */

