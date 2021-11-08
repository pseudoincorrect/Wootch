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

void app_state_init(void);
bool app_state_get_wifi_connected(void);
void app_state_set_wifi_connected(bool state);
void app_state_connect_wifi(char *ssid, char *pass);
void app_state_disconnect_wifi();
QueueHandle_t *app_state_get_imu_queue(void);

// WIFI
extern EventGroupHandle_t wifi_event_group;
extern const int CONNECTED_BIT;

// IMU
extern QueueHandle_t imu_queue;
extern imu_raw_data_t last_imu_raw_data;

#endif