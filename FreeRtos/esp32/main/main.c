#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
#define LED_PIN 13

// Protótipo da função que será chamada pelo timer
void vTimerCallback(TimerHandle_t xTimer);

void app_main() {
    // Configuração do pino do LED
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Inicialização do timer
    TimerHandle_t xTimer = xTimerCreate("LEDTimer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, vTimerCallback);

    // Verifica se o timer foi criado corretamente
    if (xTimer == NULL) {
        ESP_LOGE("app_main", "Erro ao criar o timer");
        return;
    }

    // Inicialização do timer
    if (xTimerStart(xTimer, 0) != pdPASS) {
        ESP_LOGE("app_main", "Erro ao iniciar o timer");
        return;
    }

    // Loop principal vazio
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTimerCallback(TimerHandle_t xTimer) {
    static int led_state = 0;
    
    // Inverte o estado do LED
    led_state = !led_state;
    gpio_set_level(LED_PIN, led_state);

    // Log para verificar se a tarefa está sendo chamada
    ESP_LOGI("vTimerCallback", "Tarefa de callback do timer executada");
}
