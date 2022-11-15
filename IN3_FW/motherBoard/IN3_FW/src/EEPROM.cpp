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
#include "main.h"
#include "GPRS.h"

extern bool autoLock;
extern bool WIFI_EN;
bool firstTurnOn;
extern bool controlAlgorithm;
extern int presetTemp[2]; // preset baby skin temperature
extern double RawTemperatureLow[numSensors], RawTemperatureRange[numSensors];
extern double ReferenceTemperatureRange, ReferenceTemperatureLow;
extern double fineTuneSkinTemperature;

extern in3ator_parameters in3;
extern struct GPRSstruct GPRS;

void initEEPROM()
{
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    logln("failed to initialise EEPROM");
  }
  if (EEPROM.read(EEPROM_checkStatus))
  {
    EEPROM.write(EEPROM_checkStatus, 0);
    EEPROM.commit();
    vTaskDelay(30);
    if (EEPROM.read(EEPROM_checkStatus) != 0)
    {
    }
  }
  else
  {
    EEPROM.write(EEPROM_checkStatus, 1);
    EEPROM.commit();
    vTaskDelay(30);
    if (EEPROM.read(EEPROM_checkStatus) != 1)
    {
    }
  }
  firstTurnOn = EEPROM.read(EEPROM_firstTurnOn);
  if (firstTurnOn)
  { // firstTimePowerOn
    for (int i = false; i < EEPROM_SIZE; i++)
    {
      EEPROM.write(i, 0);
    }
    loadDefaultCalibration();
    loaddefaultValues();
    logln("[FLASH] -> First turn on, loading default values");
  }
  else
  {
    logln("[FLASH] -> Loading variables stored in flash");
    recapVariables();
  }
}

void loaddefaultValues()
{
  autoLock = DEFAULT_AUTOLOCK;
  WIFI_EN = DEFAULT_WIFI_EN;
  in3.language = defaultLanguage;
  controlAlgorithm = DEFAULT_CONTROL_ALGORITHM;
  in3.controlMode = AIR_CONTROL;
  in3.desiredControlTemperature = presetTemp[in3.controlMode];
  in3.desiredControlHumidity = presetHumidity;
  EEPROM.write(EEPROM_autoLock, autoLock);
  EEPROM.write(EEPROM_WIFI_EN, WIFI_EN);
  EEPROM.write(EEPROM_language, in3.language);
  EEPROM.write(EEPROM_controlAlgorithm, controlAlgorithm);
  EEPROM.write(EEPROM_controlMode, in3.controlMode);
  EEPROM.write(EEPROM_desiredControlMode, in3.desiredControlTemperature);
  EEPROM.commit();
}

void loadDefaultCalibration()
{
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureLowCorrection, 25.54);
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureRangeCorrection, 14.28);
  EEPROM.writeFloat(EEPROM_RawAirTemperatureLowCorrection, 0);
  EEPROM.writeFloat(EEPROM_RawAirTemperatureRangeCorrection, 0);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureLowCorrection, 0);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureRangeCorrection, 0);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureRange, 14.03);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureLow, 21.15);
  EEPROM.writeFloat(EEPROM_FineTuneSkinTemperature, 0);
  EEPROM.commit();
}

void recapVariables()
{
  autoLock = EEPROM.read(EEPROM_autoLock);
  in3.language = EEPROM.read(EEPROM_language);
  RawTemperatureLow[skinSensor] = EEPROM.readFloat(EEPROM_RawSkinTemperatureLowCorrection);
  RawTemperatureRange[skinSensor] = EEPROM.readFloat(EEPROM_RawSkinTemperatureRangeCorrection);
  RawTemperatureLow[airSensor] = EEPROM.readFloat(EEPROM_RawDigitalTemperatureLowCorrection);
  RawTemperatureRange[airSensor] = EEPROM.readFloat(EEPROM_RawDigitalTemperatureRangeCorrection);
  ReferenceTemperatureRange = EEPROM.readFloat(EEPROM_ReferenceTemperatureRange);
  ReferenceTemperatureLow = EEPROM.readFloat(EEPROM_ReferenceTemperatureLow);
  fineTuneSkinTemperature = EEPROM.readFloat(EEPROM_FineTuneSkinTemperature);
  for (int i = 0; i < numSensors; i++)
  {
    logln("calibration factors: " + String(RawTemperatureLow[i]) + "," + String(RawTemperatureRange[i]) + "," + String(ReferenceTemperatureRange) + "," + String(ReferenceTemperatureLow));
  }

  for (int i = 0; i < numSensors; i++)
  {
    if (RawTemperatureLow[i] > 100)
    {
      // critical error
    }
  }
  in3.serialNumber = EEPROM.read(EEPROM_SerialNumber);
  WIFI_EN = EEPROM.read(EEPROM_WIFI_EN);
  controlAlgorithm = EEPROM.read(EEPROM_controlAlgorithm);
  in3.controlMode = EEPROM.read(EEPROM_controlMode);
  in3.desiredControlTemperature = EEPROM.read(EEPROM_desiredControlMode);
  in3.desiredControlHumidity = EEPROM.read(EEPROM_desiredControlHumidity);
  GPRS.provisioned = EEPROM.read(EEPROM_THINGSBOARD_PROVISIONED);
  if (GPRS.provisioned)
  {
    GPRS.device_token = EEPROM.readString(EEPROM_THINGSBOARD_TOKEN);
  }
}

void saveCalibrationToEEPROM()
{
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureLowCorrection, RawTemperatureLow[skinSensor]);
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureRangeCorrection, RawTemperatureRange[skinSensor]);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureLowCorrection, RawTemperatureLow[airSensor]);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureRangeCorrection, RawTemperatureRange[airSensor]);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureRange, ReferenceTemperatureRange);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureLow, ReferenceTemperatureLow);
  EEPROM.writeFloat(EEPROM_FineTuneSkinTemperature, fineTuneSkinTemperature);
  EEPROM.commit();
}
