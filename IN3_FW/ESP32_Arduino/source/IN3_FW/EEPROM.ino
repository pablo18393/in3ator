#define EEPROM_SIZE 256

//EEPROM variables
#define EEPROM_checkStatus 0
#define EEPROM_firstTurnOn 10
#define EEPROM_autoLock 20
#define EEPROM_language 30
#define EEPROM_HeaterPower 40
#define EEPROM_UARTDebug 50
#define EEPROM_usedGenericMosfet 60
#define EEPROM_controlMode 70
#define EEPROM_alarmsEnable 80
#define EEPROM_diffHumidity 90
#define EEPROM_diffSkinTemperature 100
#define EEPROM_diffAirTemperature 110
#define EEPROM_controlAlgorithm 120


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
  UARTDebug = defaultUARTDebug;
  language = defaultLanguage;
  controlAlgorithm = defaultcontrolAlgorithm;
  alarmsEnable = defaultAlarmsEnable;
  EEPROM.write(EEPROM_autoLock, autoLock);
  EEPROM.write(EEPROM_HeaterPower, HeaterPower);
  EEPROM.write(EEPROM_UARTDebug, UARTDebug);
  EEPROM.write(EEPROM_language, language);
  EEPROM.write(EEPROM_controlAlgorithm, controlAlgorithm);
  EEPROM.write(EEPROM_controlMode, controlMode);
  EEPROM.write(EEPROM_alarmsEnable, alarmsEnable);
  EEPROM.commit();
}

void recapVariables() {
  autoLock = EEPROM.read(EEPROM_autoLock);
  language = EEPROM.read(EEPROM_language);
  diffTemperature[babyNTC] = EEPROM.readFloat(EEPROM_diffSkinTemperature);
  if (diffTemperature[babyNTC] > 100) {
    diffTemperature[babyNTC] = false;
  }
  diffTemperature[airNTC] = EEPROM.readFloat(EEPROM_diffAirTemperature);
  if (diffTemperature[babyNTC] > 100) {
    diffTemperature[airNTC] = false;
  }
  diffHumidity = EEPROM.read(EEPROM_diffHumidity);
  if (diffHumidity > 100) {
    diffHumidity = false;
  }
  HeaterPower = EEPROM.read(EEPROM_HeaterPower);
  UARTDebug = EEPROM.read(EEPROM_UARTDebug);
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
