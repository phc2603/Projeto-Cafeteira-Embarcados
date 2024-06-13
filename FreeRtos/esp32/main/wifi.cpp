#include "esp_wifi.h"
#include <string.h>
#include "esp_log.h"

#ifndef WIFI_H
#define WIFI_H


class Wifi
{
    private:
        static const char* _ssid;
        static const char* _password;

    public:

        Wifi(char* SSID, char* PASSWORD)
        {
            _ssid = SSID;
            _password =  PASSWORD;
        }
        static void Check_wifi_status() 
        {
            //verifica se o wifi está conectado, caso não esteja tenta se reconectar
            wifi_ap_record_t ap_info;
            esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
            if (ret == ESP_OK) {
                ESP_LOGI("wifi", "Connected to SSID: %s", ap_info.ssid);
            } else {
                ESP_LOGE("wifi", "Not connected to Wi-Fi");
                esp_wifi_start();
                esp_wifi_connect();
            }
        }

        static void Wifi_init_sta() 
        {
            //configura a rede wifi para depois se conectar
            esp_netif_init();
            ESP_ERROR_CHECK(esp_event_loop_create_default());
            wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&config));
            wifi_config_t wifi_config = {
                .sta = {
                    .ssid = "",
                    .password = "",
                    .scan_method = WIFI_FAST_SCAN,
                    .he_trig_mu_bmforming_partial_feedback_disabled = false,
                },
            };
            memcpy(wifi_config.sta.ssid, _ssid, strlen(_ssid) + 1);
            memcpy(wifi_config.sta.password, _password, strlen(_password) + 1); 

            esp_wifi_set_mode(WIFI_MODE_STA);
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
            esp_wifi_start();
            esp_wifi_connect();

        }
};

#endif // WIFI_H

const char* Wifi::_ssid = nullptr;
const char* Wifi::_password = nullptr;