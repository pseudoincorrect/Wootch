#ifndef __state_mgt_h__
#define __state_mgt_h__

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "imu_types.h"

typedef enum security_lvl_enum
{
    SECU_LVL_1,
    SECU_LVL_2,
    SECU_LVL_3
} security_lvl_t;

// // WIFI
extern EventGroupHandle_t wifi_event_group;
extern const int CONNECTED_BIT;

void app_state_init(void);
bool app_state_get_wifi_connected(void);
void app_state_set_wifi_connected(bool state);
void app_state_connect_wifi(char *ssid, char *pass);
void app_state_disconnect_wifi(void);
void app_state_set_last_imu_raw_data(imu_raw_data_t *data);
imu_raw_data_t *app_state_get_last_imu_raw_data(void);
QueueHandle_t *app_state_get_imu_queue(void);
QueueHandle_t *app_state_get_aws_mqtt_activity_queue(void);
security_lvl_t app_state_get_security_lvl(void);
void app_state_set_security_lvl(security_lvl_t lvl);

#endif