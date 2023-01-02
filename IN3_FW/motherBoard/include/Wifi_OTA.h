
#ifndef _WIFI_OTA_H_
#define _WIFI_OTA_H_

#include <Arduino.h>
#include "main.h"
#include "ThingsBoard.h"
#include "Credentials.h"

#define THINGSBOARD_BUFFER_SIZE 512

struct WIFIstruct
{
    int provisioned = false;
    bool OTA_requested = false;
    bool provision_request_sent = false;
    bool provision_request_processed = false;
    bool serverConnectionStatus = false;
    bool lastServerConnectionStatus = false;
    bool lastWIFIConnectionStatus = false;
    String device_token;
};

bool WIFIConnectedToServer();
bool WIFIAttached();
bool WIFICheckNewEvent();

#endif // _WIFI_OTA_H_
