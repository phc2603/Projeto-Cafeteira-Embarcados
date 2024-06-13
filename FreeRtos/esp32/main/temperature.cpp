#include "ds18b20.h"
#include <onewire_bus_impl_rmt.h>
#include <esp_log.h>

#ifndef TEMPERATURE_H
#define TEMPERATURE_H
class Temperature
{
private:
    static int _pin;
    static ds18b20_device_handle_t ds18b20s;
public:
    static bool found;
    Temperature(int pin)
    {
        _pin =pin;
        Temp_setup();
    }
    static void Temp_setup()
    {
        onewire_bus_handle_t bus = NULL;
        onewire_bus_config_t bus_config = {
            .bus_gpio_num = _pin,
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
                if (ds18b20_new_device(&next_onewire_device, &ds_cfg, &ds18b20s) == ESP_OK) {
                    ESP_LOGI("Setup Temperature", "Device found");
                    found = true;
                    return;
                }
            }
        } while (search_result != ESP_ERR_NOT_FOUND);
        ESP_ERROR_CHECK(onewire_del_device_iter(iter));
        ESP_LOGI("Setup Temperature", "Device not found");
        found = false;
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

bool Temperature::found = false;
int Temperature::_pin;
ds18b20_device_handle_t Temperature::ds18b20s;