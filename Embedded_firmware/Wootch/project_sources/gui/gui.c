#include "gui.h"
// ESP-IDF
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
// LVGL
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "screens.h"

#define LV_TICK_PERIOD_MS 1

SemaphoreHandle_t xGuiSemaphore;

static void lv_tick_task(void *arg);
static const char *TAG = "GUI";

/**
 * @brief Main GUI task
 * 
 * @param arg  unused but needed by FreeRTOS
 */
void gui_task(void *arg)
{
    (void)arg;
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
            .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer,
                                             LV_TICK_PERIOD_MS * 1000));

    // start the GUI
    init_screens();

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

/**
 * @brief LV Tick increase for realtime operation
 * 
 * @param arg unused but needed by FreeRTOS
 */
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}