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
    firstTurnOn();
  }
  else {
    recapVariables();
  }
}

void loadStandardValues() {
  auto_lock = 0;
  EEPROM.write(EEPROM_autoLock, auto_lock);
  language = english;
  EEPROM.write(EEPROM_language, language);
  for (int i = 0; i < numTempSensors; i++) {
    diffTemperature[numTempSensors] = 0;
    EEPROM.write(100 + i, diffTemperature[i]);
  }
  diffHumidity = 0;
  EEPROM.write(EEPROM_diffHumidity, diffHumidity);
  heaterLimitTemp = 50;
  EEPROM.write(EEPROM_heaterLimitTemp, heaterLimitTemp);
}

void recapVariables() {
  auto_lock = EEPROM.read(EEPROM_autoLock);
  language = EEPROM.read(EEPROM_language);
  diffTemperature[cornerNTC] = EEPROM.read(EEPROM_diffTemperature);
  if (diffTemperature[cornerNTC] > 1000) {
    diffTemperature[cornerNTC] -= 65535;
  }
  diffTemperature[cornerNTC] /= 10;
  diffHumidity = EEPROM.read(EEPROM_diffHumidity);
  if (diffHumidity > 1000) {
    diffHumidity -= 65535;
  }
  heaterLimitTemp = EEPROM.read(EEPROM_heaterLimitTemp);
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
