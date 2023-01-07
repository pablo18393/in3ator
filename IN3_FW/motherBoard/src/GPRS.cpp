/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include <Arduino.h>
#include "Credentials.h"
#include "main.h"
#include "GPRS.h"

// Initialize GSM modem
TinyGsm modem(modemSerial);

// Initialize GSM client
TinyGsmClient client(modem);

// Initialize ThingsBoard instance
ThingsBoardSized<THINGSBOARD_BUFFER_SIZE, THINGSBOARD_FIELDS_AMOUNT> tb(client);

// Initialize ThingsBoard client provision instance
ThingsBoardSized<THINGSBOARD_BUFFER_SIZE> tb_provision(client); // increase buffer size

StaticJsonDocument<JSON_OBJECT_SIZE(THINGSBOARD_FIELDS_AMOUNT)> GPRS_JSON;
JsonObject addVariableToTelemetryGPRSJSON = GPRS_JSON.to<JsonObject>();

unsigned long previous_processing_time;

extern in3ator_parameters in3;
GPRSstruct GPRS;
Credentials credentials;

void clearGPRSBuffer()
{
  memset(GPRS.buffer, 0, sizeof(GPRS.buffer));
  GPRS.charToRead = false;
  GPRS.bufferWritePos = false;
}

int checkSerial(const char *success, const char *error)
{
  if (strstr(GPRS.buffer, success))
  {
    GPRS.process++;
    clearGPRSBuffer();
    return true;
  }
  if (strstr(GPRS.buffer, success))
  {
    logln("[GPRS] -> GPRS error: " + String(error));
    clearGPRSBuffer();
    return -1;
  }
  return false;
}

void initGPRS()
{
  Serial2.begin(MODEM_BAUD);
  GPRS.powerUp = true;
  GPIOWrite(GPRS_PWRKEY, HIGH);
}

bool GPRSCheckNewEvent()
{
  bool retVal = false;
  bool isGPRSConnected = GPRS.post;
  bool serverConnectionStatus = false;
  if (isGPRSConnected)
  {
    serverConnectionStatus = GPRS.serverConnectionStatus;
  }
  if (serverConnectionStatus != GPRS.lastServerConnectionStatus || isGPRSConnected != GPRS.lastGPRSConnectionStatus)
  {
    retVal = true;
  }
  GPRS.lastGPRSConnectionStatus = isGPRSConnected;
  GPRS.lastServerConnectionStatus = serverConnectionStatus;
  return (retVal);
}

bool GPRSIsAttached()
{
  return (GPRS.post);
}

bool GPRSIsConnectedToServer()
{
  return (GPRS.serverConnectionStatus);
}

bool GPRSOTAIsOngoing()
{
  return (tb.Firmware_is_updating() && GPRSIsConnectedToServer());
}

void GPRS_get_triangulation_location()
{
  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int min = 0;
  int sec = 0;
  modem.getGsmLocation(&GPRS.longitud, &GPRS.latitud, &GPRS.accuracy, &year, &month, &day, &hour,
                       &min, &sec);
}

void GPRS_get_SIM_info()
{
  GPRS.CCID = modem.getSimCCID();

  GPRS.CCID.remove(GPRS.CCID.length() - 1);

  GPRS.IMEI = modem.getIMEI();

  GPRS.IMSI = modem.getIMSI();

  GPRS.COP = modem.getOperator();

  GPRS.IP = modem.localIP();

  GPRS_get_triangulation_location();

  logln("[GPRS] -> CCID is: " + GPRS.CCID);
  logln("[GPRS] -> IMEI is: " + GPRS.IMEI);
  logln("[GPRS] -> IMSI is: " + GPRS.IMSI);
  logln("[GPRS] -> COP is: " + GPRS.COP);
}

void GPRSUpdateCSQ()
{
  GPRS.CSQ = modem.getSignalQuality();
  logln("[GPRS] -> CSQ is: " + String(GPRS.CSQ));
}

void GPRSStatusHandler()
{
  if (GPRS.process)
  {
    if (GPRS.powerUp || GPRS.connect)
    {
      if (millis() - GPRS.processTime > GPRS_TIMEOUT)
      {
        logln("[GPRS] -> timeOut: " + String(GPRS.powerUp) + String(GPRS.connect) + String(GPRS.post) + String(GPRS.process));
        GPRS.timeOut = false;
        GPRS.process = false;
        GPRS.post = false;
        GPRS.connect = false;
        GPRS.powerUp = true;
        GPRS.serverConnectionStatus = false;
        logln("[GPRS] -> powering module down...");
        Serial2.print("AT+CPOWD=1\n");
        GPRS.packetSentenceTime = millis();
        GPRS.processTime = millis();
      }
    }
  }
  if (!GPRS.powerUp && !GPRS.connect && !GPRS.post)
  {
    GPRS.powerUp = true;
  }
}

void GPRSPowerUp()
{
  switch (GPRS.process)
  {
  case 0:
    GPRS.processTime = millis();
    GPIOWrite(GPRS_PWRKEY, LOW);
    GPRS.process++;
    GPRS.packetSentenceTime = millis();
    logln("[GPRS] -> powering up GPRS");
    break;
  case 1:
    if (millis() - GPRS.packetSentenceTime > 1000)
    {
      GPIOWrite(GPRS_PWRKEY, HIGH);
      GPRS.process++;
      logln("[GPRS] -> GPRS powered");
    }
    break;
  case 2:
    if (millis() - GPRS.packetSentenceTime > 1000)
    {
      clearGPRSBuffer();
      logln("[GPRS] -> Sending AT command");
      Serial2.print(SIMCOM800_AT);
      GPRS.packetSentenceTime = millis();
    }
    checkSerial(AT_OK, AT_ERROR);
    break;
  case 3:
    GPRS.powerUp = false;
    GPRS.connect = true;
    logln("[GPRS] -> Power up success");
    GPRS.process = false;
    break;
  }
}

void GPRSStablishConnection()
{
  switch (GPRS.process)
  {
  case 0:
    logln("[GPRS] -> Stablishing connection");
    GPRS.processTime = millis();
    GPRS.packetSentenceTime = millis();
    GPRS.process++;
    break;
  case 1:
    logln("[GPRS] -> Connecting...");
    GPRS.APN = APN_TM;
    if (modem.gprsConnect(GPRS.APN.c_str(), GPRS_USER, GPRS_PASS))
    {
      logln("[GPRS] -> Attached");
      GPRS.process++;
    }
    else
    {
      logln("[GPRS] -> Attach FAIL, retrying with different APN...");
      GPRS.APN = APN_TRUPHONE;
      if (modem.gprsConnect(GPRS.APN.c_str(), GPRS_USER, GPRS_PASS))
      {
        logln("[GPRS] -> Attached");
        GPRS.process++;
        GPRS.processTime = millis();
      }
      else
      {
        logln("[GPRS] -> Attach FAIL, retrying...");
      }
    }
    break;
  case 2:
    GPRS_get_SIM_info();
    GPRS.connect = false;
    GPRS.process = false;
    GPRS.post = true;
    break;
  }
}

void readGPRSData()
{
  while (Serial2.available())
  {
    GPRS.buffer[GPRS.bufferWritePos] = Serial2.read();
    Serial.print(GPRS.buffer[GPRS.bufferWritePos]);
    GPRS.bufferWritePos++;
    if (GPRS.bufferWritePos >= RX_BUFFER_LENGTH)
    {
      GPRS.bufferWritePos = 0;
      logln("[GPRS] -> Buffer overflow");
    }
    GPRS.charToRead++;
  }
}

void GPRSSetPostPeriod()
{
  if (GPRS.firstPublish)
  {
    if (in3.temperatureControl || in3.humidityControl)
    {
      GPRS.sendPeriod = actuatingGPRSPostPeriod;
    }
    else if (in3.phototherapy)
    {
      GPRS.sendPeriod = phototherapyGPRSPostPeriod;
    }
    else
    {
      GPRS.sendPeriod = standByGPRSPostPeriod;
    }
  }
  else
  {
    GPRS.sendPeriod = false;
  }
}

void GPRSProvisionResponse(Provision_Data &data)
{
  logln("[GPRS] -> Received device provision response");
  int jsonSize = measureJson(data) + 1;
  char buffer[jsonSize];
  serializeJson(data, buffer, jsonSize);
  logln("[GPRS] -> " + String(buffer));
  if (strncmp(data["status"], "SUCCESS", strlen("SUCCESS")) != 0)
  {
    logln("[GPRS] -> Provision response contains the error: ");
    logln("[GPRS] -> " + data["errorMsg"].as<String>());
    GPRS.provision_request_processed = true;
    return;
  }
  if (strncmp(data["credentialsType"], "ACCESS_TOKEN", strlen("ACCESS_TOKEN")) == 0)
  {
    credentials.client_id = "";
    credentials.username = data["credentialsValue"].as<String>();
    credentials.password = "";
    GPRS.provisioned = true;
    GPRS.device_token = credentials.username;
    EEPROM.writeString(EEPROM_THINGSBOARD_TOKEN, GPRS.device_token);
    EEPROM.write(EEPROM_THINGSBOARD_PROVISIONED, GPRS.provisioned);
    EEPROM.commit();
    logln("[GPRS] -> Device provisioned successfully");
  }
  if (tb_provision.connected())
  {
    tb_provision.disconnect();
  }
  GPRS.provision_request_processed = true;
}

void TBProvision()
{
  if (!tb_provision.connected())
  {
    const Provision_Callback provisionCallback = (Provision_Callback)GPRSProvisionResponse;
    // Connect to the ThingsBoard
    logln("[WIFI] -> Sending provision request to: " + String(THINGSBOARD_SERVER));
    if (!tb_provision.connect(THINGSBOARD_SERVER, "provision", THINGSBOARD_PORT))
    {
      Serial.println("Failed to connect");
      return;
    }
    if (tb_provision.Provision_Subscribe(provisionCallback))
    {
    }
  }
  else
  {
    if (tb_provision.sendProvisionRequest(GPRS.CCID.c_str(), PROVISION_DEVICE_KEY, PROVISION_DEVICE_SECRET))
    {
      GPRS.provision_request_sent = true;
      Serial.println("Provision request was sent!");
    }
    else
    {
      Serial.println("Provision request FAILED!");
    }
  }
}

void UpdatedCallback(const bool &success)
{
  if (success)
  {
    logln("[GPRS] -> Done, Reboot now");
    esp_restart();
  }
  else
  {
    logln("[GPRS] -> No new firmware");
  }
}

void addIntVariableToTelemetryJSON(JsonObject &json, const char *key, const int &value)
{
  if (value != 0)
  {
    json[key] = value;
  }
}

void GPRSCheckOTA()
{
  Serial.println("Checking GPRS firwmare Update...");
  tb.Start_Firmware_Update(CURRENT_FIRMWARE_TITLE, FWversion, UpdatedCallback);
}

void addConfigTelemetriesToGPRSJSON()
{
  addVariableToTelemetryGPRSJSON[SN_KEY] = in3.serialNumber;
  addVariableToTelemetryGPRSJSON[HW_NUM_KEY] = HW_NUM;
  addVariableToTelemetryGPRSJSON[HW_REV_KEY] = String(HW_REVISION);
  addVariableToTelemetryGPRSJSON[FW_VERSION_KEY] = FWversion;
  addVariableToTelemetryGPRSJSON[CCID_KEY] = GPRS.CCID.c_str();
  addVariableToTelemetryGPRSJSON[IMEI_KEY] = GPRS.IMEI.c_str();
  addVariableToTelemetryGPRSJSON[APN_KEY] = GPRS.APN.c_str();
  addVariableToTelemetryGPRSJSON[COP_KEY] = GPRS.COP.c_str();

  addVariableToTelemetryGPRSJSON[SYS_CURR_STANDBY_TEST_KEY] = roundSignificantDigits(in3.system_current_standby_test, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[HEATER_CURR_TEST_KEY] = roundSignificantDigits(in3.heater_current_test, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[FAN_CURR_TEST_KEY] = roundSignificantDigits(in3.fan_current_test, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[PHOTOTHERAPY_CURR_KEY] = roundSignificantDigits(in3.phototherapy_current_test, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[HUMIDIFIER_CURR_KEY] = roundSignificantDigits(in3.humidifier_current_test, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[DISPLAY_CURR_TEST_KEY] = roundSignificantDigits(in3.display_current_test, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[BUZZER_CURR_TEST_KEY] = roundSignificantDigits(in3.buzzer_current_test, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[HW_TEST_KEY] = in3.HW_test_error_code;

  addVariableToTelemetryGPRSJSON[LOCATION_LONGTITUD_KEY] = GPRS.longitud;
  addVariableToTelemetryGPRSJSON[LOCATION_LATITUD_KEY] = GPRS.latitud;
  addVariableToTelemetryGPRSJSON[TRI_ACCURACY_KEY] = GPRS.accuracy;
  addVariableToTelemetryGPRSJSON[UI_LANGUAGE_KEY] = in3.language;
  addVariableToTelemetryGPRSJSON[CALIBRATED_SENSOR_KEY] = !in3.calibrationError;
}

void addTelemetriesToGPRSJSON()
{
  addVariableToTelemetryGPRSJSON[AIR_TEMPERATURE_KEY] = roundSignificantDigits(in3.temperature[airSensor], TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[SKIN_TEMPERATURE_KEY] = roundSignificantDigits(in3.temperature[skinSensor], TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[PHOTOTHERAPY_ACTIVE_KEY] = in3.phototherapy;
  addVariableToTelemetryGPRSJSON[HUMIDITY_KEY] = roundSignificantDigits(in3.humidity, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[SYSTEM_CURRENT_KEY] = roundSignificantDigits(in3.system_current, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[SYSTEM_VOLTAGE_KEY] = roundSignificantDigits(in3.system_voltage, TELEMETRIES_DECIMALS);
  addVariableToTelemetryGPRSJSON[CELL_SIGNAL_QUALITY_KEY] = GPRS.CSQ;

  if (in3.temperatureControl || in3.humidityControl)
  {
    addVariableToTelemetryGPRSJSON[FAN_CURRENT_KEY] = roundSignificantDigits(in3.fan_current, TELEMETRIES_DECIMALS);
    addVariableToTelemetryGPRSJSON[CONTROL_ACTIVE_TIME_KEY] = roundSignificantDigits(in3.control_active_time, TELEMETRIES_DECIMALS);
    addVariableToTelemetryGPRSJSON[HEATER_ACTIVE_TIME_KEY] = roundSignificantDigits(in3.heater_active_time, TELEMETRIES_DECIMALS);
    addVariableToTelemetryGPRSJSON[FAN_ACTIVE_TIME_KEY] = roundSignificantDigits(in3.fan_active_time, TELEMETRIES_DECIMALS);
    if (!GPRS.firstConfigPost)
    {
      GPRS.firstConfigPost = true;
      addVariableToTelemetryGPRSJSON[CONTROL_ACTIVE_KEY] = true;
      if (in3.temperatureControl)
      {
        if (in3.controlMode == AIR_CONTROL)
        {
          addVariableToTelemetryGPRSJSON[CONTROL_MODE_KEY] = "AIR";
        }
        else
        {
          addVariableToTelemetryGPRSJSON[CONTROL_MODE_KEY] = "SKIN";
        }
        addVariableToTelemetryGPRSJSON[DESIRED_TEMPERATURE_KEY] = in3.desiredControlTemperature;
      }
      if (in3.humidityControl)
      {
        addVariableToTelemetryGPRSJSON[DESIRED_HUMIDITY_KEY] = in3.desiredControlHumidity;
      }
    }
  }
  else
  {
    GPRS.firstConfigPost = false;
    addVariableToTelemetryGPRSJSON[CONTROL_ACTIVE_KEY] = false;
    addVariableToTelemetryGPRSJSON[STANBY_TIME_KEY] = roundSignificantDigits(in3.standby_time, TELEMETRIES_DECIMALS);
  }
  if (in3.humidityControl)
  {
    addVariableToTelemetryGPRSJSON[HUMIDIFIER_CURRENT_KEY] = roundSignificantDigits(in3.humidifier_current, TELEMETRIES_DECIMALS);
    addVariableToTelemetryGPRSJSON[HUMIDIFIER_VOLTAGE_KEY] = roundSignificantDigits(in3.humidifier_voltage, TELEMETRIES_DECIMALS);
    addVariableToTelemetryGPRSJSON[HUMIDIFIER_ACTIVE_TIME_KEY] = roundSignificantDigits(in3.humidifier_active_time, TELEMETRIES_DECIMALS);
  }
  if (in3.phototherapy)
  {
    addVariableToTelemetryGPRSJSON[PHOTOTHERAPY_CURRENT_KEY] = roundSignificantDigits(in3.phototherapy_current, TELEMETRIES_DECIMALS);
    addVariableToTelemetryGPRSJSON[PHOTHERAPY_ACTIVE_TIME_KEY] = roundSignificantDigits(in3.phototherapy_active_time, TELEMETRIES_DECIMALS);
  }
}

void GPRSPost()
{
  if (!GPRS.provisioned)
  {
    if (!GPRS.provision_request_sent)
    {
      TBProvision();
    }
    tb_provision.loop();
  }
  else
  {
    if (!tb.connected())
    {
      // Connect to the ThingsBoard
      logln("[GPRS] -> Connecting over GPRS to: " + String(THINGSBOARD_SERVER) + " with token " + String(GPRS.device_token));
      if (!tb.connect(THINGSBOARD_SERVER, GPRS.device_token.c_str()))
      {
        logln("[GPRS] -> Failed to connect");
        return;
      }
      else
      {
        GPRS.serverConnectionStatus = true;
        if (ENABLE_GPRS_OTA && !GPRS.OTA_requested)
        {
          GPRSCheckOTA();
          GPRS.OTA_requested = true;
        }
      }
    }
    if (tb.connected() && millis() - GPRS.lastSent > secsToMillis(GPRS.sendPeriod))
    {
      // Send our firmware title and version
      // StaticJsonDocument<JSON_OBJECT_SIZE(2)> TB_telemetries;
      // JsonObject telemetriesObject = TB_telemetries.to<JsonObject>();

      logln("[GPRS] -> sendPeriod is " + String(GPRS.sendPeriod) + " secs");
      logln("[GPRS] -> Posting GPRS data...");

      if (!GPRS.firstPublish)
      {
        GPRS.firstPublish = true;
        addConfigTelemetriesToGPRSJSON();
        if (tb.sendTelemetryJson(addVariableToTelemetryGPRSJSON, JSON_STRING_SIZE(measureJson(addVariableToTelemetryGPRSJSON))))
        {
          logln("[GPRS] -> GPRS MQTT PUBLISH CONFIG SUCCESS");
        }
        else
        {
          logln("[GPRS] -> GPRS MQTT PUBLISH CONFIG FAIL");
        }
        GPRS_JSON.clear();
      }
      GPRSUpdateCSQ();
      addTelemetriesToGPRSJSON();
      if (tb.sendTelemetryJson(addVariableToTelemetryGPRSJSON, JSON_STRING_SIZE(measureJson(addVariableToTelemetryGPRSJSON))))
      {
        logln("[GPRS] -> GPRS MQTT PUBLISH TELEMETRIES SUCCESS");
      }
      else
      {
        logln("[GPRS] -> GPRS MQTT PUBLISH TELEMETRIES FAIL");
      }
      GPRS_JSON.clear();
      GPRS.process = false;
      GPRS.lastSent = millis();
    }
  }
}

void GPRS_TB_Init()
{
  GPRS.provisioned = EEPROM.read(EEPROM_THINGSBOARD_PROVISIONED);
  if (GPRS.provisioned)
  {
    GPRS.device_token = EEPROM.readString(EEPROM_THINGSBOARD_TOKEN);
  }
}

void GPRS_Handler()
{
  GPRSStatusHandler();
  if (GPRS.powerUp)
  {
    GPRSPowerUp();
  }
  if (GPRS.connect)
  {
    GPRSStablishConnection();
  }
  if (GPRS.post)
  {
    GPRSSetPostPeriod();
    GPRSPost();
    tb.loop();
  }
  readGPRSData();
}