#ifndef __app_nvs_h__
#define __app_nvs_h__

#define MAX_SIZE_CREDS 64

typedef struct wifi_creds {
    bool valid;
    char ssid[MAX_SIZE_CREDS];
    char pass[MAX_SIZE_CREDS];
} wifi_creds_t;

bool app_nvs_init(void);
bool app_nvs_validate_wifi_creds(wifi_creds_t* creds);
esp_err_t app_nvs_get_wifi_creds(wifi_creds_t* creds);
esp_err_t app_nvs_set_wifi_creds(wifi_creds_t* creds);
void app_nvs_test(void);


#endif