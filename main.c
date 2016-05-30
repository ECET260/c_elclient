#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "slip.h"
#include "serialtype.h"
#if defined(BUILD_FOR_ARM)
#include "serial_arm.h"
#else
#include "serial_unix.h"
#endif
#include "elclient.h"
#include "elproto.h"



const char * elclient_conf_file              = "elclient.conf";
const char * elclient_config_param_chardev   = "chardev";
const char * elclient_config_param_hostname  = "hostname";
const char * elclient_config_param_base_path = "path";
const char * elclient_config_param_port      = "port";
const char * elclient_config_param_security  = "security";
const char * elclient_config_param_up_rate_s = "update_rate_s";

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


serial_if_t     * p_serial_if;
serial_if_cfg_t * p_serial_if_cfg;


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

#if defined(BUILD_FOR_UNIX)
static int
my_load_config(void)
{
    int f = -1;
    char param[1024];
    char value[1024];

    fprintf(stderr, "Reading config file...\n");

    if ((f = open(elclient_conf_file, O_RDONLY)) <= 0)
    {
        perror("Cannot open conf file");
        return -1;
    }

    if (lseek(f, 0, SEEK_SET) < 0)
    {
        perror("seek failed");
        close(f);
        return -1;
    }

    for (;;)
    {
        uint32_t i = 0;
        int retval;

        memset(param, 0, sizeof(param));
        memset(value, 0, sizeof(value));

        do
        {
            retval = read(f, &param[i], 1);

            if (retval == 0)
            {
                fprintf(stderr, "Read completed\n");
                close(f);
                return 0;
            }
            else if (retval < 0)
            {
                perror("Read failed");
                close(f);
                return -1;
            }
        } while(param[i++] != '=');


        //Remove '='
        param[i - 1] = '\0';

        i = 0;
        do
        {
            retval = read(f, &value[i], 1);

            if (retval == 0)
            {
                fprintf(stderr, "Read completed\n");
                close(f);
                return 0;
            }
            else if (retval < 0)
            {
                perror("Read failed");
                close(f);
                return -1;
            }
        } while(value[i++] != '\n');

        //Remove '\n'
        value[i - 1] = '\0';

#if 0
        fprintf(stderr, "Read param=%s value=%s\n", param, value);
#endif

        // +1 because we also copy \0
        if (strcmp(elclient_config_param_chardev, param) == 0)
        {
            p_serial_if_cfg->chardev = (char *)malloc(strlen(value) + 1);
            memcpy(p_serial_if_cfg->chardev, value, strlen(value) + 1);
        }
        else if (strcmp(elclient_config_param_hostname, param) == 0)
        {
            elclient_config.hostname = (char *)malloc(strlen(value) + 1);
            memcpy(elclient_config.hostname, value, strlen(value) + 1);
        }
        else if (strcmp(elclient_config_param_base_path, param) == 0)
        {
            elclient_config.base_path = (char *)malloc(strlen(value) + 1);
            memcpy(elclient_config.base_path, value, strlen(value) + 1);
        }
        else if (strcmp(elclient_config_param_port, param) == 0)
        {
            elclient_config.port = atol(value);
        }
        else if (strcmp(elclient_config_param_security, param) == 0)
        {
            elclient_config.security = atol(value);
        }
        else if (strcmp(elclient_config_param_up_rate_s, param) == 0)
        {
            elclient_config.update_rate_s = atol(value);
        }
        else
        {
            fprintf(stderr, "Unknown param=%s value=%s\n", param, value);
        }
    }

    close(f);
    return -1;
}
#endif /* #if defined(BUILD_FOR_UNIX) */

int main(void)
{
    char           full_path[sizeof(elclient_config.base_path) + 32];
    elclient_register_cb(ELCLIENT_CB_ID_COMMON, my_elclient_common_cb);
    elclient_register_cb(ELCLIENT_CB_ID_REST,   my_elclient_common_cb);

#if defined(BUILD_FOR_ARM)
    p_serial_if     = &arm_serial_if;
    p_serial_if_cfg = &arm_serial_if_cfg;
#else
    p_serial_if     = &unix_serial_if;
    p_serial_if_cfg = &unix_serial_if_cfg;

    if (my_load_config() != 0)
    {
        fprintf(stderr, "Cannot load config\n");
        return -1;
    }
#endif


    fprintf(stderr, "elclient config:\n"
                    "  chardev = '%s'\n"
                    "  host    = '%s'\n"
                    "  path    = '%s'\n"
                    "  port    = '%d'\n"
                    "  sec     = '%d'\n"
                    "  updrate = '%d'\n",
            p_serial_if_cfg->chardev,
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

        memset(full_path, 0, sizeof(full_path));
        sprintf(full_path,"%s%d", elclient_config.base_path, sensor_data);

        if (elclient_rest_request("GET", full_path, NULL))
        {
            fprintf(stderr, "elclient rest request failed \n");
            return -1;
        }
#if defined(BUILD_FOR_UNIX)
        // Min update interval is 15 seconds for thingspeak.com
        sleep(elclient_config.update_rate_s);
#else
       //TODO!!!
       for (int i =0; i < 0xFFFFFFFF; i++)
        ;;
#endif
    }

    return 0;
}
