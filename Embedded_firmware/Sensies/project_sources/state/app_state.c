#include "app_state.h"
#include "wifi_connect.h"
#include "esp_log.h"

static const char *TAG = "STATE";

static bool wifi_connected;

void app_state_init(void)
{
    wifi_connected = false;
}

bool app_state_get_wifi_connected(void)
{

    return wifi_connected;
}

void app_state_set_wifi_connected(bool state)
{
    ESP_LOGI(TAG, "wifi is connected = %d", state);
    wifi_connected = state;
}

void app_state_connect_wifi(char *ssid, char *pass)
{
    wifi_connect(ssid, pass);
}

void app_state_disconnect_wifi()
{
    wifi_disconnect();
}