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

using namespace std;

#define COFFE_PIN GPIO_NUM_13
#define NTC_PIN GPIO_NUM_34
// quando tava em c, podia definir o pino so falando o numero inteiro agora tem que usar GPIO_NUM e falar o numero do pino


float temperature = 0;


//constantes de conexao da rede wifi, pretendo passar para um arquivo depois


void vTimerCallback(TimerHandle_t xTimer);
void temperature_task(void *pvParameter);

void wifi_task(void *pvParameter);

TimerHandle_t xTimer;
BaseType_t xTask;
BaseType_t xWifi;
void setup_pins()
{
    //toda configuracao de pinagem vem aqui
    gpio_pad_select_gpio(COFFE_PIN);
    gpio_set_direction(COFFE_PIN, GPIO_MODE_OUTPUT);
}

void create_tasks()
{
    xTimer = xTimerCreate("LEDTimer", pdMS_TO_TICKS(30), pdTRUE, (void *)0, vTimerCallback);
    xTask = xTaskCreate(&temperature_task, "temperature_task", 2048, NULL, 5, NULL);
    xWifi = xTaskCreate(&wifi_task, "wifi_task", 2048, NULL, 5, NULL);
}

void validate_tasks()
{
    if (xTimer == NULL) 
    {
        ESP_LOGE("validate_tasks", "Erro ao criar o timer");
        xTimer = xTimerCreate("LEDTimer", pdMS_TO_TICKS(30), pdTRUE, (void *)0, vTimerCallback);
    }

    if (xTask == NULL) 
    {
        ESP_LOGE("validate_tasks", "Erro ao criar o task temperatura");
        xTask = xTaskCreate(&temperature_task, "temperature_task", 2048, NULL, 5, NULL);
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

extern "C" void app_main()  
{
    esp_error_handler();

    Wifi::Wifi("Esp32", "12345678");
    
    Wifi::Wifi_init_sta();

    setup_pins();

    create_tasks();

    while (true) 
    {
        validate_tasks();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void wifi_task(void *pvParameter)
{
    while(true)
    {
        Wifi::Check_wifi_status();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTimerCallback(TimerHandle_t xTimer) {
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(COFFE_PIN, led_state);
    
    
}


void temperature_task(void *pvParameter)
{
    //ESTA SIMULANDO SENSOR DE TEMPERATURA
    temperature = 15;
    srand(time(0));
    while(true)
    {
        temperature += (rand() % 5) + 1;
        //ESP_LOGI("Temperatura :", " %f", temperature);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
