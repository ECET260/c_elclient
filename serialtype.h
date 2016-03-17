#ifndef SERIAL_TYPE_H__
#define SERIAL_TYPE_H__

#include <stdint.h>
#include <stdbool.h>


typedef struct 
{
    char      * chardev;
    int32_t     baudrate;
    uint8_t     parity;
    uint8_t     stopbits;
    uint8_t     bytesize;
    uint8_t     flowctrl;
} serial_if_cfg_t;

typedef struct 
{
    serial_if_cfg_t * p_cfg;
    int               (*init) (serial_if_cfg_t * p_cfg);
    int               (*read) (uint8_t * p_data, uint32_t len, bool blocking);
    int               (*write)(uint8_t * p_data, uint32_t len);
    uint8_t           (*getb) (void);
    void              (*putb) (uint8_t byte);
    uint32_t          (*bytes_available) (void);
} serial_if_t;

#endif /* #ifndef SERIAL_TYPE_H__ */
