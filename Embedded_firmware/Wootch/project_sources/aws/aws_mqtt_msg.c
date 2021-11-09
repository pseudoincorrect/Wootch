#include "aws_mqtt_msg.h"
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "MQTT_MSG";

/**
 * @brief convert an activity msg struct into a json string
 * 
 * @param msg activity msg struct
 * @param buff char buffer
 * @param buff_size size of the buffer
 * @return error code
 */
esp_err_t activity_msg_to_json_string(activity_msg_t *msg, char *buff, int buff_size)
{
    esp_err_t err;
    err = snprintf(buff, buff_size,
                   "{ \"watchLvl\": \"%d\", \"maxAcc\": \"%d\", \"accThresh\": \"%d\", \"maxRot\": \"%d\", \"rotThresh\": \"%d\"}",
                   msg->watchLvl, msg->maxAcc, msg->accThresh, msg->maxRot, msg->rotThresh);
    if (err < 0 || err >= buff_size)
    {
        ESP_LOGE(TAG, "format activity json msg, %d", err);
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
 * @brief convert a pairing msg struct into a json string
 * 
 * @param msg pairing msg struct
 * @param buff char buffer
 * @param buff_size size of the buffer
 * @return error code
 */
esp_err_t pairing_msg_to_json_string(pairing_msg_t *msg, char *buff, int buff_size)
{
    esp_err_t err;
    char *b = msg->secret;
    err = snprintf(buff, buff_size,
                   "{ \"secret\": \"%c%c%c%c%c%c\"}",
                   b[0], b[1], b[2], b[3], b[4], b[5]);
    if (err < 0 || err >= buff_size)
    {
        ESP_LOGE(TAG, "format pairing json msg, %d", err);
        return ESP_FAIL;
    }
    return ESP_OK;
}