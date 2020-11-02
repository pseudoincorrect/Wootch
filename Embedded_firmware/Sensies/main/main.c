// STD
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
// ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_freertos_hooks.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"
// LVGL
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
// #include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"
#include "screens.h"
// MPU6050 IMU
#include "mpu60x0.h"
// AWS-IOT
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"


////////////////////////////////////////////////////////////////////////////////
// DEFINES
//
// LVGL
#define LV_TICK_PERIOD_MS 1
#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)
// AWS-IOT
// #define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
// #define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

#define EXAMPLE_WIFI_SSID "maxi"
#define EXAMPLE_WIFI_PASS "cornimont"
#define CONFIG_AWS_EXAMPLE_CLIENT_ID "myesp32"


////////////////////////////////////////////////////////////////////////////////
//  STATIC PROTOTYPES
//
// IMU
static void i2c_slave_init(void);
static void i2c_test_task(void *arg);
// LVGL
static void guiTask(void *pvParameter);
static void create_demo_application(void);
static void lv_tick_task(void *arg);
// AWS-IOT
static esp_err_t event_handler(void *ctx, system_event_t *event);
void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName,
                                    uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params, void *pData);
void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data);
void aws_iot_task(void *param);
static void initialise_wifi(void);

////////////////////////////////////////////////////////////////////////////////
//  STATIC AND GLOBAL VARIABLES
//
// LOG module
static const char *TAG = "MAIN";
// IMU
mpu_handle_t mpu;
// LVGL
SemaphoreHandle_t xGuiSemaphore;
// AWS-IOT
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
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
    sensie_gui();
    // lv_demo_widgets();
}

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

void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName,
                                    uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params, void *pData)
{
    ESP_LOGI(TAG, "Subscribe callback");
    ESP_LOGI(TAG, "%.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen,
             (char *)params->payload);
}

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data)
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

void aws_iot_task(void *param)
{
    char cPayload[100];

    int32_t i = 0;

    IoT_Error_t rc = FAILURE;

    AWS_IoT_Client client;
    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

    IoT_Publish_Message_Params paramsQOS0;
    IoT_Publish_Message_Params paramsQOS1;

    ESP_LOGI(TAG, "AWS IoT SDK Version %d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR,
             VERSION_PATCH, VERSION_TAG);

    mqttInitParams.enableAutoReconnect = false; // We enable this later below
    mqttInitParams.pHostURL = HostAddress;
    mqttInitParams.port = port;
    mqttInitParams.pRootCALocation = (const char *)aws_root_ca_pem_start;
    mqttInitParams.pDeviceCertLocation = (const char *)certificate_pem_crt_start;
    mqttInitParams.pDevicePrivateKeyLocation = (const char *)private_pem_key_start;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = NULL;

    rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if(SUCCESS != rc)
    {
        ESP_LOGE(TAG, "aws_iot_mqtt_init returned error : %d ", rc);
        abort();
    }

    /* Wait for WiFI to show as connected */
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    /* Client ID is set in the menuconfig of the example */
    connectParams.pClientID = CONFIG_AWS_EXAMPLE_CLIENT_ID;
    connectParams.clientIDLen = (uint16_t) strlen(CONFIG_AWS_EXAMPLE_CLIENT_ID);
    connectParams.isWillMsgPresent = false;

    ESP_LOGI(TAG, "Connecting to AWS...");
    do
    {
        rc = aws_iot_mqtt_connect(&client, &connectParams);
        if(SUCCESS != rc)
        {
            ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL,
                     mqttInitParams.port);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }
    while(SUCCESS != rc);

    /*
     * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
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

    paramsQOS0.qos = QOS0;
    paramsQOS0.payload = (void *) cPayload;
    paramsQOS0.isRetained = 0;

    paramsQOS1.qos = QOS1;
    paramsQOS1.payload = (void *) cPayload;
    paramsQOS1.isRetained = 0;

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
        paramsQOS0.payloadLen = strlen(cPayload);
        rc = aws_iot_mqtt_publish(&client, TOPIC, TOPIC_LEN, &paramsQOS0);

        sprintf(cPayload, "%s : %d ", "hello from ESP32 (QOS1)", i++);
        paramsQOS1.payloadLen = strlen(cPayload);
        rc = aws_iot_mqtt_publish(&client, TOPIC, TOPIC_LEN, &paramsQOS1);
        if(rc == MQTT_REQUEST_TIMEOUT_ERROR)
        {
            ESP_LOGW(TAG, "QOS1 publish ack not received.");
            rc = SUCCESS;
        }
    }

    ESP_LOGE(TAG, "An error occurred in the main loop.");
    abort();
}

static void initialise_wifi(void)
{
    esp_netif_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config =
    {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
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
    // // AWS-IOT
    // esp_err_t err = nvs_flash_init();
    // if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    // {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     err = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(err);
    // initialise_wifi();
    // xTaskCreatePinnedToCore(&aws_iot_task, "aws_iot_task", 9216, NULL, 5, NULL, 1);

    // // IMU
    // i2c_slave_init();
    // xTaskCreate(i2c_test_task, "i2c_test_task_1", 1024 * 2, (void *)1, 10, NULL);

    // LVGL
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
}