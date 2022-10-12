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

#define EEPROM_SIZE 256

//EEPROM variables
#define EEPROM_checkStatus 0
#define EEPROM_firstTurnOn 10
#define EEPROM_autoLock 20
#define EEPROM_language 30
#define EEPROM_SerialNumber 40
#define EEPROM_WIFI_EN 50
#define EEPROM_usedGenericMosfet 60
#define EEPROM_controlMode 70
#define EEPROM_desiredControlMode 80
#define EEPROM_desiredControlHumidity 90
#define EEPROM_RawSkinTemperatureLowCorrection 100
#define EEPROM_RawSkinTemperatureRangeCorrection 110
#define EEPROM_RawAirTemperatureLowCorrection 120
#define EEPROM_RawAirTemperatureRangeCorrection 130
#define EEPROM_RawDigitalTemperatureLowCorrection 140
#define EEPROM_RawDigitalTemperatureRangeCorrection 150
#define EEPROM_controlAlgorithm 160
#define EEPROM_ReferenceTemperatureRange 170
#define EEPROM_ReferenceTemperatureLow 180
#define EEPROM_FineTuneSkinTemperature 190


bool firstTurnOn;

void initEEPROM() {
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    logln("failed to initialise EEPROM");
  }
  if (EEPROM.read(EEPROM_checkStatus)) {
    EEPROM.write(EEPROM_checkStatus, 0);
    EEPROM.commit();
    vTaskDelay(30);
    if (EEPROM.read(EEPROM_checkStatus) != 0) {
    }
  }
  else {
    EEPROM.write(EEPROM_checkStatus, 1);
    EEPROM.commit();
    vTaskDelay(30);
    if (EEPROM.read(EEPROM_checkStatus) != 1) {
    }
  }
  firstTurnOn = EEPROM.read(EEPROM_firstTurnOn);
  if (firstTurnOn) { //firstTimePowerOn
    for (int i = false; i < EEPROM_SIZE; i++) {
      EEPROM.write(i, 0);
    }
    loadDefaultCalibration();
    loaddefaultValues();
    logln("[FLASH] -> First turn on, loading default values");
  }
  else {
    logln("[FLASH] -> Loading variables stored in flash");
    recapVariables();
  }
}

void loaddefaultValues() {
  autoLock = defaultAutoLock;
  WIFI_EN = defaultWIFI_EN;
  language = defaultLanguage;
  controlAlgorithm = defaultcontrolAlgorithm;
  controlMode = defaultcontrolMode;
  desiredControlTemperature = presetTemp[controlMode];
  desiredControlHumidity = presetHumidity;
  EEPROM.write(EEPROM_autoLock, autoLock);
  EEPROM.write(EEPROM_WIFI_EN, WIFI_EN);
  EEPROM.write(EEPROM_language, language);
  EEPROM.write(EEPROM_controlAlgorithm, controlAlgorithm);
  EEPROM.write(EEPROM_controlMode, controlMode);
  EEPROM.write(EEPROM_desiredControlMode, desiredControlTemperature);
  EEPROM.commit();
}

void loadDefaultCalibration() {
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

void recapVariables() {
  autoLock = EEPROM.read(EEPROM_autoLock);
  language = EEPROM.read(EEPROM_language);
  RawTemperatureLow[skinSensor] = EEPROM.readFloat(EEPROM_RawSkinTemperatureLowCorrection);
  RawTemperatureRange[skinSensor] = EEPROM.readFloat(EEPROM_RawSkinTemperatureRangeCorrection);
  RawTemperatureLow[airSensor] = EEPROM.readFloat(EEPROM_RawDigitalTemperatureLowCorrection);
  RawTemperatureRange[airSensor] = EEPROM.readFloat(EEPROM_RawDigitalTemperatureRangeCorrection);
  ReferenceTemperatureRange = EEPROM.readFloat(EEPROM_ReferenceTemperatureRange);
  ReferenceTemperatureLow = EEPROM.readFloat(EEPROM_ReferenceTemperatureLow);
  fineTuneSkinTemperature = EEPROM.readFloat(EEPROM_FineTuneSkinTemperature);
  for (int i = 0; i < numSensors; i++) {
    logln("calibration factors: " + String(RawTemperatureLow [i]) + "," + String (RawTemperatureRange [i]) + "," + String (ReferenceTemperatureRange) + "," + String (ReferenceTemperatureLow));
  }

  for (int i = 0; i < numSensors; i++) {
    if (RawTemperatureLow[i] > 100) {
      //critical error
    }
  }
  serialNumber = EEPROM.read(EEPROM_SerialNumber);
  WIFI_EN = EEPROM.read(EEPROM_WIFI_EN);
  controlAlgorithm = EEPROM.read(EEPROM_controlAlgorithm);
  controlMode = EEPROM.read(EEPROM_controlMode);
  desiredControlTemperature = EEPROM.read(EEPROM_desiredControlMode);
  desiredControlHumidity = EEPROM.read(EEPROM_desiredControlHumidity);
}


void saveCalibrationToEEPROM() {
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureLowCorrection, RawTemperatureLow[skinSensor]);
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureRangeCorrection, RawTemperatureRange[skinSensor]);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureLowCorrection, RawTemperatureLow[airSensor]);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureRangeCorrection, RawTemperatureRange[airSensor]);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureRange, ReferenceTemperatureRange);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureLow, ReferenceTemperatureLow);
  EEPROM.writeFloat(EEPROM_FineTuneSkinTemperature, fineTuneSkinTemperature);
  EEPROM.commit();
}
