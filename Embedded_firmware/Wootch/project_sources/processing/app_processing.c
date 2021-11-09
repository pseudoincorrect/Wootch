#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "app_processing.h"
#include "app_state.h"
#include "imu_types.h"
#include "aws_mqtt_msg.h"

#define ACTIVITY_ACC_THRESHOLD 500
#define ACTIVITY_ROT_THRESHOLD 1000

static const char *TAG = "PROC";

static QueueHandle_t *imu_queue;
static imu_raw_data_t imu_data;
static QueueHandle_t *activity_queue;

/**
 * @brief Main loop for processing data and gluing part of the
 * app together (AWS, IMU, GUI, etc...)
 * 
 * @param param unused but required by FreeRTOS
 */
void app_processing_task(void *param)
{
    imu_queue = app_state_get_imu_queue();
    activity_queue = app_state_get_aws_mqtt_activity_queue();

    while (1)
    {
        int err;
        int cnt = 0;
        int sum_acc = 0;
        int prev_sum_acc = 0;
        int sum_rot = 0;
        int prev_sum_rot = 0;
        bool activity_detected = false;
        int diff_acc = 0;
        int diff_rot = 0;

        do
        {
            err = xQueueReceive(*imu_queue, &imu_data, 0);
            if (err)
            {
                prev_sum_acc = sum_acc;

                sum_acc = sqrt(imu_data.g_x * imu_data.g_x +
                               imu_data.g_y * imu_data.g_y +
                               imu_data.g_z * imu_data.g_z);

                sum_rot = sqrt(imu_data.r_x * imu_data.r_x +
                               imu_data.r_y * imu_data.r_y +
                               imu_data.r_z * imu_data.r_z);
                if (cnt >= 2)
                {
                    diff_acc = abs(sum_acc - prev_sum_acc);
                    diff_rot = abs(sum_rot - prev_sum_rot);

                    if (!activity_detected && diff_acc > ACTIVITY_ACC_THRESHOLD)
                    {
                        ESP_LOGI(TAG, "Acceleration detected %d", diff_acc);
                        ESP_LOGI(TAG, "Acceleration = %d", sum_acc);
                        activity_detected = true;
                    }

                    if (!activity_detected && diff_rot > ACTIVITY_ROT_THRESHOLD)
                    {
                        ESP_LOGI(TAG, "Rotation detected %d", diff_rot);
                        ESP_LOGI(TAG, "Rotation     = %d", sum_rot);
                        activity_detected = true;
                    }
                }
                cnt++;
            }
        } while (err);

        if (activity_detected)
        {
            activity_msg_t activity;
            activity.watchLvl = app_state_get_security_lvl();
            activity.maxAcc = diff_acc;
            activity.accThresh = ACTIVITY_ACC_THRESHOLD;
            activity.maxRot = diff_rot;
            activity.rotThresh = ACTIVITY_ROT_THRESHOLD;
            xQueueSend(*activity_queue, &activity, pdMS_TO_TICKS(50));
        };

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}