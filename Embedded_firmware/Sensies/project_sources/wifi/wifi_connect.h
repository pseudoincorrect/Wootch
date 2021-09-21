#ifndef __wifi_connect_h__
#define __wifi_connect_h__

#include"esp_event.h"

esp_err_t event_handler(void *ctx, system_event_t *event);
void wifi_init(void);
void wifi_connect(char* ssid, char* password);
void wifi_disconnect(void);

#endif