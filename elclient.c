#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elclient.h"

#define ELCLIENT_LOG(...) fprintf(stderr, __VA_ARGS__)

elclient_cb_t elclient_cb_table[ELCLIENT_CB_ID_NUMBER];

int32_t        rest_remote_inst = -1;

void
elclient_register_cb(elclient_cb_id_t cb_id, elclient_cb_t p_cb)
{
    if (cb_id < ELCLIENT_CB_ID_NUMBER)
    {
        elclient_cb_table[cb_id] = p_cb;
    }
}


static elproto_pkt_t * 
my_receive_response(void)
{
    elproto_pkt_t * p_pkt;
    uint16_t args_size;

    if ((p_pkt = elproto_get_resp(&args_size)) == NULL)
    {
        ELCLIENT_LOG("%s: error detected in response\n", __FUNCTION__);
        return NULL;
    }
    
    if ((p_pkt->argc * sizeof(uint32_t)) != args_size)
    {
        fprintf(stderr, "%s invalid arg size=%d detected\n", __FUNCTION__, args_size);
//        return NULL;
    }

    switch (p_pkt->cmd)
    {
        case CMD_RESP_V:
            ELCLIENT_LOG("%s RESP V args_size=%d\n", __FUNCTION__, args_size);
            break;

        case CMD_RESP_CB:
            ELCLIENT_LOG("%s RESP CB\n", __FUNCTION__);
            if ((p_pkt->value < ELCLIENT_CB_ID_NUMBER) && 
                (elclient_cb_table[p_pkt->value] != NULL))
            {
                elclient_cb_table[p_pkt->value](p_pkt, args_size);
            }
            break;

        default:
            return NULL;
            break;
    }

    return p_pkt;
}

int elclient_sync(void)
{
    ELCLIENT_LOG("%s\n",__FUNCTION__);

    elproto_new_req(CMD_SYNC, ELCLIENT_CB_ID_COMMON, 0);
    elproto_req_finish();

    // Get response...
    my_receive_response();
    
    //get callback
    my_receive_response();

    return 0;
}

int elclient_rest_setup(const char * hostname, uint16_t port, uint8_t security)
{
    elproto_pkt_t * p_pkt;
    
    ELCLIENT_LOG("%s hostname=%s port=%d security=%d\n",__FUNCTION__, hostname, port, security);

    elproto_new_req(CMD_REST_SETUP, ELCLIENT_CB_ID_REST, 3);
    elproto_push_arg(hostname,  strlen(hostname));
    elproto_push_arg(&port,     sizeof(port));
    elproto_push_arg(&security, sizeof(security));
    elproto_req_finish();

    // Get response...
    p_pkt = my_receive_response();

    if (p_pkt != NULL)
    {
        rest_remote_inst = p_pkt->value;
    }

    return 0;
}

int elclient_rest_request(const char * method, char * path, char * data)
{
    uint8_t argc = (data != NULL) ? 3 : 2;

    ELCLIENT_LOG("%s method=%s path=%s data=%s\n",__FUNCTION__, method, path, data);

    elproto_new_req(CMD_REST_REQUEST, rest_remote_inst, argc);
    elproto_push_arg(method, strlen(method));
    elproto_push_arg(path, strlen(path));

    if (data != NULL)
    {
        elproto_push_arg(data, strlen(data));
    }

    elproto_push_arg(path, strlen(path));
    elproto_req_finish();
    
    my_receive_response();

    return 0;
}

