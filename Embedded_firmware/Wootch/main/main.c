#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "gui.h"
#include "imu_sample.h"
#include "aws_connect.h"
#include "wifi_connect.h"
#include "app_nvs.h"
#include "app_state.h"
#include "app_processing.h"
#include "aws_mqtt_msg.h"

static const char *TAG = "MAIN";

/**
 * @brief Program main entry point
 */
void app_main(void)
{
    app_state_init();
    app_nvs_init();

    // Wi-Fi
    wifi_init();
    app_state_connect_wifi("Maxi", "notmyrealpassword");

    // AWS IOT
    xTaskCreatePinnedToCore(&aws_iot_mqtt_manage_task, "aws_iot_mqtt_manage_task",
                            9216, NULL, 5, NULL, 1);

    // LVGL (Light and Versatile Graphical Library)
    xTaskCreatePinnedToCore(gui_task, "gui_task", 4096 * 2, NULL, 0, NULL, 1);

    // IMU
    imu_init();
    xTaskCreate(imu_task, "imu_task", 1024 * 2, (void *)1, 10, NULL);

    // App Processing
    xTaskCreatePinnedToCore(&app_processing_task, "app_processing_task",
                            2048, NULL, 5, NULL, 1);
}
