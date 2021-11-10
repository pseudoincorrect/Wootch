#ifndef __awq_mqtt_types_h__
#define __awq_mqtt_types_h__

#include "esp_err.h"

#define PAIRING_SECRET_SIZE 7 // including null terminator

typedef struct activity_msg_struct
{
    int watchLvl;
    int maxAcc;
    int accThresh;
    int maxRot;
    int rotThresh;
} activity_msg_t;

typedef struct pairing_msg_struct
{
    char secret[PAIRING_SECRET_SIZE];
} pairing_msg_t;

typedef struct notification_msg_struct
{
    char text[128];
} notification_msg_t;

esp_err_t activity_msg_to_json_string(activity_msg_t *msg, char *buff, int buff_size);
esp_err_t pairing_msg_to_json_string(pairing_msg_t *msg, char *buff, int buff_size);

#endif