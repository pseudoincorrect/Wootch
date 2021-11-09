#include "imu_sample.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "mpu60x0.h"
#include "app_state.h"

static const char *TAG = "IMU_TASK";
mpu_handle_t mpu;
QueueHandle_t *imu_queue;

/**
 * @brief Update the data buffers, to be used by outside functions
 * 
 * @param accel_raw IMU acceleration raw data
 * @param gyro_raw IMU rotation raw data
 */
static void update_imu_data(raw_axes_t *accel_raw, raw_axes_t *gyro_raw)
{
    imu_raw_data_t imu_raw_data = {0};
    // Depending on how the chip is oriented to the screen we switch the
    // the x / y and change the sign (+/-)
    imu_raw_data.g_x = accel_raw->x;
    imu_raw_data.g_y = -accel_raw->y;
    imu_raw_data.g_z = accel_raw->z;

    imu_raw_data.r_x = gyro_raw->x;
    imu_raw_data.r_y = gyro_raw->y;
    imu_raw_data.r_z = gyro_raw->z;

    app_state_set_last_imu_raw_data(&imu_raw_data);
    xQueueSend(*imu_queue, &imu_raw_data, pdMS_TO_TICKS(50));
}

/**
 * @brief Init the Inertial Management Unit (IMU) with I2C
 */
void imu_init(void)
{
    esp_err_t err;
    imu_queue = app_state_get_imu_queue();
    mpu.addr = 104;
    mpu.bus.num = 0;
    mpu.bus.timeout = 500;
    mpu.init.clk_speed = 100000;
    mpu.init.sda_io_num = GPIO_NUM_26;
    mpu.init.scl_io_num = GPIO_NUM_27;
    mpu.init.sda_pullup_en = true;
    mpu.init.scl_pullup_en = true;
    err = mpu_initialize_peripheral(&mpu);
    err = mpu_test_connection(&mpu);
    while (err)
    {
        ESP_LOGE(TAG, "Failed to connect to the MPU, error=%#X", err);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        err = mpu_test_connection(&mpu);
    }
    ESP_LOGI(TAG, "MPU connection successful!");
    err = mpu_initialize_chip(&mpu);
    ESP_LOGI(TAG, "i2c_slave_init");
}

/**
 * @brief Main IMU sampling task
 * 
 * @param arg Unused but needed by FreeRTOS
 */
void imu_task(void *arg)
{
    esp_err_t err;
    raw_axes_t accel_raw;
    raw_axes_t gyro_raw;

    while (true)
    {
        err = mpu_acceleration(&mpu, &accel_raw);
        ESP_ERROR_CHECK(err);
        err = mpu_rotation(&mpu, &gyro_raw);
        ESP_ERROR_CHECK(err);
        // ESP_LOGI(TAG,"accel: %d\t %d\t %d\n", accel_raw.x, accel_raw.y, accel_raw.z);
        update_imu_data(&accel_raw, &gyro_raw);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
