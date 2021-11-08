#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "app_processing.h"
#include "app_state.h"
#include "imu_types.h"

#define ACTIVITY_ACC_THRESHOLD 500
#define ACTIVITY_ROT_THRESHOLD 1000

static const char *TAG = "PROC";

/**
 * @brief Main loop for processing data and gluing part of the
 * app together (AWS, IMU, GUI, etc...)
 * 
 * @param param unused but required by FreeRTOS
 */
void app_processing_task(void *param)
{
    QueueHandle_t *imu_queue = app_state_get_imu_queue();
    imu_raw_data_t imu_data;

    while (1)
    {
        int rc;
        int cnt = 0;
        int sum_acc = 0;
        int prev_sum_acc = 0;
        int sum_rot = 0;
        int prev_sum_rot = 0;
        bool activity_detected = false;
        do
        {
            rc = xQueueReceive(*imu_queue, &imu_data, 0);
            if (rc)
            {
                prev_sum_acc = sum_acc;
                sum_acc = sqrt(imu_data.g_x * imu_data.g_x + imu_data.g_y * imu_data.g_y + imu_data.g_z * imu_data.g_z);
                sum_rot = sqrt(imu_data.r_x * imu_data.r_x + imu_data.r_y * imu_data.r_y + imu_data.r_z * imu_data.r_z);
                if (cnt >= 2)
                {
                    int diff_acc = abs(sum_acc - prev_sum_acc);
                    int diff_rot = abs(sum_rot - prev_sum_rot);

                    if (!activity_detected && diff_acc > ACTIVITY_ACC_THRESHOLD)
                    {
                        ESP_LOGI(TAG, "Activity acceleration detected %d", diff_acc);
                        activity_detected = true;
                    }

                    if (!activity_detected && diff_rot > ACTIVITY_ROT_THRESHOLD)
                    {
                        ESP_LOGI(TAG, "Activity rotation detected %d", diff_rot);
                        activity_detected = true;
                    }
                }
                cnt++;
            }
        } while (rc);
        if (cnt)
        {
            ESP_LOGI(TAG, "sum_acc = %d", sum_acc);
            ESP_LOGI(TAG, "sum_rot = %d", sum_rot);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}