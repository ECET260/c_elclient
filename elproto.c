#include "elproto.h"
#include "slip.h"
#include <stdlib.h>
#include <stdio.h>


#if defined(BUILD_FOR_UNIX)
//#define ELPROTO_LOG(...) fprintf(stderr, __VA_ARGS__)
#define ELPROTO_LOG(...)
#else
#define ELPROTO_LOG(...)
#endif

static uint16_t elproto_crc;
// TODO: set adequate size...
static uint8_t elproto_resp_buffer[256];

static uint16_t my_crc16_add(uint8_t b, uint16_t acc)
{
  acc ^= b;
  acc = (acc >> 8) | (acc << 8);
  acc ^= (acc & 0xff00) << 4;
  acc ^= (acc >> 8) >> 4;
  acc ^= (acc & 0xff00) >> 5;
  return acc;
}

static void my_crc16_update(const uint8_t * data, uint16_t len)
{
    for (uint16_t i=0; i<len; i++)
    {
        elproto_crc = my_crc16_add(*data++, elproto_crc);
    }
}

static void my_crc16_reset(void)
{
    elproto_crc = 0;
}


void
elproto_new_req(uint16_t cmd, uint32_t value, uint16_t argc)
{
    ELPROTO_LOG("%s cmd=0x%02X\n", __FUNCTION__, cmd);

    slip_tx_end();

    my_crc16_reset();

    // TODO: big/little endian??
    slip_tx_frame((uint8_t *)&cmd, 2);
    my_crc16_update((uint8_t *)&cmd, 2);

    slip_tx_frame((uint8_t *)&argc, 2);
    my_crc16_update((uint8_t *)&argc, 2);

    slip_tx_frame((uint8_t *)&value, 4);
    my_crc16_update((uint8_t *)&value, 4);
}


void
elproto_push_arg(const void * p_data, uint16_t len)
{
    uint16_t pad;
    uint8_t  pad_data = 0;

    ELPROTO_LOG("%s\n", __FUNCTION__);

    // tx len
    slip_tx_frame((uint8_t *)&len, 2);
    my_crc16_update((uint8_t *)&len, 2);

    // tx data
    slip_tx_frame((uint8_t *)p_data,   len);
    my_crc16_update((uint8_t *)p_data, len);

    // tx pad
    pad = (4 - (len & 3)) & 3;

    while(pad--)
    {
        slip_tx_frame(&pad_data, 1);
        my_crc16_update(&pad_data, 1);
    }
}

void
elproto_req_finish(void)
{
    ELPROTO_LOG("%s\n", __FUNCTION__);

    slip_tx_frame((uint8_t *)&elproto_crc, 2);
    slip_tx_end();
}

elproto_pkt_t *
elproto_get_resp(uint16_t * p_args_size)
{
    elproto_pkt_t * p_pkt;
    uint32_t rxlen;
    uint16_t rcvd_crc;

    *p_args_size = 0;

    ELPROTO_LOG("%s\n", __FUNCTION__);

    // TODO: timeout / retry?
    rxlen = slip_rx_frame(elproto_resp_buffer);

    if (rxlen < 8)
    {
        ELPROTO_LOG("%s not enough num of bytes=%d rcvd\n", __FUNCTION__, rxlen);
        return NULL;
    }

    p_pkt = (elproto_pkt_t *)elproto_resp_buffer;

    ELPROTO_LOG("%s cmd=0x%02X argc=%d value=0x%08X len=%d\n",
           __FUNCTION__, p_pkt->cmd, p_pkt->argc, p_pkt->value, rxlen);

    //verify crc
    rcvd_crc  = *(uint16_t *)(&elproto_resp_buffer[rxlen - 2]);
    my_crc16_reset();
    my_crc16_update(elproto_resp_buffer, rxlen -2);

    if (rcvd_crc != elproto_crc)
    {
        ELPROTO_LOG("CRC check fail!\n");
        return NULL;
    }

    *p_args_size = rxlen - 2 - (uint16_t)((void *)p_pkt->args - (void *)p_pkt);
    ELPROTO_LOG("Decoded CMD: ");

    if ((p_pkt->cmd != CMD_RESP_V) && (p_pkt->cmd != CMD_RESP_CB))
    {
        ELPROTO_LOG("Unknown cmd=0x%02X\n", p_pkt->cmd);
        return NULL;
    }

    return p_pkt;
}

