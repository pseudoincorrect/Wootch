#include <ctype.h>
#include <string.h>

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "app_nvs.h"

#define APP_NVS_STORAGE_NAMESPACE "app_nvs_storage"
#define APP_NVS_CREDS_KEY "wificreds"


static const char *TAG = "APP_NVS";

bool app_nvs_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition needs to be erased, retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    return true;
}

bool app_nvs_validate_wifi_creds(wifi_creds_t* creds)
{
    // check validity
    if(!creds->valid) return false;
    // Check if every char are alfa num
    for (int i = 0; i < MAX_SIZE_CREDS - 1; i++)
    {
        char s = creds->ssid[i];
        char p = creds->pass[i];
        if ((!isalnum(s) && s != 0) || (!isalnum(p) && p != 0))
        {
            return false;
        }
    }
    // Check for null terminator
    if (creds->ssid[MAX_SIZE_CREDS - 1] != 0
            || creds->ssid[MAX_SIZE_CREDS - 1] != 0)
    {
        return false;
    }
    return true;
}

esp_err_t app_nvs_get_wifi_creds(wifi_creds_t* creds)
{
    nvs_handle_t handle;
    esp_err_t err;
    // open non volatile storage
    err = nvs_open(APP_NVS_STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;
    // Find the size to read to avoid unsafe behavior on the second get_blob
    size_t read_size = 0;
    err = nvs_get_blob(handle, APP_NVS_CREDS_KEY, NULL, &read_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    // Read previously saved blob if available
    if (read_size == sizeof(wifi_creds_t))
    {
        err = nvs_get_blob(handle, APP_NVS_CREDS_KEY, creds, &read_size);
    }
    else return ESP_ERR_INVALID_SIZE;
    if (err != ESP_OK ) return err;
    // close non volatile storage
    nvs_close(handle);
    return ESP_OK;
}


esp_err_t app_nvs_set_wifi_creds(wifi_creds_t* creds)
{
    nvs_handle handle;
    esp_err_t err;
    // Validate wifi credentials
    if (! app_nvs_validate_wifi_creds(creds))
    {
        return ESP_ERR_INVALID_ARG;
    }
    // Open non volatile storage
    err = nvs_open(APP_NVS_STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;
    // Store wifi creds
    err = nvs_set_blob(handle, APP_NVS_CREDS_KEY, creds,
                       sizeof(wifi_creds_t));
    if (err != ESP_OK) return err;
    // Commit/process the changes to the NVS
    err = nvs_commit(handle);
    if (err != ESP_OK) return err;
    return ESP_OK;
}



void app_nvs_test(void)
{
    esp_err_t err;
    wifi_creds_t creds = {};
    //enable nvs module
    app_nvs_init();
    // Get the credentials from NVS
    err = app_nvs_get_wifi_creds(&creds);
    if (err != ESP_OK) ESP_LOGI(TAG, "app_nvs_get_wifi_creds error %x", err);
    // // If they are invalid, fill them with valid data
    if (!creds.valid)
    {
        strcpy(creds.ssid, "Maxi");
        strcpy(creds.pass, "notmyrealpassword");
        creds.valid = true;
    }
    // Set the credentials in the NVS
    err = app_nvs_set_wifi_creds(&creds);
    if (err != ESP_OK)  ESP_LOGI(TAG, "app_nvs_set_wifi_creds error %x", err);

    err = app_nvs_get_wifi_creds(&creds);
    if (err != ESP_OK) ESP_LOGI(TAG, "app_nvs_get_wifi_creds error %x", err);
}