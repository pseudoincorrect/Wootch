#if !defined(__project_extern_variables_h__)
#define __project_extern_variables_h__

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "imu_types.h"

// WIFI 
extern EventGroupHandle_t wifi_event_group;
extern const int CONNECTED_BIT;

// IMU
extern QueueHandle_t imu_queue;
extern imu_raw_data_t last_imu_raw_data;

// AWS

#endif // __project_extern_variables_h__

