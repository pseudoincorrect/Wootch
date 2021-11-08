#include "app_state.h"
#include "wifi_connect.h"
#include "esp_log.h"
#include "freertos/queue.h"

static const char *TAG = "STATE";

static bool wifi_connected;
static security_lvl_t security_lvl;
static int acceleration_thresh;
QueueHandle_t imu_queue;

/**
 * @brief Initiate all Application State variables
 */
void app_state_init(void)
{
    imu_queue = xQueueCreate(10, sizeof(imu_raw_data_t));
    wifi_connected = false;
    security_lvl = SECU_LVL_1;
    acceleration_thresh = 1000;
}

/**
 * @brief Get wheither the wifi is connected
 */
bool app_state_get_wifi_connected(void)
{
    return wifi_connected;
}

/**
 * @brief Set Wifi state
 */
void app_state_set_wifi_connected(bool state)
{
    wifi_connected = state;
}

/**
 * @brief starte the wifi connection
 * 
 * @param ssid Wifi SSID
 * @param pass Wifi Password
 */
void app_state_connect_wifi(char *ssid, char *pass)
{
    wifi_connect(ssid, pass);
}

/**
 * @brief Disconnect the wifi
 */
void app_state_disconnect_wifi(void)
{
    wifi_disconnect();
}

/**
 * @brief Get the IMU queue handle
 * 
 * @return QueueHandle_t* IMU queue handle
 */
QueueHandle_t *app_state_get_imu_queue(void)
{
    return &imu_queue;
}