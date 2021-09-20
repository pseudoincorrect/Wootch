// ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
// LVGL
#include "gui.h"
#include "gui_screens.h"
// IMU
#include "imu_sample.h"
// AWS-IOT
#include "aws_connect.h"
// WIFI
#include "wifi_connect.h"
// NVS
#include "app_nvs.h"

////////////////////////////////////////////////////////////////////////////////
// STATIC AND GLOBAL VARIABLES
// LOG module Tag
static const char *TAG = "MAIN";

// ##     ##    ###    #### ##    ##
// ###   ###   ## ##    ##  ###   ##
// #### ####  ##   ##   ##  ####  ##
// ## ### ## ##     ##  ##  ## ## ##
// ##     ## #########  ##  ##  ####
// ##     ## ##     ##  ##  ##   ###
// ##     ## ##     ## #### ##    ##

/*******************************************************************************
 * @brief
 * @param
 */
void app_main(void)
{
    // app_nvs_init();

    // Flash storage
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Wi-Fi
    // initialise_wifi();

    // AWS IOT
    // xTaskCreatePinnedToCore(&aws_iot_mqtt_manage_task, "aws_iot_mqtt_manage_task",
    //                          9216, NULL, 5, NULL, 1);
    // xTaskCreatePinnedToCore(&aws_iot_publish_1_task, "aws_iot_publish_1_task",
    //                          9216, NULL, 5, NULL, 1);
    // xTaskCreatePinnedToCore(&aws_iot_publish_2_task, "aws_iot_publish_2_task",
    //                          9216, NULL, 5, NULL, 1);


    // LVGL (Light and Versatile Graphical Library)
    // initialize GUI callbacks
    gui_init_cb(connect_wifi, disconnect_wifi);
    // xTaskCreatePinnedToCore(gui_task, "gui_task", 4096 * 2, NULL, 0, NULL, 1);

    // connect_wifi("Maxi", "notmyrealpassword");
    // app_nvs_test();

    // IMU
    imu_init();
    // vTaskDelay(5000 / portTICK_PERIOD_MS);
    xTaskCreate(imu_task, "imu_task", 1024 * 2, (void *)1, 10, NULL);

}