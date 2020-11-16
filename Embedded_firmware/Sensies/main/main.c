// STD
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
// ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
// LVGL
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "screens.h"
// MPU6050 IMU
#include "mpu60x0.h"
// AWS-IOT (Amazon Web Services - Internet of Things)
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

/*******************************************************************************
 * @brief
 * @param
 */
// DEFINES
//
// LVGL
#define LV_TICK_PERIOD_MS 1
#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

// Wi-Fi and AWS-IOT
// #define CONFIG_WIFI_SSID         ""
// #define CONFIG_WIFI_PASSWORD     ""
// #define AWS_THING_CLIENT_ID      ""

/*******************************************************************************
 * @brief
 * @param
 */
//  STATIC PROTOTYPES

// IMU (Inertial Management Unit)
static void i2c_slave_init(void);
static void imu_task(void *arg);

// LVGL (Light and Versatile Graphical Library)
static void gui_task(void *pvParameter);
static void lv_tick_task(void *arg);

// AWS-IOT (Amazon Web Services - Internet of Things)
static esp_err_t event_handler(void *ctx, system_event_t *event);
void iot_subscribe_callback_handler(AWS_IoT_Client *pClient,
                                    char *topicName,
                                    uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params,
                                    void *pData);
void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data);
void aws_iot_task(void *param);

// Wi-Fi
static void initialise_wifi(void);
static void disconnect_wifi(void);
static void connect_wifi(char* ssid, char* passw);

/*******************************************************************************
 * @brief
 * @param
 */
//  STATIC AND GLOBAL VARIABLES
//
// LOG module
static const char *TAG = "MAIN";

// IMU (Inertial Management Unit)
mpu_handle_t mpu; // "mpu" is the model reference

// LVGL (Light and Versatile Graphical Library)
SemaphoreHandle_t xGuiSemaphore;

// AWS-IOT (Amazon Web Services - Internet of Things)
static EventGroupHandle_t wifi_event_group;
extern const uint8_t aws_root_ca_pem_start[]
asm("_binary_aws_root_ca_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_aws_root_ca_pem_end");
extern const uint8_t certificate_pem_crt_start[]
asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_pem_crt_end[]
asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_pem_key_start[]
asm("_binary_private_pem_key_start");
extern const uint8_t private_pem_key_end[] asm("_binary_private_pem_key_end");
char HostAddress[255] = AWS_IOT_MQTT_HOST;
uint32_t port = AWS_IOT_MQTT_PORT;

// Wi-Fi
const int CONNECTED_BIT = BIT0;

// #### ##     ## ##     ##
//  ##  ###   ### ##     ##
//  ##  #### #### ##     ##
//  ##  ## ### ## ##     ##
//  ##  ##     ## ##     ##
//  ##  ##     ## ##     ##
// #### ##     ##  #######

/*******************************************************************************
 * @brief
 * @param
 */
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

/*******************************************************************************
 * @brief
 * @param
 */
static void imu_task(void *arg)
{
    esp_err_t err;
    raw_axes_t accel_raw;   // x, y, z axes as int16
    raw_axes_t gyro_raw;    // x, y, z axes as int16
    while (true)
    {
        err = mpu_acceleration(&mpu, &accel_raw);  // fetch raw data
        ESP_ERROR_CHECK(err);
        err = mpu_rotation(&mpu, &gyro_raw);       // fetch raw data
        ESP_ERROR_CHECK(err);
        // printf("accel: %d\t %d\t %d\n", accel_raw.x, accel_raw.y, accel_raw.z);
        screen_imu.g_x = accel_raw.x;
        screen_imu.g_y = accel_raw.y;

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

// ##       ##     ##  ######   ##
// ##       ##     ## ##    ##  ##
// ##       ##     ## ##        ##
// ##       ##     ## ##   #### ##
// ##        ##   ##  ##    ##  ##
// ##         ## ##   ##    ##  ##
// ########    ###     ######   ########

/*******************************************************************************
 * @brief
 * @param
 */
static void gui_task(void *pvParameter)
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

    // initialize GUI callbacks
    gui_init_cb(connect_wifi, disconnect_wifi);

    // start the GUI
    start_gui();

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

/*******************************************************************************
 * @brief
 * @param
 */
static void lv_tick_task(void *arg)
{
    (void) arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

//    ###     ##      ##   ######            ####   #######   ########
//   ## ##    ##  ##  ##  ##    ##            ##   ##     ##     ##
//  ##   ##   ##  ##  ##  ##                  ##   ##     ##     ##
// ##     ##  ##  ##  ##   ######   #######   ##   ##     ##     ##
// #########  ##  ##  ##        ##            ##   ##     ##     ##
// ##     ##  ##  ##  ##  ##    ##            ##   ##     ##     ##
// ##     ##   ###  ###    ######            ####   #######      ##

/*******************************************************************************
 * @brief
 * @param
 */
void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName,
                                    uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params, void *pData)
{
    ESP_LOGI(TAG, "Subscribe callback");
    ESP_LOGI(TAG, "%.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen,
             (char *)params->payload);
}

/*******************************************************************************
 * @brief
 * @param
 */
void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data)
{
    ESP_LOGW(TAG, "MQTT Disconnect");
    IoT_Error_t rc = FAILURE;

    if(NULL == pClient)
    {
        return;
    }

    if(aws_iot_is_autoreconnect_enabled(pClient))
    {
        ESP_LOGI(TAG, "Auto Reconnect is enabled, Reconnecting attempt will start now");
    }
    else
    {
        ESP_LOGW(TAG, "Auto Reconnect not enabled. Starting manual reconnect...");
        rc = aws_iot_mqtt_attempt_reconnect(pClient);
        if(NETWORK_RECONNECTED == rc)
        {
            ESP_LOGW(TAG, "Manual Reconnect Successful");
        }
        else
        {
            ESP_LOGW(TAG, "Manual Reconnect Failed - %d", rc);
        }
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
void aws_iot_task(void *param)
{
    char cPayload[100];

    int32_t i = 0;

    IoT_Error_t rc = FAILURE;

    AWS_IoT_Client client;
    IoT_Client_Init_Params mqtt_init_params = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connect_params = iotClientConnectParamsDefault;

    IoT_Publish_Message_Params params_qos0;
    IoT_Publish_Message_Params params_qos1;

    ESP_LOGI(TAG, "AWS IoT SDK Version %d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR,
             VERSION_PATCH, VERSION_TAG);

    mqtt_init_params.enableAutoReconnect = false; // We enable this later below
    mqtt_init_params.pHostURL = HostAddress;
    mqtt_init_params.port = port;
    mqtt_init_params.pRootCALocation = (const char *)aws_root_ca_pem_start;
    mqtt_init_params.pDeviceCertLocation = (const char *)certificate_pem_crt_start;
    mqtt_init_params.pDevicePrivateKeyLocation = (const char *)
            private_pem_key_start;
    mqtt_init_params.mqttCommandTimeout_ms = 20000;
    mqtt_init_params.tlsHandshakeTimeout_ms = 5000;
    mqtt_init_params.isSSLHostnameVerify = true;
    mqtt_init_params.disconnectHandler = disconnect_callback_handler;
    mqtt_init_params.disconnectHandlerData = NULL;

    rc = aws_iot_mqtt_init(&client, &mqtt_init_params);
    if(SUCCESS != rc)
    {
        ESP_LOGE(TAG, "aws_iot_mqtt_init returned error : %d ", rc);
        abort();
    }

    ESP_LOGI(TAG, "Waiting for internet connection...");

    /* Wait for WiFI to show as connected */
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    ESP_LOGI(TAG, "Connected to internet !");

    connect_params.keepAliveIntervalInSec = 10;
    connect_params.isCleanSession = true;
    connect_params.MQTTVersion = MQTT_3_1_1;
    /* Client ID is set in the menuconfig of the example */
    connect_params.pClientID = CONFIG_AWS_THING_CLIENT_ID;
    connect_params.clientIDLen = (uint16_t) strlen(CONFIG_AWS_THING_CLIENT_ID);
    connect_params.isWillMsgPresent = false;

    ESP_LOGI(TAG, "Connecting to AWS...");
    do
    {
        rc = aws_iot_mqtt_connect(&client, &connect_params);
        if(SUCCESS != rc)
        {
            ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", rc, mqtt_init_params.pHostURL,
                     mqtt_init_params.port);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }
    while(SUCCESS != rc);

    /*
     * Enable Auto Reconnect functionality. Minimum and Maximum time of
     * Exponential backoff are set in aws_iot_config.h
     *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
     *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
     */
    rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
    if(SUCCESS != rc)
    {
        ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", rc);
        abort();
    }

    const char *TOPIC = "test_topic/esp32";
    const int TOPIC_LEN = strlen(TOPIC);

    ESP_LOGI(TAG, "Subscribing...");
    rc = aws_iot_mqtt_subscribe(&client, TOPIC, TOPIC_LEN, QOS0,
                                iot_subscribe_callback_handler, NULL);
    if(SUCCESS != rc)
    {
        ESP_LOGE(TAG, "Error subscribing : %d ", rc);
        abort();
    }

    sprintf(cPayload, "%s : %d ", "hello from SDK", i);

    params_qos0.qos = QOS0;
    params_qos0.payload = (void *) cPayload;
    params_qos0.isRetained = 0;

    params_qos1.qos = QOS1;
    params_qos1.payload = (void *) cPayload;
    params_qos1.isRetained = 0;

    while((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc
            || SUCCESS == rc))
    {

        //Max time the yield function will wait for read messages
        rc = aws_iot_mqtt_yield(&client, 100);
        if(NETWORK_ATTEMPTING_RECONNECT == rc)
        {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
        }

        ESP_LOGI(TAG, "Stack remaining for task '%s' is %d bytes",
                 pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(1000 / portTICK_RATE_MS);
        sprintf(cPayload, "%s : %d ", "hello from ESP32 (QOS0)", i++);
        params_qos0.payloadLen = strlen(cPayload);
        rc = aws_iot_mqtt_publish(&client, TOPIC, TOPIC_LEN, &params_qos0);

        sprintf(cPayload, "%s : %d ", "hello from ESP32 (QOS1)", i++);
        params_qos1.payloadLen = strlen(cPayload);
        rc = aws_iot_mqtt_publish(&client, TOPIC, TOPIC_LEN, &params_qos1);
        if(rc == MQTT_REQUEST_TIMEOUT_ERROR)
        {
            ESP_LOGW(TAG, "QOS1 publish ack not received.");
            rc = SUCCESS;
        }
    }

    ESP_LOGE(TAG, "An error occurred in the main loop.");
    abort();
}

// ##      ## #### ######## ####
// ##  ##  ##  ##  ##        ##
// ##  ##  ##  ##  ##        ##
// ##  ##  ##  ##  ######    ##
// ##  ##  ##  ##  ##        ##
// ##  ##  ##  ##  ##        ##
//  ###  ###  #### ##       ####

/*******************************************************************************
 * @brief
 * @param
 */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP32 WiFi libs don't currently
               auto-reassociate. */
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

/*******************************************************************************
 * @brief
 * @param
 */
static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

/*******************************************************************************
 * @brief
 * @param
 */
static void connect_wifi(char* ssid, char* password)
{
    wifi_config_t wifi_config = {0};
    strncpy((char*) wifi_config.sta.ssid, ssid, 32);
    strncpy((char*) wifi_config.sta.password, password, 32);
    ESP_LOGI(TAG, "SSID %s", wifi_config.sta.ssid);
    ESP_LOGI(TAG, "PASS %s", wifi_config.sta.password);

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

/*******************************************************************************
 * @brief
 * @param
 */
static void disconnect_wifi(void)
{
    ESP_ERROR_CHECK(esp_wifi_stop());
}

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
    // Flash storage
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Wi-Fi
    initialise_wifi();

    // AWS IOT
    // xTaskCreatePinnedToCore(&aws_iot_task, "aws_iot_task", 9216, NULL, 5, NULL, 1);

    // IMU
    i2c_slave_init();
    xTaskCreate(imu_task, "imu_task_1", 1024 * 2, (void *)1, 10, NULL);

    // LVGL (Light and Versatile Graphical Library)
    xTaskCreatePinnedToCore(gui_task, "gui", 4096 * 2, NULL, 0, NULL, 1);
}