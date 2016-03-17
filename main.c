#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "slip.h"
#include "serialtype.h"
#include "serial_unix.h"
#include "elclient.h"
#include "elproto.h"



const char  *  rest_hostname   = "api.thingspeak.com";
const char  *  rest_method_get = "GET";
const char  *  rest_base_path  = "/update?api_key=A1KB9EIGB127VKX3&field1=";
const uint16_t rest_port       = 80;
const uint8_t  rest_sec        = 0;


typedef struct __attribute__((packed))
{
    uint16_t  len;
    uint8_t   data[0];
} elproto_arg_t;


serial_if_cfg_t unix_serial_if_cfg = 
{
    .chardev  = "/dev/ttyUSB0",
    .baudrate = 115200,
    .flowctrl = 0,
    .parity   = 0,
    .stopbits = 1,
    .bytesize = 1
};

serial_if_t unix_serial_if =
{
    .p_cfg       = &unix_serial_if_cfg,
    .init        = serial_unix_init,
    .putb        = serial_unix_putb,
    .getb        = serial_unix_getb,
    .bytes_available = serial_unix_bytes_available,
};

void my_elclient_common_cb(void * p_param, uint16_t args_size)
{
    elproto_pkt_t * p_pkt   = (elproto_pkt_t *)p_param;
    elproto_arg_t * p_arg_x = (elproto_arg_t *) &(p_pkt->args[0]);

    uint16_t        i;

    fprintf(stderr, "%s argc=%d args size=%d ", __FUNCTION__, p_pkt->argc, args_size);

#if 0
    for (i = 0; i < args_size; i++)
    {
        fprintf(stderr, "%02X ", p_pkt->args[i]);
    }
#endif

    for (i = 0; i < p_pkt->argc; i++)
    {
        uint16_t j;
        fprintf(stderr, " | arg[%d] len=%d dump=", i, p_arg_x->len);
    
        for (j = 0; j < p_arg_x->len; j++)
        {
            fprintf(stderr, "%02X ", p_arg_x->data[j]);
        }


        // skip the pad
        while (((j + sizeof(p_arg_x->len)) % 4) != 0)
        {
            j++;
        }

//        fprintf(stderr, "before: %p\n", p_arg_x);
        p_arg_x = (elproto_arg_t *)&(p_arg_x->data[j]);
//        fprintf(stderr, "after: %p\n", p_arg_x);
    }

    fprintf(stderr, "\n");
}


int main(void)
{
    char     full_path[sizeof(rest_base_path) + 32];

    elclient_register_cb(ELCLIENT_CB_ID_COMMON, my_elclient_common_cb);
    elclient_register_cb(ELCLIENT_CB_ID_REST,   my_elclient_common_cb);

    if (slip_init(&unix_serial_if) < 0)
    {
        fprintf(stderr, "slip init failed\n");
        return -1;
    }
    
    if (elclient_sync())
    {
        fprintf(stderr, "elclient sync failed\n");
        return -1;
    }

    if (elclient_rest_setup(rest_hostname, 80, 0))
    {
        fprintf(stderr, "elclient rest setup failed\n");
        return -1;
    }
   
    srand(time(NULL)); 
    
    for (;;)
    {
        int rand_variance = (rand() & 0x0F) - 8;
        int16_t sensor_data;

        sensor_data = 20 + rand_variance;

        memset(full_path, 0, sizeof(full_path));
        sprintf(full_path,"%s%d", rest_base_path, sensor_data);

        if (elclient_rest_request("GET", full_path, NULL))
        {
            fprintf(stderr, "elclient rest request failed \n");
            return -1;
        }

        // Update interval is 15 seconds for thingspeak.com
        sleep(20);
    }



    return 0;
}
