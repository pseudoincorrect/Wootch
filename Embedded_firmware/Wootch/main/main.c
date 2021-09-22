// ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
// APP
#include "gui.h"
#include "gui_screens.h"
#include "imu_sample.h"
#include "aws_connect.h"
#include "wifi_connect.h"
#include "app_nvs.h"
#include "app_state.h"

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
    app_state_init();
    app_nvs_init();

    // Wi-Fi
    wifi_init();

    // AWS IOT
    // xTaskCreatePinnedToCore(&aws_iot_mqtt_manage_task, "aws_iot_mqtt_manage_task",
    //                          9216, NULL, 5, NULL, 1);
    // xTaskCreatePinnedToCore(&aws_iot_publish_1_task, "aws_iot_publish_1_task",
    //                          9216, NULL, 5, NULL, 1);
    // xTaskCreatePinnedToCore(&aws_iot_publish_2_task, "aws_iot_publish_2_task",
    //                          9216, NULL, 5, NULL, 1);


    // LVGL (Light and Versatile Graphical Library)
    // initialize GUI callbacks
    xTaskCreatePinnedToCore(gui_task, "gui_task", 4096 * 2, NULL, 0, NULL, 1);

    // IMU
    imu_init();

    xTaskCreate(imu_task, "imu_task", 1024 * 2, (void *)1, 10, NULL);

}