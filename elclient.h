#ifndef ELCLIENT_H__
#define ELCLIENT_H__

#include <stdint.h>
#include "elproto.h"
#include "stm32f4xx_hal.h"

typedef void (*elclient_cb_t)(void * p_param, uint16_t args_size);

typedef enum
{
    ELCLIENT_CB_ID_NONE  = 0,
    ELCLIENT_CB_ID_COMMON,
    ELCLIENT_CB_ID_REST,
    // Add here // 
    ELCLIENT_CB_ID_MQTT,
	ELCLIENT_CB_ID_WEB,
    ELCLIENT_CB_ID_NUMBER,
}elclient_cb_id_t;

uint32_t myTime;


void    elclient_register_cb(elclient_cb_id_t cb_id, elclient_cb_t p_cb);

int elclient_sync();

int elclient_rest_setup(const char * hostname, uint16_t port, uint8_t security);

int elclient_rest_request(const char * method, char * path, char * data);

int elclient_mqtt_setup(const char * hostname, uint16_t port, uint8_t security);

int elclient_mqtt_publish(char * path, char * data);

int elclient_mqtt_subscribe(const char * path, uint8_t qos);

elproto_pkt_t *  my_receive_response(void);

int elclient_getTime(char *buf, int bufSize);

int elclient_ntpTime(RTC_TimeTypeDef *ntpTime, RTC_DateTypeDef *ntpDate);

int elclient_getWifiStatus();

int elclient_web_setup(const char * method, const char * hostname, uint16_t port);

int elclient_web_data(uint16_t reason, const char * hostname, uint16_t port);

#endif /* #ifndef ELCLIENT_H__ */
