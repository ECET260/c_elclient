#ifndef ELCLIENT_H__
#define ELCLIENT_H__

#include <stdint.h>
#include "elproto.h"

typedef void (*elclient_cb_t)(void * p_param, uint16_t args_size);

typedef enum
{
    ELCLIENT_CB_ID_NONE  = 0,
    ELCLIENT_CB_ID_COMMON,
    ELCLIENT_CB_ID_REST,
    // Add here // 
    
    ELCLIENT_CB_ID_NUMBER,
}elclient_cb_id_t;

void    elclient_register_cb(elclient_cb_id_t cb_id, elclient_cb_t p_cb);

int elclient_sync();

int elclient_rest_setup(const char * hostname, uint16_t port, uint8_t security);

int elclient_rest_request(const char * method, char * path, char * data);

#endif /* #ifndef ELCLIENT_H__ */
