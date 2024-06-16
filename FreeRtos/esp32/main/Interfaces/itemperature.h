#include <ds18b20.h>
#include <onewire_bus_impl_rmt.h>
#include <esp_log.h>

class iTemperature
{
    protected:
        static int _pin;
        static ds18b20_device_handle_t ds18b20s;
    public:
        static bool found;
};

bool iTemperature::found = false;
int iTemperature::_pin;
ds18b20_device_handle_t iTemperature::ds18b20s;