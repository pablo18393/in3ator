
//EEPROM variables
#define EEPROM_firstTurnOn 0x10
#define EEPROM_autoLock 0x20
#define EEPROM_language 0x30
#define EEPROM_HeaterPower 0x40
#define EEPROM_UARTDebug 0x50
#define EEPROM_diffHumidity 0x60
#define EEPROM_diffTemperature 0x70
#define EEPROM_usedGenericMosfet 0x80
#define EEPROM_checkStatus 0x90

bool firstTurnOn;

void initEEPROM() {
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize  = 0x800;
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
  firstTurnOn = EEPROM.read(EEPROM_firstTurnOn);
  if (firstTurnOn) { //firstTimePowerOn
    EEPROM.format();
    for (int i = 0; i <= 253; i++) {
      EEPROM.write(i, 0);
    }
    loadStandardValues();
    logln("[FLASH] -> First turn on, loading standard values");
  }
  else {
    logln("[FLASH] -> Loading variables stored in flash");
    recapVariables();
  }
}

void loadStandardValues() {
  autoLock = standardAutoLock;
  EEPROM.write(EEPROM_autoLock, autoLock);
  HeaterPower = standardHeaterPower;
  EEPROM.write(EEPROM_HeaterPower, HeaterPower);
  UARTDebug = standardUARTDebug;
  EEPROM.write(EEPROM_UARTDebug, UARTDebug);
  language = defaultLanguage;
  EEPROM.write(EEPROM_language, language);

}

void recapVariables() {
  autoLock = EEPROM.read(EEPROM_autoLock);
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
  HeaterPower = EEPROM.read(EEPROM_HeaterPower);
  UARTDebug = EEPROM.read(EEPROM_UARTDebug);
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
