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
#define EEPROM_alarmsEnable 80
#define EEPROM_diffHumidity 90
#define EEPROM_RawSkinTemperatureLowCorrection 100
#define EEPROM_RawSkinTemperatureRangeCorrection 110
#define EEPROM_RawAirTemperatureLowCorrection 120
#define EEPROM_RawAirTemperatureRangeCorrection 130
#define EEPROM_RawDigitalTemperatureLowCorrection 140
#define EEPROM_RawDigitalTemperatureRangeCorrection 150
#define EEPROM_controlAlgorithm 160
#define EEPROM_ReferenceTemperatureRange 170
#define EEPROM_ReferenceTemperatureLow 180



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
  controlMode = defaultcontrolMode;
  WIFI_EN = defaultWIFI_EN;
  language = defaultLanguage;
  controlAlgorithm = defaultcontrolAlgorithm;
  alarmsEnable = defaultAlarmsEnable;
  EEPROM.write(EEPROM_autoLock, autoLock);
  EEPROM.write(EEPROM_WIFI_EN, WIFI_EN);
  EEPROM.write(EEPROM_language, language);
  EEPROM.write(EEPROM_controlAlgorithm, controlAlgorithm);
  EEPROM.write(EEPROM_controlMode, controlMode);
  EEPROM.write(EEPROM_alarmsEnable, alarmsEnable);
  EEPROM.commit();
}

void loadDefaultCalibration(){
    EEPROM.writeFloat(EEPROM_RawSkinTemperatureLowCorrection, 23.22);
    EEPROM.writeFloat(EEPROM_RawSkinTemperatureRangeCorrection, 22.71);
    EEPROM.writeFloat(EEPROM_RawAirTemperatureLowCorrection, 22.65);
    EEPROM.writeFloat(EEPROM_RawAirTemperatureRangeCorrection, 22.63);
    EEPROM.writeFloat(EEPROM_RawDigitalTemperatureLowCorrection, 18.43);
    EEPROM.writeFloat(EEPROM_RawDigitalTemperatureRangeCorrection, 0.16);
    EEPROM.writeFloat(EEPROM_ReferenceTemperatureRange, 22.06);
    EEPROM.writeFloat(EEPROM_ReferenceTemperatureLow, 18.98);
    EEPROM.commit();
}

void recapVariables() {
  autoLock = EEPROM.read(EEPROM_autoLock);
  language = EEPROM.read(EEPROM_language);
  RawTemperatureLow[babyNTC] = EEPROM.readFloat(EEPROM_RawSkinTemperatureLowCorrection);
  RawTemperatureRange[babyNTC] = EEPROM.readFloat(EEPROM_RawSkinTemperatureRangeCorrection);
  RawTemperatureLow[airNTC] = EEPROM.readFloat(EEPROM_RawAirTemperatureLowCorrection);
  RawTemperatureRange[airNTC] = EEPROM.readFloat(EEPROM_RawAirTemperatureRangeCorrection);
  RawTemperatureLow[digitalTempSensor] = EEPROM.readFloat(EEPROM_RawDigitalTemperatureLowCorrection);
  RawTemperatureRange[digitalTempSensor] = EEPROM.readFloat(EEPROM_RawDigitalTemperatureRangeCorrection);
  ReferenceTemperatureRange = EEPROM.readFloat(EEPROM_ReferenceTemperatureRange);
  ReferenceTemperatureLow = EEPROM.readFloat(EEPROM_ReferenceTemperatureLow);

  for (int i = 0; i < numTempSensors; i++) {
    Serial.println("calibration factors: " + String(RawTemperatureLow [i]) + "," + String (RawTemperatureRange [i]) + "," + String (ReferenceTemperatureRange) + "," + String (ReferenceTemperatureLow));
  }


  for (int i = 0; i < numTempSensors; i++) {
    if (RawTemperatureLow[i] > 100) {
      //critical error
    }
  }
  diffHumidity = EEPROM.read(EEPROM_diffHumidity);
  if (diffHumidity > 100) {
    diffHumidity = false;
  }
  serialNumber = EEPROM.read(EEPROM_SerialNumber);
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
