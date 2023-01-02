#ifndef _GPRS_H_
#define _GPRS_H_

#define TINY_GSM_MODEM_SIM800
#define modemSerial Serial2

#include <Arduino.h>
#include <TinyGsmClient.h>
#include "ThingsBoard.h"

#define THINGSBOARD_BUFFER_SIZE 512

#define APN_TM "TM"
#define APN_TRUPHONE "iot.truphone.com"

#define GPRS_USER ""
#define GPRS_PASS ""

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD 115200
#define MODEM_BAUD 115200
#define RX_BUFFER_LENGTH 1024
#define GPRS_TIMEOUT 30000 // in millisecs

#define standByGPRSPostPeriod 3600
#define actuatingGPRSPostPeriod 60
#define phototherapyGPRSPostPeriod 180
#define GPRS_SHUT OFF

#define SIMCOM800_AT "AT\n"
#define SIMCOM800_AT_CFUN "AT+CFUN=1\n"

#define AT_OK "OK"
#define AT_ERROR "ERROR"

struct GPRSstruct
{
  int provisioned = false;
  bool OTA_requested = false;
  bool provision_request_sent = false;
  bool provision_request_processed = false;
  bool lastGPRSConnectionStatus = false;
  bool serverConnectionStatus = false;
  bool lastServerConnectionStatus = false;
  bool enable;
  long sendPeriod;
  long lastSent;
  char buffer[RX_BUFFER_LENGTH];
  int charToRead;
  int bufferWritePos;
  bool powerUp;
  bool connect;
  bool connectionStatus;
  bool timeOut;
  byte process;
  long processTime;
  long packetSentenceTime;
  bool post;
  bool firstPost = false;
  bool firstConfigPost;

  String CCID;
  String IMEI;
  String IMSI;
  String COP;
  int CSQ;
  String APN;
  IPAddress IP;
  String device_token;

  float longitud;
  float latitud;
  float accuracy;
};

// Struct for client connecting after provisioning
struct Credentials
{
  String client_id;
  String username;
  String password;
};

bool GPRSCheckNewEvent();
bool GPRSAttached();
bool GPRSConnectedToServer();
void GPRS_Handler();
void GPRS_TB_Init();
void initGPRS();
void GPRSSetPostPeriod();

#endif // _GPRS_123H_