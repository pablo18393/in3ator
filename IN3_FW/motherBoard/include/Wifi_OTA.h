#include <Arduino.h>
#include "main.h"
#include "ThingsBoard.h"
#include "Credentials.h"

#define THINGSBOARD_BUFFER_SIZE 512 

WiFiClient espClient;

// Initialize ThingsBoard instance
ThingsBoardSized<THINGSBOARD_BUFFER_SIZE> tb_wifi(espClient);

struct WIFIstruct
{
    int provisioned = false;
    bool OTA_requested = false;
    bool provision_request_sent = false;
    bool provision_request_processed = false;
    String device_token;
};