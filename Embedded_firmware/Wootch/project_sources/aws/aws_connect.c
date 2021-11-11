#include "aws_connect.h"
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "esp_wifi.h"
#include "app_state.h"
#include "dev_id.h"
#include "aws_mqtt_msg.h"

#define MQTT_MSG_BUFFER_SIZE 128

static const char *TAG = "AWS_CON";

extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_aws_root_ca_pem_end");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_pem_crt_end[] asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_pem_key_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_pem_key_end[] asm("_binary_private_pem_key_end");

static char HostAddress[255] = AWS_IOT_MQTT_HOST;
static uint32_t port = AWS_IOT_MQTT_PORT;
static QueueHandle_t *activity_queue;
static QueueHandle_t *pairing_queue;

AWS_IoT_Client client;
IoT_Client_Init_Params mqtt_init_params;

static char topic_notif[128];
static int topic_notif_len;
static char topic_activ[128];
static int topic_activ_len;
static char topic_pairing[128];
static int topic_pairing_len;
static char topic_user[128];
static int topic_user_len;

static char mqtt_msg_buffer[MQTT_MSG_BUFFER_SIZE];

/**
 * @brief Create the MQTT topic string
 */
void create_topics(void)
{
    sprintf(topic_activ, "WootchDev/device/%s/data/activity", AWS_THING_NAME);
    topic_activ_len = strlen(topic_activ);

    sprintf(topic_pairing, "WootchDev/device/%s/pairing/request", AWS_THING_NAME);
    topic_pairing_len = strlen(topic_pairing);

    sprintf(topic_notif, "WootchDev/device/%s/notification/alert", AWS_THING_NAME);
    topic_notif_len = strlen(topic_notif);

    sprintf(topic_user, "WootchDev/device/%s/pairing/user", AWS_THING_NAME);
    topic_user_len = strlen(topic_user);
}

/**
 * @brief Handler for MQTT Topic "notification"
 * 
 * @param pClient connection client
 * @param topicName Topic name
 * @param topicNameLen Topic name length
 * @param params publish parameters
 * @param pData unused
 */
void topic_notif_handler(AWS_IoT_Client *pClient,
                         char *topicName, uint16_t topicNameLen,
                         IoT_Publish_Message_Params *params, void *pData)
{
    ESP_LOGI(TAG, "Notification topic handler");
    ESP_LOGI(TAG, "%.*s\t\"%.*s\"", topicNameLen, topicName, (int)params->payloadLen,
             (char *)params->payload);
}

/**
 * @brief Handler for MQTT Topic "user"
 * 
 * @param pClient connection client
 * @param topicName Topic name
 * @param topicNameLen Topic name length
 * @param params publish parameters
 * @param pData unused
 */
void topic_user_handler(AWS_IoT_Client *pClient,
                        char *topicName, uint16_t topicNameLen,
                        IoT_Publish_Message_Params *params, void *pData)
{
    ESP_LOGI(TAG, "User topic handler");
    ESP_LOGI(TAG, "%.*s\t%.*s", topicNameLen, topicName, (int)params->payloadLen,
             (char *)params->payload);

    // format : {"email":"johndoe@meh.com"}
    char email[64] = {0};
    strncpy(email, (char *)params->payload + 9 * sizeof(char), (int)params->payloadLen - 3);
    app_state_update_user_email(email);
}

/**
 * @brief Disconnect from AWS MQTT
 * 
 * @param pClient AWS IoT client
 * @param data Unused
 */
void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data)
{
    ESP_LOGW(TAG, "MQTT Disconnect");
    IoT_Error_t err = FAILURE;

    if (pClient == NULL)
    {
        return;
    }

    if (aws_iot_is_autoreconnect_enabled(pClient))
    {
        ESP_LOGI(TAG, "Auto Reconnect is enabled, Reconnecting attempt will start now");
    }
    else
    {
        ESP_LOGW(TAG, "Auto Reconnect not enabled. Starting manual reconnect...");
        err = aws_iot_mqtt_attempt_reconnect(pClient);
        if (err == NETWORK_RECONNECTED)
        {
            ESP_LOGW(TAG, "Manual Reconnect Successful");
        }
        else
        {
            ESP_LOGW(TAG, "Manual Reconnect Failed - %d", err);
        }
    }
}

/**
 * @brief init MQTT connection to AWS 
 */
void mqtt_init(void)
{
    IoT_Error_t err = FAILURE;
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

    err = aws_iot_mqtt_init(&client, &mqtt_init_params);
    if (err != SUCCESS)
    {
        ESP_LOGE(TAG, "aws_iot_mqtt_init returned error : %d ", err);
        abort();
    }
}

/**
 * @brief Connect to AWS MQTT broker
 */
void mqtt_connect(void)
{
    IoT_Error_t err = FAILURE;
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
        err = aws_iot_mqtt_connect(&client, &connect_params);
        if (err != SUCCESS)
        {
            ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", err, mqtt_init_params.pHostURL,
                     mqtt_init_params.port);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    } while (err != SUCCESS);

    err = aws_iot_mqtt_autoreconnect_set_status(&client, true);
    if (err != SUCCESS)
    {
        ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", err);
        abort();
    }
}

/**
 * @brief Subscribe to a MQTT topic
 */
void mqtt_subscribe_topic(char *topic, int topic_len, void *topic_handler)
{
    IoT_Error_t err = FAILURE;

    ESP_LOGI(TAG, "topic     : %s", topic);
    ESP_LOGI(TAG, "topic_len : %d", topic_len);

    err = aws_iot_mqtt_subscribe(&client, topic, topic_len, QOS0,
                                 topic_handler, NULL);
    if (err != SUCCESS)
    {
        ESP_LOGE(TAG, "error subscribing topic %s alert : %d", topic, err);
        vTaskDelay(5000 / portTICK_RATE_MS);
        abort();
    }

    ESP_LOGI(TAG, "Subscribed to alert notifs...");
}

void mqtt_subscribe(void)
{
    mqtt_subscribe_topic(topic_notif, topic_notif_len, topic_notif_handler);
    mqtt_subscribe_topic(topic_user, topic_user_len, topic_user_handler);
}

/**
 * @brief Publish on the "Activity" MQTT topic
 */
void mqtt_publish(void)
{
    IoT_Error_t err = SUCCESS;
    char mqtt_msg_buffer[100];
    IoT_Publish_Message_Params params_qos0;
    IoT_Publish_Message_Params params_qos1;
    int32_t pub_cnt = 0;

    params_qos0.qos = QOS0;
    params_qos0.payload = (void *)mqtt_msg_buffer;
    params_qos0.isRetained = 0;

    do
    {
        err = aws_iot_mqtt_yield(&client, 100);
        if (err == NETWORK_ATTEMPTING_RECONNECT)
        {
            vTaskDelay(10000 / portTICK_RATE_MS);
        }
    } while (err == NETWORK_ATTEMPTING_RECONNECT);

    ESP_LOGI(TAG, "publishing a message on %s", topic_activ);

    sprintf(mqtt_msg_buffer, "{ \"device\" : \"debug1\", \"data\": \"%d\"}", pub_cnt++);
    params_qos0.payloadLen = strlen(mqtt_msg_buffer);

    err = aws_iot_mqtt_publish(&client, topic_activ, topic_activ_len, &params_qos0);
    if (err != SUCCESS)
    {
        ESP_LOGE(TAG, "publish error %d", err);
    }
}

/**
 * @brief Set the mqtt buffer to 0s
 */
void reset_mqtt_buffer(void)
{
    memset(mqtt_msg_buffer, 0, sizeof(char) * MQTT_MSG_BUFFER_SIZE);
}

/**
 * @brief Check the activity msg queue and publish pairing request if any
 */
void activity_publishing_process(void)
{
    IoT_Error_t err = SUCCESS;
    activity_msg_t activity_msg;

    err = xQueueReceive(*activity_queue, &activity_msg, 0);

    if (err)
    {
        IoT_Publish_Message_Params params;
        params.qos = QOS0;
        params.payload = (void *)mqtt_msg_buffer;
        params.isRetained = 0;

        reset_mqtt_buffer();
        activity_msg_to_json_string(&activity_msg, mqtt_msg_buffer, MQTT_MSG_BUFFER_SIZE);

        params.payloadLen = strlen(mqtt_msg_buffer);

        err = aws_iot_mqtt_publish(&client, topic_activ, topic_activ_len, &params);
        if (err != SUCCESS)
        {
            ESP_LOGE(TAG, "activity publish error %d", err);
        }
        else
        {
            ESP_LOGI(TAG, "activity published: %s", mqtt_msg_buffer);
        }
    }
}

/**
 * @brief Check the pairing msg queue and publish pairing request if any
 */
void pairing_publishing_process(void)
{
    IoT_Error_t err = SUCCESS;
    pairing_msg_t pairing_msg;

    err = xQueueReceive(*pairing_queue, &pairing_msg, 0);

    if (err)
    {
        IoT_Publish_Message_Params params;
        params.qos = QOS1;
        params.payload = (void *)mqtt_msg_buffer;
        params.isRetained = 0;

        reset_mqtt_buffer();
        pairing_msg_to_json_string(&pairing_msg, mqtt_msg_buffer, MQTT_MSG_BUFFER_SIZE);

        params.payloadLen = strlen(mqtt_msg_buffer);

        err = aws_iot_mqtt_publish(&client, topic_pairing, topic_pairing_len, &params);
        if (err != SUCCESS)
        {
            ESP_LOGE(TAG, "pairing publish error %d", err);
        }
        else
        {
            ESP_LOGI(TAG, "pairing published: %s", mqtt_msg_buffer);
        }
    }
}

/**
 * @brief infinite loop managing MQTT messages pub
 */
void aws_iot_mqtt_loop(void)
{
    IoT_Error_t err = SUCCESS;

    while ((err == NETWORK_ATTEMPTING_RECONNECT || err == NETWORK_RECONNECTED || err == SUCCESS))
    {
        err = aws_iot_mqtt_yield(&client, 100);
        if (err == NETWORK_ATTEMPTING_RECONNECT)
        {
            vTaskDelay(10000 / portTICK_RATE_MS);
            continue;
        }

        activity_publishing_process();
        pairing_publishing_process();

        vTaskDelay(10000 / portTICK_RATE_MS);
    }
}

/**
 * @brief Main task for initiating and managing AWS MQTT
 * 
 * @param param unused but needed by FreeRTOS
 */
void aws_iot_mqtt_manage_task(void *param)
{
    activity_queue = app_state_get_aws_mqtt_activity_queue();
    pairing_queue = app_state_get_aws_mqtt_pairing_queue();

    create_topics();

    mqtt_init();

    ESP_LOGI(TAG, "Waiting for internet connection...");

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    ESP_LOGI(TAG, "Connected to internet !");

    mqtt_connect();

    ESP_LOGI(TAG, "Connected to AWS !");

    mqtt_subscribe();

    ESP_LOGI(TAG, "subscribed to %s !", topic_notif);

    // mqtt_publish();
    // ESP_LOGI(TAG, "published to %s !", topic_activ);

    aws_iot_mqtt_loop();
}
