#pragma region Imports


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_continuous.h>
#include <driver/adc.h>
#include <stdio.h>
#include <time.h>
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include <string.h>
#include "iostream"
#include "wifi.cpp"
#include "ds18b20.h"
#include <onewire_bus_impl_rmt.h>

using namespace std;


#pragma endregion Imports


#pragma region Variables_Declaration


#define COFFE_PIN GPIO_NUM_13

#define EXAMPLE_ONEWIRE_BUS_GPIO    14
#define EXAMPLE_ONEWIRE_MAX_DS18B20 1
// quando tava em c, podia definir o pino so falando o numero inteiro agora tem que usar GPIO_NUM e falar o numero do pino

TimerHandle_t xSubscriber;
BaseType_t xTemperature;
BaseType_t xWifi;

float temperature = 0;


#pragma endregion Variables_Declaration


#pragma region Functions_Declaration


void subscriber_callback(TimerHandle_t xTimer);
void temperature_task(void *pvParameter);
void wifi_task(void *pvParameter);


#pragma endregion Functions_Declaration


#pragma region Config_Procedures


void setup_pins()
{
    //toda configuracao de pinagem vem aqui
    gpio_pad_select_gpio(COFFE_PIN);
    gpio_set_direction(COFFE_PIN, GPIO_MODE_OUTPUT);

}

void create_tasks()
{
    xSubscriber = xTimerCreate("Subscriber", pdMS_TO_TICKS(30), pdTRUE, (void *)0, subscriber_callback);
    xTemperature = xTaskCreate(&temperature_task, "temperature_task", 4*2048, NULL, 5, NULL);
    xWifi = xTaskCreate(&wifi_task, "wifi_task", 2048, NULL, 5, NULL);
}

void validate_tasks()
{
    if (xSubscriber == NULL) 
    {
        ESP_LOGE("validate_tasks", "Erro ao criar o subscriber");
        xSubscriber = xTimerCreate("Subscriber", pdMS_TO_TICKS(30), pdTRUE, (void *)0, subscriber_callback);
    }

    if (xTemperature == NULL) 
    {
        ESP_LOGE("validate_tasks", "Erro ao criar o task temperatura");
        xTemperature = xTaskCreate(&temperature_task, "temperature_task", 2048, NULL, 5, NULL);
    }

    if (xWifi == NULL) 
    {
        ESP_LOGE("validate_tasks", "Erro ao criar o task wifi");
        xWifi = xTaskCreate(&wifi_task, "wifi_task", 4*2048, NULL, 5, NULL);
    }

}

void esp_error_handler()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void wifi_setup()
{
    char* ssid = "Esp32";
    char* pass = "12345678";

    Wifi(ssid, pass); 
    //podemos depois passar para um arquivo o ssid e password mas por enquanto está mockado

    Wifi::Wifi_init_sta();
}

int ds18b20_device_num = 0;
ds18b20_device_handle_t ds18b20s[EXAMPLE_ONEWIRE_MAX_DS18B20];
void temp_setup()
{
    onewire_bus_handle_t bus = NULL;
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = EXAMPLE_ONEWIRE_BUS_GPIO,
    };
    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 12, // 1byte ROM command + 8byte ROM number + 1byte device command
    };
    ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus));


    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t next_onewire_device;
    esp_err_t search_result = ESP_OK;

    // create 1-wire device iterator, which is used for device search
    ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));
    ESP_LOGI("Setup Temperature", "Device iterator created, start searching...");
    do {
        search_result = onewire_device_iter_get_next(iter, &next_onewire_device);
        if (search_result == ESP_OK) { // found a new device, let's check if we can upgrade it to a DS18B20
            ds18b20_config_t ds_cfg = {};
            // check if the device is a DS18B20, if so, return the ds18b20 handle
            if (ds18b20_new_device(&next_onewire_device, &ds_cfg, &ds18b20s[ds18b20_device_num]) == ESP_OK) {
                ESP_LOGI("Setup Temperature", "Found a DS18B20[%d], address: %016llX", ds18b20_device_num, next_onewire_device.address);
                ds18b20_device_num++;
            } else {
                ESP_LOGI("Setup Temperature", "Found an unknown device, address: %016llX", next_onewire_device.address);
            }
        }
    } while (search_result != ESP_ERR_NOT_FOUND);
    ESP_ERROR_CHECK(onewire_del_device_iter(iter));
    ESP_LOGI("Setup Temperature", "Searching done, %d DS18B20 device(s) found", ds18b20_device_num);
}

#pragma endregion Config_Procedures


#pragma region App_main


extern "C" void app_main()  
{
    
    esp_error_handler();

    wifi_setup();

    temp_setup();

    setup_pins();

    create_tasks();

    while (true) 
    {
        validate_tasks();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


#pragma endregion App_main


#pragma region Tasks


void wifi_task(void *pvParameter)
{
    while(true)
    {
        Wifi::Check_wifi_status();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void subscriber_callback(TimerHandle_t xTimer) {
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(COFFE_PIN, led_state);
    
    
}


void temperature_task(void *pvParameter)
{
    for (int i = 0; i < ds18b20_device_num; i ++) {
        ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(ds18b20s[i]));
        ESP_ERROR_CHECK(ds18b20_get_temperature(ds18b20s[i], &temperature));
        ESP_LOGI("Temp Task", "temperature read from DS18B20[%d]: %.2fC", i, temperature);
    }
}


#pragma endregion Tasks