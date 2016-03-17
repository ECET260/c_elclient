#ifndef ELPROTO_H__
#define ELPROTO_H__

#include <stdint.h>

// Enumeration of commands supported by esp-link, this needs to match the definition in
// esp-link!
typedef enum {
  CMD_NULL = 0,     // null, mainly to prevent 0 from doing something bad
  CMD_SYNC,         // synchronize, starts the protocol
  CMD_RESP_V,       // response with a value
  CMD_RESP_CB,      // response with a callback
  CMD_WIFI_STATUS,  // get the wifi status
  CMD_CB_ADD,       // add a custom callback
  CMD_CB_EVENTS,    // ???
  CMD_GET_TIME,     // get current time in seconds since the unix epoch
  //CMD_GET_INFO,

  CMD_MQTT_SETUP = 10,
  CMD_MQTT_PUBLISH,
  CMD_MQTT_SUBSCRIBE,
  CMD_MQTT_LWT,
  CMD_MQTT_EVENTS,

  CMD_REST_SETUP = 20,
  CMD_REST_REQUEST,
  CMD_REST_SETHEADER,
  CMD_REST_EVENTS
} elproto_cmd_t;

typedef enum
{
  STATION_IDLE = 0,
  STATION_CONNECTING,
  STATION_WRONG_PASSWORD,
  STATION_NO_AP_FOUND,
  STATION_CONNECT_FAIL,
  STATION_GOT_IP
} elproto_wifi_status_t;


typedef struct __attribute__((packed))
{
    uint16_t cmd;
    uint16_t argc;
    uint32_t value;
    uint8_t  args[0];
}elproto_pkt_t;


void elproto_new_req(uint16_t cmd, uint32_t value, uint16_t argc);

void elproto_push_arg(const void * p_data, uint16_t len);

void elproto_req_finish(void);

elproto_pkt_t * elproto_get_resp(uint16_t * p_args_size);

#endif /* #ifndef ELPROTO_H__ */
