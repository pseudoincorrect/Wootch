#ifndef __awq_mqtt_types_h__
#define __awq_mqtt_types_h__

typedef struct activity_msg_struct
{
    int watchLvl;
    int maxAcc;
    int accThresh;
} activity_msg_t;

typedef struct pairing_msg_struct
{
    char buff[6];
} pairing_msg_t;

typedef struct notification_msg_struct
{
    char buff[128];
} notification_msg_t;

#endif