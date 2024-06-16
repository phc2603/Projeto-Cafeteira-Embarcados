#pragma region Imports


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "Classes/wifi.cpp"
#include "Classes/temperature.cpp"
#include "Classes/rabbitmqMqtt.cpp"
#include "Enum/coffeTimes.h"


#pragma endregion Imports


#pragma region Variables_Declaration


#define COFFE_PIN GPIO_NUM_13

#define TEMP_PIN    14
// quando tava em c, podia definir o pino so falando o numero inteiro agora tem que usar GPIO_NUM e falar o numero do pino

//tasks
BaseType_t xSubscriber;
BaseType_t xTemperature;
BaseType_t xWifi;

//temperature
float temperature = 0;
const float idealTemperature = 27;

//wifi
char* ssid = "Lemescosonet";
char* pass = "nettiigbn";

//rabbitmq
RabbitmqMqtt rabbitmq;
char* rabbitUri = "mqtt://192.168.0.15:1883";

#pragma endregion Variables_Declaration


#pragma region Functions_Declaration


void subscriber_callback(void *pvParameter);
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
    xSubscriber = xTaskCreate(&subscriber_callback, "subscriber_task", 2*2048, NULL, 5, NULL);
    xTemperature = xTaskCreate(&temperature_task, "temperature_task", 2*2048, NULL, 5, NULL);
    xWifi = xTaskCreate(&wifi_task, "wifi_task", 2*2048, NULL, 5, NULL);
}

void validate_tasks()
{
    if (xSubscriber == NULL) 
    {
        ESP_LOGE("validate_tasks", "Erro ao criar o subscriber");
        xSubscriber = xTaskCreate(&subscriber_callback, "subscriber_task", 2*2048, NULL, 5, NULL);
    }

    if (xTemperature == NULL) 
    {
        ESP_LOGE("validate_tasks", "Erro ao criar o task temperatura");
        xTemperature = xTaskCreate(&temperature_task, "temperature_task", 2*2048, NULL, 5, NULL);
    }

    if (xWifi == NULL) 
    {
        ESP_LOGE("validate_tasks", "Erro ao criar o task wifi");
        xWifi = xTaskCreate(&wifi_task, "wifi_task", 2*2048, NULL, 5, NULL);
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
    //ESP_ERROR_CHECK(esp_event_loop_create_default());
}

void wifi_setup()
{

    Wifi(ssid, pass); 
    //podemos depois passar para um arquivo o ssid e password mas por enquanto está mockado

    Wifi::Wifi_init_sta();
    while(!Wifi::Check_wifi_status()){ vTaskDelay(pdMS_TO_TICKS(1000));}
}

void temp_setup()
{
    Temperature(TEMP_PIN);
}

void rabbitmqInitialize()
{
    while(!Wifi::Check_wifi_status()){ vTaskDelay(pdMS_TO_TICKS(1000));}
    while(!rabbitmq.initialized)
    {
        rabbitmq.rabbitmqInitialize(rabbitUri);
        if(!rabbitmq.initialized)
            vTaskDelay(pdMS_TO_TICKS(1000));
    }
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

    rabbitmqInitialize();
    
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

void subscriber_callback(void *pvParameter) {

    while(true)
    {
        if(!rabbitmq.commandQueue.empty())
        {
            gpio_set_level(COFFE_PIN, true);
            while(temperature < idealTemperature)
            {
                ESP_LOGW("COFFE", "Esperando temperatura ideal");
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            switch (rabbitmq.commandQueue.front())
            {
                case 'L':
                    ESP_LOGW("COFFE", "Começando cafe grande");
                     vTaskDelay(pdMS_TO_TICKS(CoffeTimes(Large)));
                    break;
                case 'M':
                    ESP_LOGW("COFFE", "Começando cafe medio");
                    vTaskDelay(pdMS_TO_TICKS(CoffeTimes(Medium)));
                    break;
                case 'S':
                    ESP_LOGW("COFFE", "Começando cafe pequeno");
                    vTaskDelay(pdMS_TO_TICKS(CoffeTimes(Small)));
                    break;
            }
            gpio_set_level(COFFE_PIN, false);
            ESP_LOGW("COFFE", "Finalizando o cafe");
            rabbitmq.commandQueue.pop();
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
        else
        {
           vTaskDelay(pdMS_TO_TICKS(100)); 
        }
    }
    
    
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
            ESP_LOGI("temperature_task", "Temperatura nao configurado");
            temp_setup(); 
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


#pragma endregion Tasks