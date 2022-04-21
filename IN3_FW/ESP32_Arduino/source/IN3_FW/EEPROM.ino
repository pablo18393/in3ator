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
    logln("failed to initialise EEPROM");
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
  EEPROM.write(EEPROM_autoLock, autoLock);
  EEPROM.write(EEPROM_WIFI_EN, WIFI_EN);
  EEPROM.write(EEPROM_language, language);
  EEPROM.write(EEPROM_controlAlgorithm, controlAlgorithm);
  EEPROM.write(EEPROM_controlMode, controlMode);
  EEPROM.commit();
}

void loadDefaultCalibration() {
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureLowCorrection, 22.52);
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureRangeCorrection, 11.70);
  EEPROM.writeFloat(EEPROM_RawAirTemperatureLowCorrection, 0);
  EEPROM.writeFloat(EEPROM_RawAirTemperatureRangeCorrection, 0);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureLowCorrection, 0);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureRangeCorrection, 0);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureRange, 10.57);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureLow, 21.29);
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

  for (int i = 0; i < numSensors; i++) {
    logln("calibration factors: " + String(RawTemperatureLow [i]) + "," + String (RawTemperatureRange [i]) + "," + String (ReferenceTemperatureRange) + "," + String (ReferenceTemperatureLow));
  }
  
  for (int i = 0; i < numSensors; i++) {
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
  controlMode = EEPROM.read(EEPROM_controlMode);
}


void saveCalibrationToEEPROM() {
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureLowCorrection, RawTemperatureLow[skinSensor]);
  EEPROM.writeFloat(EEPROM_RawSkinTemperatureRangeCorrection, RawTemperatureRange[skinSensor]);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureLowCorrection, RawTemperatureLow[airSensor]);
  EEPROM.writeFloat(EEPROM_RawDigitalTemperatureRangeCorrection, RawTemperatureRange[airSensor]);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureRange, ReferenceTemperatureRange);
  EEPROM.writeFloat(EEPROM_ReferenceTemperatureLow, ReferenceTemperatureLow);
  EEPROM.commit();
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
