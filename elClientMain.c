//Forked code from https://github.com/marcov/c_elclient
//a C implemenation of the Arduino elclient libraries
//for use with Jeelabs esp-link at https://github.com/jeelabs/esp-link
//
//Modified to use with STM32F4Discovery boards and the STM32CubeMX software
//
//Mel Dundas
//Feb 21, 2017


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "slip.h"


#include "elclient.h"
#include "elproto.h"
#include "stm32f4xx_hal.h"



//const char * elclient_config_param_hostname  = "api.thingspeak.com";
//const char * elclient_config_param_base_path = "/update?api_key=A1KB9EIGB127VKX3&field1=";
//const char * elclient_config_param_port      = "80";
//const char * elclient_config_param_security  = "0";
//const char * elclient_config_param_up_rate_s = "20";

struct
{
    char      * hostname;
    char      * base_path;
    uint16_t    port;
    uint8_t     security;
    uint16_t    update_rate_s;
} elclient_config;

typedef struct __attribute__((packed))
{
    uint16_t  len;
    uint8_t   data[0];
} elproto_arg_t;




void my_elclient_common_cb(void * p_param, uint16_t args_size)
{
    elproto_pkt_t * p_pkt   = (elproto_pkt_t *)p_param;
    elproto_arg_t * p_arg_x = (elproto_arg_t *) &(p_pkt->args[0]);

    uint16_t        i;

    fprintf(stderr, "%s argc=%d args size=%d ", __FUNCTION__, p_pkt->argc, args_size);


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


void my_elclient_rest_cb(void * p_param, uint16_t args_size)
{
    elproto_pkt_t * p_pkt   = (elproto_pkt_t *)p_param;
    elproto_arg_t * p_arg_x = (elproto_arg_t *) &(p_pkt->args[0]);

    uint16_t        i;

    fprintf(stderr, "%s argc=%d args size=%d ", __FUNCTION__, p_pkt->argc, args_size);


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



int elClientMain(void)
{
//    char           full_path[sizeof(elclient_config.base_path) + 32];
    elclient_register_cb(ELCLIENT_CB_ID_COMMON, my_elclient_common_cb);
    elclient_register_cb(ELCLIENT_CB_ID_REST,   my_elclient_common_cb);
    elclient_register_cb(ELCLIENT_CB_ID_MQTT,   my_elclient_common_cb);
    elclient_register_cb(ELCLIENT_CB_ID_WEB,   my_elclient_common_cb);


/*
    p_serial_if     = &arm_serial_if;
    p_serial_if_cfg = &arm_serial_if_cfg;


    fprintf(stderr, "elclient config:\n"
                    "  host    = '%s'\n"
                    "  path    = '%s'\n"
                    "  port    = '%d'\n"
                    "  sec     = '%d'\n"
                    "  updrate = '%d'\n",

            elclient_config.hostname,
            elclient_config.base_path,
            elclient_config.port,
            elclient_config.security,
            elclient_config.update_rate_s);


    if (slip_init(p_serial_if) < 0)
    {
        fprintf(stderr, "slip init failed\n");
        return -1;
    }


    if (elclient_sync())
    {
        fprintf(stderr, "elclient sync failed\n");
        return -1;
    }

    if (elclient_rest_setup(elclient_config.hostname, 80, 0))
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

//        memset(full_path, 0, sizeof(full_path));
        sprintf(full_path,"%s%d", elclient_config.base_path, sensor_data);

        if (elclient_rest_request("GET", full_path, NULL))
        {
            fprintf(stderr, "elclient rest request failed \n");
            return -1;
        }

        // Min update interval is 15 seconds for thingspeak.com
        HAL_Delay(elclient_config.update_rate_s*1000);

    }
*/
    return 0;
}
