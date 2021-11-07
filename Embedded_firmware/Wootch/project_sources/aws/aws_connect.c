#include "aws_connect.h"
// STD
#include <string.h>
// ESP-IDF
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
// AWS IOT
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
// WIFI
#include "esp_wifi.h"
// MISC
#include "app_state.h"
// GUI
#include "old_gui.h"
// CERTS
#include "dev_id.h"

////////////////////////////////////////////////////////////////////////////////
// GLOBAL, EXTERN AND STATIC VARIABLES
// WIFI
// extern EventGroupHandle_t wifi_event_group;
// extern const int CONNECTED_BIT;
// AWS-IOT
//// aws root
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_aws_root_ca_pem_end");
//// certificate
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_pem_crt_end[] asm("_binary_certificate_pem_crt_end");
//// private key
extern const uint8_t private_pem_key_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_pem_key_end[] asm("_binary_private_pem_key_end");
//// MQTT
char HostAddress[255] = AWS_IOT_MQTT_HOST;
uint32_t port = AWS_IOT_MQTT_PORT;
// ESP LOG TAG
static const char *TAG = "AWS_CON";
// Topics
char TOPIC_NOTIF[128];
char TOPIC_ACTIV[128];

/*******************************************************************************
 * @brief
 * @param
 */
void iot_subscribe_notif_alert_callback_handler(AWS_IoT_Client *pClient,
                                                char *topicName, uint16_t topicNameLen,
                                                IoT_Publish_Message_Params *params, void *pData)
{
    ESP_LOGI(TAG, "Subscribe alert callback");
    ESP_LOGI(TAG, "%.*s\t%.*s", topicNameLen, topicName, (int)params->payloadLen,
             (char *)params->payload);

    // display_alert();
}

/*******************************************************************************
 * @brief
 * @param
 */
void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data)
{
    ESP_LOGW(TAG, "MQTT Disconnect");
    IoT_Error_t rc = FAILURE;

    if (NULL == pClient)
        return;

    if (aws_iot_is_autoreconnect_enabled(pClient))
        ESP_LOGI(TAG, "Auto Reconnect is enabled, Reconnecting attempt will start now");
    else
    {
        ESP_LOGW(TAG, "Auto Reconnect not enabled. Starting manual reconnect...");
        rc = aws_iot_mqtt_attempt_reconnect(pClient);
        if (NETWORK_RECONNECTED == rc)
            ESP_LOGW(TAG, "Manual Reconnect Successful");
        else
            ESP_LOGW(TAG, "Manual Reconnect Failed - %d", rc);
    }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

AWS_IoT_Client client;
IoT_Client_Init_Params mqtt_init_params;

/*******************************************************************************
 * @brief
 * @param
 */
void mqtt_init(void)
{
    IoT_Error_t rc = FAILURE;
    mqtt_init_params = iotClientInitParamsDefault;

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
    if (SUCCESS != rc)
    {
        ESP_LOGE(TAG, "aws_iot_mqtt_init returned error : %d ", rc);
        abort();
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
void mqtt_connect(void)
{
    IoT_Error_t rc = FAILURE;
    IoT_Client_Connect_Params connect_params = iotClientConnectParamsDefault;

    connect_params.keepAliveIntervalInSec = 10;
    connect_params.isCleanSession = true;
    connect_params.MQTTVersion = MQTT_3_1_1;
    /* Client ID is set in the menuconfig of the example */
    connect_params.pClientID = AWS_THING_NAME;
    connect_params.clientIDLen = (uint16_t)strlen(AWS_THING_NAME);
    connect_params.isWillMsgPresent = false;

    ESP_LOGI(TAG, "Connecting to %s AWS...", AWS_THING_NAME);
    do
    {
        rc = aws_iot_mqtt_connect(&client, &connect_params);
        if (SUCCESS != rc)
        {
            ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", rc, mqtt_init_params.pHostURL,
                     mqtt_init_params.port);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    } while (SUCCESS != rc);

    rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
    if (SUCCESS != rc)
    {
        ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", rc);
        abort();
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
void mqtt_subscribe_notification(void)
{
    IoT_Error_t rc = FAILURE;

    sprintf(TOPIC_NOTIF, "WootchDev/device/%s/notification/alert", AWS_THING_NAME);
    const int TOPIC_NOTIF_LEN = strlen(TOPIC_NOTIF);

    ESP_LOGI(TAG, "TOPIC_NOTIF     : %s", TOPIC_NOTIF);
    ESP_LOGI(TAG, "TOPIC_NOTIF_LEN : %d", TOPIC_NOTIF_LEN);

    rc = aws_iot_mqtt_subscribe(&client, TOPIC_NOTIF, TOPIC_NOTIF_LEN, QOS0,
                                iot_subscribe_notif_alert_callback_handler,
                                NULL);
    if (SUCCESS != rc)
    {
        ESP_LOGE(TAG, "error subscribing alert : %d", rc);
        vTaskDelay(5000 / portTICK_RATE_MS);
        abort();
    }

    ESP_LOGI(TAG, "Subscribed to alert notifs...");
}

/*******************************************************************************
 * @brief
 * @param
 */
void mqtt_publish(void)
{
    IoT_Error_t rc = SUCCESS;
    char cPayload[100];
    IoT_Publish_Message_Params params_qos0;
    IoT_Publish_Message_Params params_qos1;
    int32_t pub_cnt = 0;

    sprintf(TOPIC_ACTIV, "WootchDev/device/%s/data/activity", AWS_THING_NAME);
    const int TOPIC_ACTIV_LEN = strlen(TOPIC_ACTIV);

    params_qos0.qos = QOS0;
    params_qos0.payload = (void *)cPayload;
    params_qos0.isRetained = 0;

    while ((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc))
    {
        //Max time the yield function will wait for read messages
        rc = aws_iot_mqtt_yield(&client, 100);
        if (rc == NETWORK_ATTEMPTING_RECONNECT)
        {
            vTaskDelay(10000 / portTICK_RATE_MS);
            continue;
        }

        ESP_LOGI(TAG, "publishing a message on %s", TOPIC_ACTIV);

        sprintf(cPayload, "{ \"device\" : \"debug1\", \"data\": \"%d\"}", pub_cnt++);
        params_qos0.payloadLen = strlen(cPayload);

        rc = aws_iot_mqtt_publish(&client, TOPIC_ACTIV, TOPIC_ACTIV_LEN, &params_qos0);
        if (rc != SUCCESS)
        {
            ESP_LOGE(TAG, "publish error %d", rc);
            abort();
        }

        vTaskDelay(10000 / portTICK_RATE_MS);
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
void aws_iot_mqtt_manage_task(void *param)
{
    mqtt_init();

    ESP_LOGI(TAG, "Waiting for internet connection...");

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    ESP_LOGI(TAG, "Connected to internet !");

    mqtt_connect();

    ESP_LOGI(TAG, "Connected to AWS !");

    mqtt_subscribe_notification();

    ESP_LOGI(TAG, "subscribed to %s !", TOPIC_NOTIF);

    mqtt_publish();

    ESP_LOGI(TAG, "published to %s !", TOPIC_ACTIV);
}

/*******************************************************************************
 * @brief
 * @param
 */
void aws_iot_publish_1_task(void *param)
{
}
