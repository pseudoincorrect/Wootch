#ifndef __wifi_connect_h__
#define __wifi_connect_h__

#include"esp_event.h"

esp_err_t event_handler(void *ctx, system_event_t *event);
void initialise_wifi(void);
void connect_wifi(char* ssid, char* password);
void disconnect_wifi(void);

#endif