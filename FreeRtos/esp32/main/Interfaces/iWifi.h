#include "esp_wifi.h"
#include <string.h>
#include "esp_log.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
class iWifi
{
    protected:
        static const char* _ssid;
        static const char* _password;

    public:
};

const char* iWifi::_ssid = nullptr;
const char* iWifi::_password = nullptr;