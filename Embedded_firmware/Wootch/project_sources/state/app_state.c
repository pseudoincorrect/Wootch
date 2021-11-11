#include <string.h>
#include "app_state.h"
#include "wifi_connect.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "aws_mqtt_msg.h"

static const char *TAG = "STATE";

static bool wifi_connected;
static security_lvl_t security_lvl;
static int acceleration_thresh;
static QueueHandle_t imu_queue;
static QueueHandle_t aws_mqtt_activity_queue;
static QueueHandle_t aws_mqtt_pairing_queue;
static imu_raw_data_t last_imu_raw_data;
static char user_email[USER_EMAIL_MAX_SIZE];

/**
 * @brief Initiate all Application State variables
 */
void app_state_init(void)
{
    imu_queue = xQueueCreate(50, sizeof(imu_raw_data_t));
    aws_mqtt_activity_queue = xQueueCreate(10, sizeof(activity_msg_t));
    aws_mqtt_pairing_queue = xQueueCreate(1, sizeof(pairing_msg_t));
    wifi_connected = false;
    security_lvl = SECU_LVL_1;
    acceleration_thresh = 1000;
    strncpy(user_email, "no user paired", USER_EMAIL_MAX_SIZE - 1);
}

/**
 * @brief Get wheither the wifi is connected
 */
bool app_state_get_wifi_connected(void)
{
    return wifi_connected;
}

/**
 * @brief Set Wifi state
 */
void app_state_set_wifi_connected(bool state)
{
    wifi_connected = state;
}

/**
 * @brief starte the wifi connection
 * 
 * @param ssid Wifi SSID
 * @param pass Wifi Password
 */
void app_state_connect_wifi(char *ssid, char *pass)
{
    wifi_connect(ssid, pass);
}

/**
 * @brief Disconnect the wifi
 */
void app_state_disconnect_wifi(void)
{
    wifi_disconnect();
}

/**
 * @brief set the last IMU data
 * 
 * @param data IMU data
 */
void app_state_set_last_imu_raw_data(imu_raw_data_t *data)
{
    memcpy(&last_imu_raw_data, data, sizeof(imu_raw_data_t));
}

/**
 * @brief get the last imu data
 * 
 * @return imu_raw_data_t imu data
 */
imu_raw_data_t *app_state_get_last_imu_raw_data(void)
{
    return &last_imu_raw_data;
}

/**
 * @brief Get the IMU queue handle
 * 
 * @return QueueHandle_t* IMU queue handle
 */
QueueHandle_t *app_state_get_imu_queue(void)
{
    return &imu_queue;
}

/**
 * @brief Get the AWS MQTT Pairing topic msg queue
 * 
 * @return QueueHandle_t* AWS MQTT pairing queue handle
 */
QueueHandle_t *app_state_get_aws_mqtt_pairing_queue(void)
{
    return &aws_mqtt_pairing_queue;
}

/**
 * @brief Get the AWS MQTT Activity topic msg queue
 * 
 * @return QueueHandle_t* AWS MQTT activity queue handle
 */
QueueHandle_t *app_state_get_aws_mqtt_activity_queue(void)
{
    return &aws_mqtt_activity_queue;
}

/**
 * @brief Get the security level
 * 
 * @return security_lvl_t security lvl
 */
security_lvl_t app_state_get_security_lvl(void)
{
    return security_lvl;
}

/**
 * @brief Set the security level
 * 
 * @param lvl security lvl
 */
void app_state_set_security_lvl(security_lvl_t lvl)
{
    security_lvl = lvl;
}

static void generate_secret(char *secret, int secret_size)
{
    int rand;
    for (size_t i = 0; i < secret_size - 1; i++)
    {
        rand = esp_random() % 26;
        secret[i] = 65 + rand;
    }
    secret[secret_size - 1] = '\0';
}

/**
 * @brief Create a random secret and publish a pairing request
 * 
 * @param secret output, buffer containing the pairing secret  
 * @return esp_err_t error code
 */
esp_err_t app_state_start_pairing(char *secret)
{
    pairing_msg_t pairing;
    generate_secret(secret, PAIRING_SECRET_SIZE);

    for (size_t i = 0; i < PAIRING_SECRET_SIZE; i++)
    {
        pairing.secret[i] = secret[i];
    }

    ESP_LOGI(TAG, "pairing secret: %s", pairing.secret);

    xQueueSend(aws_mqtt_pairing_queue, &pairing, pdMS_TO_TICKS(50));

    return ESP_OK;
}
/**
  * @brief Update the user email 
  * 
  * @param email user email string
  */
void app_state_update_user_email(char *email)
{
    strncpy(user_email, email, USER_EMAIL_MAX_SIZE - 1);
}

/**
 * @brief copy user email to destination buffer
 *  
 * @param dest destination buffer
 * @param dest_size max size to be written to
 */
void app_state_get_user_email(char *dest, int dest_size)
{
    int max_size;
    if (dest_size > USER_EMAIL_MAX_SIZE)
    {
        max_size = USER_EMAIL_MAX_SIZE;
    }
    strncpy(dest, user_email, max_size - 1);
}