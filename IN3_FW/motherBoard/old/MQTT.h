
#define TINY_GSM_MODEM_SIM800
#include "main.h"
#include <TinyGsmClient.h>
#include "ThingsBoard.h"
#include <StreamDebugger.h>

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[] = "iot.truphone.com";
const char apn2[] = "TM";
const char user[] = "";
const char pass[] = "";

void MQTT_Handler();
void MQTT_init();
