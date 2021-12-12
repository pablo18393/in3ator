#define EEPROM_SIZE 256

//EEPROM variables
#define EEPROM_checkStatus 0
#define EEPROM_firstTurnOn 10
#define EEPROM_autoLock 20
#define EEPROM_language 30
#define EEPROM_HeaterPower 40
#define EEPROM_WIFI_EN 50
#define EEPROM_usedGenericMosfet 60
#define EEPROM_controlMode 70
#define EEPROM_alarmsEnable 80
#define EEPROM_diffHumidity 90
#define EEPROM_errorSkinTemperatureFactor 100
#define EEPROM_errorSkinTemperatureOffset 110
#define EEPROM_errorAirTemperatureFactor 120
#define EEPROM_errorAirTemperatureOffset 130
#define EEPROM_errorDigitalTemperatureFactor 140
#define EEPROM_errorDigitalTemperatureOffset 150
#define EEPROM_controlAlgorithm 160


bool firstTurnOn;

void initEEPROM() {
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
  }
  if (EEPROM.read(EEPROM_checkStatus)) {
    EEPROM.write(EEPROM_checkStatus, 0);
    EEPROM.commit();
    delay(30);
    if (EEPROM.read(EEPROM_checkStatus) != 0) {
    }
  }
  else {
    EEPROM.write(EEPROM_checkStatus, 1);
    EEPROM.commit();
    delay(30);
    if (EEPROM.read(EEPROM_checkStatus) != 1) {
    }
  }
  firstTurnOn = EEPROM.read(EEPROM_firstTurnOn);
  if (firstTurnOn) { //firstTimePowerOn
    for (int i = false; i < EEPROM_SIZE; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
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
  HeaterPower = defaultHeaterPower;
  controlMode = defaultcontrolMode;
  WIFI_EN = defaultWIFI_EN;
  language = defaultLanguage;
  controlAlgorithm = defaultcontrolAlgorithm;
  alarmsEnable = defaultAlarmsEnable;
  EEPROM.write(EEPROM_autoLock, autoLock);
  EEPROM.write(EEPROM_HeaterPower, HeaterPower);
  EEPROM.write(EEPROM_WIFI_EN, WIFI_EN);
  EEPROM.write(EEPROM_language, language);
  EEPROM.write(EEPROM_controlAlgorithm, controlAlgorithm);
  EEPROM.write(EEPROM_controlMode, controlMode);
  EEPROM.write(EEPROM_alarmsEnable, alarmsEnable);
  EEPROM.commit();
}

void recapVariables() {
  autoLock = EEPROM.read(EEPROM_autoLock);
  language = EEPROM.read(EEPROM_language);

  temperatureCalibrationFactor[babyNTC] = EEPROM.readFloat(EEPROM_errorSkinTemperatureFactor);
  temperatureCalibrationOffset[babyNTC] = EEPROM.readFloat(EEPROM_errorSkinTemperatureOffset);
  temperatureCalibrationFactor[airNTC] = EEPROM.readFloat(EEPROM_errorAirTemperatureFactor);
  temperatureCalibrationOffset[airNTC] = EEPROM.readFloat(EEPROM_errorAirTemperatureOffset);
  temperatureCalibrationFactor[digitalTempSensor] = EEPROM.readFloat(EEPROM_errorDigitalTemperatureFactor);
  temperatureCalibrationOffset[digitalTempSensor] = EEPROM.readFloat(EEPROM_errorDigitalTemperatureOffset);

  for (int i = 0; i < numTempSensors; i++) {
    Serial.println("calibration factors: " + String(temperatureCalibrationFactor [i]) + "x +" + String (temperatureCalibrationOffset [i]));
  }


  for (int i = 0; i < numTempSensors; i++) {
    if (temperatureCalibrationFactor[i] > 100) {
      //critical error
    }
  }
  diffHumidity = EEPROM.read(EEPROM_diffHumidity);
  if (diffHumidity > 100) {
    diffHumidity = false;
  }
  HeaterPower = EEPROM.read(EEPROM_HeaterPower);
  WIFI_EN = EEPROM.read(EEPROM_WIFI_EN);
  controlAlgorithm = EEPROM.read(EEPROM_controlAlgorithm);
  alarmsEnable = EEPROM.read(EEPROM_alarmsEnable);
  controlMode = EEPROM.read(EEPROM_controlMode);
}

long EEPROMReadLong(int p_address)
{
  int lowByte = EEPROM.read(p_address);
  int highByte = EEPROM.read(p_address + 1);
  return ((lowByte << 0) & 0xFFFF) + ((highByte << 16) & 0xFFFF0000);
}
void EEPROMWriteLong(int p_address, long p_value)
{
  int lowByte = ((p_value >> 0) & 0xFFFFFFFF);
  int highByte = ((p_value >> 16) & 0xFFFFFFFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
  EEPROM.commit();
}
