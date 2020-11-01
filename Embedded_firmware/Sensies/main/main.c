#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

/* Littlevgl specific */
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"

// MPU6050 IMU
#include "mpu60x0.h"

#define LV_TICK_PERIOD_MS 1

static const char *TAG = "MAIN";
mpu_handle_t mpu;
SemaphoreHandle_t xGuiSemaphore;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void i2c_slave_init(void);
static void i2c_test_task(void *arg);
static void guiTask(void *pvParameter);
static void create_demo_application(void);
static void lv_tick_task(void *arg);

// #### ##     ## ##     ##
//  ##  ###   ### ##     ##
//  ##  #### #### ##     ##
//  ##  ## ### ## ##     ##
//  ##  ##     ## ##     ##
//  ##  ##     ## ##     ##
// #### ##     ##  #######

static void i2c_slave_init(void)
{
    esp_err_t err;
    mpu.addr = 104;
    mpu.bus.num = 1;
    mpu.bus.timeout = 50;
    mpu.init.clk_speed = 400000;
    mpu.init.sda_io_num = GPIO_NUM_32;
    mpu.init.scl_io_num = GPIO_NUM_33;
    mpu.init.sda_pullup_en = false;
    mpu.init.scl_pullup_en = false;
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

static void i2c_test_task(void *arg)
{
    esp_err_t err;
    printf("Reading sensor data:\n");
    raw_axes_t accel_raw;   // x, y, z axes as int16
    raw_axes_t gyro_raw;    // x, y, z axes as int16
    while (true)
    {
        err = mpu_acceleration(&mpu, &accel_raw);  // fetch raw data from the registers
        ESP_ERROR_CHECK(err);
        err = mpu_rotation(&mpu, &gyro_raw);       // fetch raw data from the registers
        ESP_ERROR_CHECK(err);
        printf("accel: %d\t %d\t %d\n", accel_raw.x, accel_raw.y, accel_raw.z);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// ##       ##     ##  ######   ##
// ##       ##     ## ##    ##  ##
// ##       ##     ## ##        ##
// ##       ##     ## ##   #### ##
// ##        ##   ##  ##    ##  ##
// ##         ## ##   ##    ##  ##
// ########    ###     ######   ########

static void guiTask(void *pvParameter)
{

    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init();
    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();
    static lv_color_t buf1[DISP_BUF_SIZE];
    static lv_color_t buf2[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args =
    {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer,
                    LV_TICK_PERIOD_MS * 1000));

    /* Create the demo application */
    create_demo_application();

    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
    /* A task should NEVER return */
    vTaskDelete(NULL);
}

static void create_demo_application(void)
{
    lv_demo_widgets();
}

static void lv_tick_task(void *arg)
{
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}


// ##     ##    ###    #### ##    ##
// ###   ###   ## ##    ##  ###   ##
// #### ####  ##   ##   ##  ####  ##
// ## ### ## ##     ##  ##  ## ## ##
// ##     ## #########  ##  ##  ####
// ##     ## ##     ##  ##  ##   ###
// ##     ## ##     ## #### ##    ##

void app_main(void)
{
    i2c_slave_init();
    xTaskCreate(i2c_test_task, "i2c_test_task_1", 1024 * 2, (void *)1, 10, NULL);
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
}