#include "wifi_connect.h"
// STD
#include <string.h>
// ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "app_state.h"
#include "app_state.h"

// LOG module
static const char *TAG = "WIFI";
// Wi-Fi
EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

/**
 * @brief Wifi state event handler
 * 
 * @param ctx Unused
 * @param event Wifi event
 * @return esp_err_t error code
 */
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        app_state_set_wifi_connected(true);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
               auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        app_state_set_wifi_connected(false);
        break;
    default:
        break;
    }
    return ESP_OK;
}

/**
 * @brief Initialise wifi and event handler
 */
void wifi_init(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

/**
 * @brief Connect to a wifi network
 * 
 * @param ssid Wifi SSID
 * @param password Wifi Password
 */
void wifi_connect(char *ssid, char *password)
{
    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, 32);
    strncpy((char *)wifi_config.sta.password, password, 32);
    ESP_LOGI(TAG, "SSID %s", wifi_config.sta.ssid);
    ESP_LOGI(TAG, "PASS %s", wifi_config.sta.password);

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

/**
 * @brief Disconnect from the current wifi network
 */
void wifi_disconnect(void)
{
    ESP_ERROR_CHECK(esp_wifi_stop());
}