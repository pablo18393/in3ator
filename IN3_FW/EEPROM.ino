
//EEPROM variables
#define EEPROM_firstTurnOn 0
#define EEPROM_autoLock 1
#define EEPROM_language 2
#define EEPROM_maxHeaterTemp 3
#define EEPROM_fanSpeed 5
#define EEPROM_diffHumidity 10
#define EEPROM_diffTemperature 20
#define EEPROM_swapTempSensors 30
#define EEPROM_usedGenericMosfet 40
#define EEPROM_checkStatus 100

void initEEPROM() {
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize  = 0x400;
  EEPROM.init();
  if (EEPROM.read(EEPROM_checkStatus)) {
    EEPROM.write(EEPROM_checkStatus, 0);
    delay(30);
    if (EEPROM.read(EEPROM_checkStatus) != 0) {
      EEPROM.format();
    }
  }
  else {
    EEPROM.write(EEPROM_checkStatus, 1);
    delay(30);
    if (EEPROM.read(EEPROM_checkStatus) != 1) {
      EEPROM.format();
    }
  }
  if (EEPROM.read(EEPROM_firstTurnOn)) { //firstTimePowerOn
    EEPROM.format();
    for (int i = 0; i <= 253; i++) {
      EEPROM.write(i, 0);
    }
    loadStandardValues();
    //logln("first turn on");
    //firstTurnOn();
  }
  else {
    //logln("Recap all variables");
    recapVariables();
  }
  if (!language) {
    loadStandardValues();
  }
}

void loadStandardValues() {
  auto_lock = 0;
  EEPROM.write(EEPROM_autoLock, auto_lock);
  language = english;
  EEPROM.write(EEPROM_language, language);
  for (int i = 0; i < numTempSensors; i++) {
    diffTemperature[numTempSensors] = 0;
    EEPROM.write(EEPROM_diffTemperature + i, diffTemperature[i]);
  }
  diffHumidity = 0;
  EEPROM.write(EEPROM_diffHumidity, diffHumidity);
  maxHeaterTemp = standardmaxHeaterTemp;
  EEPROM.write(EEPROM_maxHeaterTemp, maxHeaterTemp);
  fanSpeed = standardFanSpeed;
  EEPROM.write(EEPROM_fanSpeed, fanSpeed);
}

void recapVariables() {
  auto_lock = EEPROM.read(EEPROM_autoLock);
  language = EEPROM.read(EEPROM_language);
  diffTemperature[babyNTC] = EEPROM.read(EEPROM_diffTemperature);
  if (diffTemperature[babyNTC] > 1000) {
    diffTemperature[babyNTC] -= 65535;
  }
  diffTemperature[babyNTC] /= 10;
  diffHumidity = EEPROM.read(EEPROM_diffHumidity);
  if (diffHumidity > 1000) {
    diffHumidity -= 65535;
  }
  maxHeaterTemp = EEPROM.read(EEPROM_maxHeaterTemp);
  fanSpeed = EEPROM.read(EEPROM_fanSpeed);
  swapTempSensors = EEPROM.read(EEPROM_swapTempSensors);
  if (swapTempSensors) {
    asignCorrectTempSensorsPin();
  }
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
}
