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
#include "temperature.cpp"

using namespace std;


#pragma endregion Imports


#pragma region Variables_Declaration


#define COFFE_PIN GPIO_NUM_13

#define TEMP_PIN    14

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
    xTemperature = xTaskCreate(&temperature_task, "temperature_task", 2048, NULL, 5, NULL);
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
        xWifi = xTaskCreate(&wifi_task, "wifi_task", 2048, NULL, 5, NULL);
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

void temp_setup()
{
    Temperature(TEMP_PIN);
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
    while (true)
    {
        if(Temperature::found)
        {
            temperature = Temperature::GetTemperature();
            ESP_LOGI("temperature_task", "Temperatura : %f", temperature);
        }
        else
        {
           temp_setup(); 
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


#pragma endregion Tasks