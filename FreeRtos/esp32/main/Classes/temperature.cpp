#include "../Interfaces/itemperature.h"

#ifndef TEMPERATURE_H
#define TEMPERATURE_H
class Temperature : public iTemperature
{
    public:
    Temperature(int pin)
    {
        _pin =pin;
        Temp_setup();
    }
    void static Temp_setup()
    {
        onewire_bus_handle_t bus = NULL;
        onewire_bus_config_t bus_config = {
            .bus_gpio_num = _pin,
        };
        onewire_bus_rmt_config_t rmt_config = {
            .max_rx_bytes = 12,
        };

        onewire_device_iter_handle_t iter = NULL;

         

        if(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus) == ESP_OK)
        {

            if(onewire_new_device_iter(bus, &iter) == ESP_OK)
            {
                onewire_device_t next_onewire_device;
                esp_err_t search_result;

                ESP_LOGI("Setup Temperature", "Device iterator created, start searching...");
                do {
                    search_result = onewire_device_iter_get_next(iter, &next_onewire_device);
                    if (search_result == ESP_OK) 
                    {
                        ds18b20_config_t ds_cfg = {};
                        if (ds18b20_new_device(&next_onewire_device, &ds_cfg, &ds18b20s) == ESP_OK) {
                            ESP_LOGI("Setup Temperature", "Device found");
                            found = true;
                            return;
                        }
                    }
                    else
                    {
                        onewire_del_device_iter(iter);
                    }
                } while (search_result != ESP_ERR_NOT_FOUND );
            }
            else
            {
                onewire_del_device_iter(iter);
            }
        }
        ESP_LOGI("Setup Temperature", "Device not found");
        found = false;
        return;
    }

    static float GetTemperature()
    {

        float temperature;
        ds18b20_trigger_temperature_conversion(ds18b20s);
        ds18b20_get_temperature(ds18b20s, &temperature);
        return temperature;
    }
};

#endif // TEMPERATURE_H

