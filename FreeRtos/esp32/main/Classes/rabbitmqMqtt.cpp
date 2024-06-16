#include "../Interfaces/iRabbitmqMqtt.h"

#ifndef RABBIT_H
#define RABBIT_H

class RabbitmqMqtt : public iRabbitmqMqtt
{
    
    public:
    static void subscribe()
    {
        esp_mqtt_client_subscribe(client, "coffe" , 0);
    }

    static void unsubscribe()
    {
        esp_mqtt_client_unsubscribe(client, "coffe");
    }
    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
    {
        ESP_LOGD("mqtt_example", "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
        esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
        switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI("mqtt_example", "Connected");
            subscribe();
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI("mqtt_example", "Disconnected");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI("mqtt_example", "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI("mqtt_example", "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI("mqtt_example", "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI("mqtt_example", "MQTT_EVENT_DATA");
            ESP_LOGI("mqtt_example","TOPIC=%.*s\r\n", event->topic_len, event->topic);
            ESP_LOGI("mqtt_example","DATA=%.*s\r\n", event->data_len, event->data);
            commandQueue.push(event->data[0]);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI("mqtt_example", "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                ESP_LOGI("TAG", "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI("mqtt_example", "Other event id:%d", event->event_id);
            break;
        }
    }

    static void rabbitmqInitialize(char* uri)
    {
        //uri:"mqtt://192.168.0.15:1883"
        memset(&mqtt_cfg, 0, sizeof(mqtt_cfg));
        initialized = false;
        ESP_LOGI("mqtt_example", "1");
        mqtt_cfg.broker.address.uri = uri;
        mqtt_cfg.session.protocol_ver = MQTT_PROTOCOL_V_3_1_1;
        mqtt_cfg.credentials.username = "guest";
        mqtt_cfg.credentials.authentication.password="guest";
        mqtt_cfg.credentials.client_id = "coffe";

        ESP_LOGI("mqtt_example", "2");
        client = esp_mqtt_client_init(&mqtt_cfg);
        if (client == NULL) {
            ESP_LOGE("mqtt_example", "Failed to initialize MQTT client");
            return;
        }
        ESP_LOGI("mqtt_example", "3");
        esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, client);
        ESP_LOGI("mqtt_example", "4");
        esp_mqtt_client_start(client);
        ESP_LOGI("mqtt_example", "5");
        initialized = true;
    }

    RabbitmqMqtt()
    {

    }

};

#endif
