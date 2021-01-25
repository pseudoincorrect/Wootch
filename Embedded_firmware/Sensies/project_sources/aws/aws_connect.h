#ifndef __aws_connect_h__
#define __aws_connect_h__

#include "aws_iot_mqtt_client_interface.h"

void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName,
                                    uint16_t topicNameLen,
                                    IoT_Publish_Message_Params *params, void *pData);

void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data);

// void aws_iot_task(void *param); 
void aws_iot_mqtt_manage_task(void *param);
void aws_iot_publish_1_task(void *param);
void aws_iot_publish_2_task(void *param);


#endif
