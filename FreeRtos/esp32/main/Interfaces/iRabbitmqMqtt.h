#include "mqtt_client.h"
#include <esp_log.h>
#include "protocol_examples_common.h"
#include <queue>

using namespace std;

class iRabbitmqMqtt
{
    
    protected:
        static esp_mqtt_client_handle_t client;
        static esp_mqtt_client_config_t mqtt_cfg;

    public:
        static bool initialized;
        static queue<char> commandQueue;
    
};

bool iRabbitmqMqtt::initialized = false;
queue<char> iRabbitmqMqtt::commandQueue;
esp_mqtt_client_handle_t iRabbitmqMqtt::client = {};
esp_mqtt_client_config_t iRabbitmqMqtt::mqtt_cfg = {};