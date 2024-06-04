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

#define COFFE_PIN 13
#define NTC_PIN 34
bool started = false;

void vTimerCallback(TimerHandle_t xTimer);
void adc_task(void *pvParameter);
TimerHandle_t xTimer;
BaseType_t xTask;
int counter = 0;
void app_main() {
    gpio_pad_select_gpio(COFFE_PIN);
    gpio_set_direction(COFFE_PIN, GPIO_MODE_OUTPUT);


    xTimer = xTimerCreate("LEDTimer", pdMS_TO_TICKS(30), pdTRUE, (void *)0, vTimerCallback);
    xTask = xTaskCreate(&adc_task, "adc_task", 2048, NULL, 5, NULL);

    if (xTimer == NULL) {
        ESP_LOGE("app_main", "Erro ao criar o timer");
        return;
    }

    if (xTimerStart(xTimer, 0) != pdPASS) {
        ESP_LOGE("app_main", "Erro ao iniciar o timer");
        return;
    }

}

void vTimerCallback(TimerHandle_t xTimer) {
    static int led_state = 0;

    counter++;
    led_state = !led_state;
    gpio_set_level(COFFE_PIN, led_state);


    ESP_LOGI("vTimerCallback", "Tarefa de callback do timer executada");
    ESP_LOGI("Counter", "Contador == %d",counter );

    if(counter == 99) 
    {
        ESP_LOGI("vTaskDelete", "Tarefa de callback do timer deletada");

        xTimerDelete(xTimer,pdMS_TO_TICKS(30000));
    }

    if(started == false)
    {
        started = true;
        xTimerChangePeriod(xTimer,pdMS_TO_TICKS(120000),pdMS_TO_TICKS(30000));
        ESP_LOGI("Started", "Startamos");
    }
    else
    {
        xTimerChangePeriod(xTimer,pdMS_TO_TICKS(60000),pdMS_TO_TICKS(30000));
    }
    
}


void adc_task(void *pvParameter)
{
    
    adc1_config_width(ADC_WIDTH_BIT_12);
    
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    
    while(true)
    {
        float adc_reading = adc1_get_raw(ADC1_CHANNEL_6);

        ESP_LOGI("Temperatura :", " %f", adc_reading);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
