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
#include "GPRS.h"
#include "main.h"
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include "ThingsBoard.h"

// Initialize GSM modem
TinyGsm modem(modemSerial);

// Initialize GSM client
TinyGsmClient client(modem);

// Initialize ThingsBoard instance
ThingsBoard tb(client);

int GPRSsequence = false;

struct GPRSstruct GPRS;
extern in3ator_parameters in3;

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
  Serial2.begin(115200);
  GPRS.powerUp = true;
  digitalWrite(GPRS_PWRKEY, HIGH);
}

void GPRS_get_triangulation_location()
{
  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int min = 0;
  int sec = 0;
  modem.getGsmLocation(&GPRS.latitud, &GPRS.longitud, &GPRS.accuracy, &year, &month, &day, &hour,
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

  GPRS.CSQ = modem.getSignalQuality();

  GPRS_get_triangulation_location();
}

void GPRSStatusHandler()
{
  if (GPRS.process)
  {
    if (GPRS.powerUp || GPRS.connect || GPRS.post)
    {
      if (millis() - GPRS.processTime > GPRS_TIMEOUT)
      {
        logln("[GPRS] -> timeOut: " + String(GPRS.powerUp) + String(GPRS.connect) + String(GPRS.post) + String(GPRS.process));
        GPRS.timeOut = false;
        GPRS.process = false;
        GPRS.post = false;
        GPRS.connect = false;
        GPRS.powerUp = true;
        logln("[GPRS] -> powering module down...");
        Serial2.print("AT+CPOWD=1\n");
        GPRS.packetSentenceTime = millis();
        GPRS.processTime = millis();
      }
    }
  }
  if (!GPRS.powerUp && !GPRS.connect && !GPRS.post)
  {
    if (!GPRS.connectionStatus)
    {
      GPRS.powerUp = true;
    }
  }
}

void GPRSPowerUp()
{
  switch (GPRS.process)
  {
  case 0:
    if (millis() - GPRS.packetSentenceTime > 2000)
    {
      GPRS.processTime = millis();
      digitalWrite(GPRS_PWRKEY, LOW);
      GPRS.process++;
      GPRS.packetSentenceTime = millis();
      logln("[GPRS] -> powering up GPRS");
    }
    break;
  case 1:
    if (millis() - GPRS.packetSentenceTime > 2000)
    {
      digitalWrite(GPRS_PWRKEY, HIGH);
      GPRS.packetSentenceTime = millis();
      GPRS.process++;
      logln("[GPRS] -> GPRS powered");
    }
    break;
  case 2:
    if (millis() - GPRS.packetSentenceTime > 7000)
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
    if (millis() - GPRS.processTime > 5000)
    {
      logln("[GPRS] -> Connecting...");
      GPRS.APN = APN_TM;
      if (modem.gprsConnect(GPRS.APN.c_str(), GPRS_USER, GPRS_PASS))
      {
        GPRS_get_SIM_info();
        logln("[GPRS] -> Attached");
        GPRS.process++;
      }
      else
      {
        logln("[GPRS] -> Attach FAIL, retrying with different APN...");
        GPRS.APN = APN_TRUPHONE;
        if (modem.gprsConnect(GPRS.APN.c_str(), GPRS_USER, GPRS_PASS))
        {
          GPRS_get_SIM_info();
          logln("[GPRS] -> Attached");
          GPRS.process++;
        }
        else
        {
          logln("[GPRS] -> Attach FAIL, retrying...");
        }
      }
    }
    break;
  case 2:

    GPRS.connectionStatus = true;
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

void GPRSPost()
{

  if (!tb.connected())
  {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN))
    {
      Serial.println("Failed to connect");
      return;
    }
    delay(1000);
  }
  if (tb.connected() && millis() - GPRS.lastSent > millisToSecs(GPRS.sendPeriod))
  {
    logln("[GPRS] -> Posting GPRS data...");
    if (!GPRS.firstPost)
    {
      GPRS.firstPost = true;
      tb.sendTelemetryInt("SN", in3.serialNumber);
      tb.sendTelemetryFloat("SYS_current_stanby_test", in3.system_current_standby_test);
      tb.sendTelemetryFloat("Heater_current_test", in3.heater_current_test);
      tb.sendTelemetryFloat("Fan_current_test", in3.fan_current_test);
      tb.sendTelemetryFloat("Phototherapy_current_test", in3.phototherapy_current_test);
      tb.sendTelemetryFloat("Humidifier_current_test", in3.humidifier_current_test);
      tb.sendTelemetryFloat("Display_current_test", in3.humidifier_current_test);
      tb.sendTelemetryFloat("Buzzer_current_test", in3.buzzer_current_test);
      tb.sendTelemetryInt("HW_Test", in3.HW_test_error_code);
      tb.sendTelemetryInt("CSQ", GPRS.CSQ);
      tb.sendTelemetryString("CCID", GPRS.CCID.c_str());
      tb.sendTelemetryString("IMEI", GPRS.IMEI.c_str());
      tb.sendTelemetryString("COP", GPRS.COP.c_str());
      tb.sendTelemetryString("APN", GPRS.APN.c_str());
      tb.sendTelemetryFloat("tri_longitud", GPRS.longitud);
      tb.sendTelemetryFloat("tri_latitud", GPRS.latitud);
      tb.sendTelemetryFloat("tri_accuracy", GPRS.accuracy);
      tb.sendTelemetryInt("UI_language", in3.language);
    }

    tb.sendTelemetryFloat("Air_temp", in3.temperature[airSensor]);
    tb.sendTelemetryFloat("Skin_temp", in3.temperature[skinSensor]);
    tb.sendTelemetryInt("Humidity", in3.humidity);

    if (in3.temperatureControl || in3.humidityControl)
    {
      if (!GPRS.firstConfigPost)
      {
        GPRS.firstConfigPost = true;
        if (in3.temperatureControl)
        {
          if (in3.controlMode == AIR_CONTROL)
          {
            tb.sendTelemetryString("Control_mode", "AIR");
          }
          else
          {
            tb.sendTelemetryString("Control_mode", "SKIN");
          }
          tb.sendTelemetryFloat("Temp_desired", in3.desiredControlTemperature);
        }
        if (in3.humidityControl)
        {
          tb.sendTelemetryFloat("Hum_desired", in3.desiredControlHumidity);
        }
      }
      tb.sendTelemetryFloat("SYS_current", in3.system_current);
      tb.sendTelemetryFloat("Fan_current", in3.fan_current);
    }
    else
    {
      GPRS.firstConfigPost = false;
    }
    if (in3.humidityControl)
    {
      tb.sendTelemetryFloat("Humidifier_current", in3.humidifier_current);
      tb.sendTelemetryFloat("Humidifier_voltage", in3.humidifier_voltage);
    }
    if (in3.phototherapy)
    {
      tb.sendTelemetryFloat("Phototherapy_current", in3.phototherapy_current);
    }

    GPRS.process = false;
    GPRS.lastSent = millis();
    logln("[GPRS] -> GPRS POST SUCCESS");
  }
}

void GPRS_Handler()
{
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
  }
  readGPRSData();
}