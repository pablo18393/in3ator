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
    Serial.println("first turn on");
    //firstTurnOn();
  }
  else {
    Serial.println("Recap all variables");
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
    EEPROM.write(100 + i, diffTemperature[i]);
  }
  diffHumidity = 0;
  EEPROM.write(EEPROM_diffHumidity, diffHumidity);
  heaterLimitTemp = 70;
  EEPROM.write(EEPROM_heaterLimitTemp, heaterLimitTemp);
}

void recapVariables() {
  auto_lock = EEPROM.read(EEPROM_autoLock);
  language = EEPROM.read(EEPROM_language);
  diffTemperature[roomNTC] = EEPROM.read(EEPROM_diffTemperature);
  if (diffTemperature[roomNTC] > 1000) {
    diffTemperature[roomNTC] -= 65535;
  }
  diffTemperature[roomNTC] /= 10;
  diffHumidity = EEPROM.read(EEPROM_diffHumidity);
  if (diffHumidity > 1000) {
    diffHumidity -= 65535;
  }
  heaterLimitTemp = EEPROM.read(EEPROM_heaterLimitTemp);
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
