#include "esp_wifi.h"
#include <string.h>
#include "esp_log.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>

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

        static bool ping_google() {
            const char* hostname = "www.google.com";
            struct sockaddr_in server_addr;
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                ESP_LOGE("ping", "Failed to create socket");
                return false;
            }

            struct hostent *server = gethostbyname(hostname);
            if (server == NULL) {
                ESP_LOGE("ping", "DNS lookup failed for %s", hostname);
                close(sock);
                return false;
            }

            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
            server_addr.sin_port = htons(80); // Porta HTTP

            if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                ESP_LOGE("ping", "Connection to %s failed", hostname);
                close(sock);
                return false;
            }

            ESP_LOGI("ping", "Successfully connected to %s", hostname);
            close(sock);
            return true;
        }
        static bool Check_wifi_status() 
        {
            //verifica se o wifi está conectado, caso não esteja tenta se reconectar
            wifi_ap_record_t ap_info;
            esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
            if (ret == ESP_OK) {
                ESP_LOGI("wifi", "Connected to SSID: %s", ap_info.ssid);
                ping_google();
                return true;
            } else {
                ESP_LOGE("wifi", "Not connected to Wi-Fi");
                esp_wifi_start();
                esp_wifi_connect();
                esp_netif_init();
                return false;
            }
        }

        static void Wifi_init_sta() 
        {
            //configura a rede wifi para depois se conectar
            ESP_ERROR_CHECK(esp_event_loop_create_default());
            esp_netif_init();
            esp_netif_create_default_wifi_sta();
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