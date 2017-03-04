#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elclient.h"

#include "GUI.h"
#include <stdio.h>
#include <time.h>
#define ELCLIENT_LOG(...) fprintf(stderr, __VA_ARGS__)

elclient_cb_t elclient_cb_table[ELCLIENT_CB_ID_NUMBER];

int32_t        rest_remote_inst = -1;
int32_t 	myData = -1;

void
elclient_register_cb(elclient_cb_id_t cb_id, elclient_cb_t p_cb)
{
    if (cb_id < ELCLIENT_CB_ID_NUMBER)
    {
        elclient_cb_table[cb_id] = p_cb;
    }
}


elproto_pkt_t *
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
 //           ELCLIENT_LOG("%s RESP V args_size=%d\n", __FUNCTION__, args_size);
 //           myData = p_pkt->value;
            break;

        case CMD_RESP_CB:
 //           ELCLIENT_LOG("%s RESP CB\n", __FUNCTION__);
            if ((p_pkt->value < ELCLIENT_CB_ID_NUMBER) &&
                (elclient_cb_table[p_pkt->value] != NULL))
            {
                elclient_cb_table[p_pkt->value](p_pkt, args_size);

                printf("cmd: %d\tvalue: %d\n",(int) p_pkt->cmd, (int)p_pkt->value);
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
//    ELCLIENT_LOG("%s\n",__FUNCTION__);

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
	 elproto_pkt_t * p_pkt;

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

int elclient_mqtt_setup(const char * hostname, uint16_t port, uint8_t security)
{

    ELCLIENT_LOG("%s hostname=%s port=%d security=%d\n",__FUNCTION__, hostname, port, security);

    elproto_new_req(CMD_MQTT_SETUP, ELCLIENT_CB_ID_MQTT, 2);
    elproto_push_arg(hostname,  strlen(hostname));
    elproto_push_arg(&port,     sizeof(port));
 //   elproto_push_arg(&security, sizeof(security));
    elproto_req_finish();


    return 0;
}


int elclient_mqtt_publish(char * path, char * data)
{
    uint8_t argc =  2;

//    ELCLIENT_LOG("%s method=%s path=%s data=%s\n",__FUNCTION__, method, path, data);

    elproto_new_req(CMD_MQTT_PUBLISH, rest_remote_inst, argc);
    elproto_push_arg(path, strlen(path));
    elproto_push_arg(data, strlen(data));

//    if (data != NULL)
//    {
//        elproto_push_arg(data, strlen(data));
//    }
//
//    elproto_push_arg(path, strlen(path));
    elproto_req_finish();


    return 0;
}

int elclient_mqtt_subscribe(const char * path, uint8_t qos)
{
	  uint8_t argc =  2;

//    ELCLIENT_LOG("%s method=%s path=%s data=%s\n",__FUNCTION__, method, path, data);

    elproto_new_req(CMD_MQTT_SUBSCRIBE, rest_remote_inst, argc);
    elproto_push_arg(path, strlen(path));
   // elproto_push_arg(&qos,     sizeof(qos));

//    if (data != NULL)
//    {
//        elproto_push_arg(data, strlen(data));
//    }
//
//    elproto_push_arg(path, strlen(path));
    elproto_req_finish();


    return 0;
}


int elclient_getTime(char *buf, int bufSize)
{

    uint8_t argc =  0;
    elproto_new_req(CMD_GET_TIME, rest_remote_inst, argc);
    elproto_req_finish();

    elproto_pkt_t *netTime = my_receive_response();


    //http://www.epochconverter.com/programming/c
	time_t     now;
	struct tm  ts;

	// Get current time
	now = netTime->value; //subtract 28800 =8 * 60 *60 convert fromGMT to -8h PST - do it in webpage

	// Format time, "ddd mm dd, yyyy\n hh:mm:ss"
	//http://man7.org/linux/man-pages/man3/strftime.3.html

	ts = *localtime(&now);

	strftime(buf, bufSize, "%a %b %d, %Y\n %I:%M:%S %P", &ts);

	return 0;
}




int elclient_ntpTime(RTC_TimeTypeDef *ntpTime, RTC_DateTypeDef *ntpDate)
{
    uint8_t argc =  0;
    elproto_new_req(CMD_GET_TIME, rest_remote_inst, argc);
    elproto_req_finish();

    elproto_pkt_t *netTime = my_receive_response();


    //http://www.epochconverter.com/programming/c
	time_t     now;
	struct tm  ts;

	// Get current time
	now = netTime->value; //subtract 28800 =8 * 60 *60 convert fromGMT to -8h PST - do it in webpage

	ts = *localtime(&now);

	//http://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html
/*
		tm_year = years since 1900
	    tm_mon = 7;           // Month, 0 - jan
	    tm_mday = 8;          // Day of the month
	    tm_hour = 16;
	    tm_min = 11;
	    tm_sec = 42;
	    tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
	    t_of_day = mktime(&t);
*/
	ntpDate->Year = ts.tm_year-100;
	ntpDate->Month = ts.tm_mon;
	ntpDate->Date = ts.tm_mday;
	ntpDate->WeekDay = ts.tm_wday;

	if(ts.tm_hour>=12)
	{
		ntpTime->Hours = ts.tm_hour-12;
		ntpTime->TimeFormat = RTC_HOURFORMAT12_PM;
	}
	else
	{
		ntpTime->Hours = ts.tm_hour;
		ntpTime->TimeFormat = RTC_HOURFORMAT12_AM;
	}
	ntpTime->Minutes = ts.tm_min;
	ntpTime->Seconds = ts.tm_sec;



	GUI_SetFont(&GUI_Font8x16);
	GUI_DispStringAt("NTP: ",30,210);
	GUI_DispDec(now,10);

	if(now<1000000000Ul)
	{
		return -1;		//invalid data ntp not connected?
	}

	return 0; //valid data
}

int elclient_getWifiStatus()
{

    uint8_t argc =  0;
    elproto_new_req(CMD_WIFI_STATUS, rest_remote_inst, argc);
    elproto_req_finish();

    elproto_pkt_t *wifiStatus = my_receive_response();

    int status=-1;
	// Get current wifi status
	status = wifiStatus->value; //subtract 28800 =8 * 60 *60 convert fromGMT to -8h PST - do it in webpage


	return 0;
}

int elclient_web_setup(const char * method, const char * hostname, uint16_t port)
{

    ELCLIENT_LOG("%s hostname=%s port=%d\n",__FUNCTION__, hostname, port);

    elproto_new_req(CMD_WEB_SETUP, ELCLIENT_CB_ID_WEB, 2);
    elproto_push_arg(method, strlen(method));
    elproto_push_arg(hostname,  strlen(hostname));
    elproto_push_arg(&port,     sizeof(port));
 //   elproto_push_arg(&security, sizeof(security));
    elproto_req_finish();


    return 0;
}

int elclient_web_data(uint16_t reason, const char * hostname, uint16_t port )
{
  //  ELCLIENT_LOG("%s hostname=%s port=%d security=%d\n",__FUNCTION__, hostname, port);

    elproto_new_req(CMD_WEB_DATA, ELCLIENT_CB_ID_WEB, 3);
    elproto_push_arg(&reason,     sizeof(reason));
    elproto_push_arg(hostname,  strlen(hostname));
    elproto_push_arg(&port,     sizeof(port));
 //   elproto_push_arg(&security, sizeof(security));
    elproto_req_finish();


    return 0;
}
