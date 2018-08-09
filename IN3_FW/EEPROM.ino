void initEEPROM() {
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize  = 0x400;
  EEPROM.init();
  if (EEPROM.read(170)) {
    EEPROM.write(170, 0);
    delay(30);
    if (EEPROM.read(170) != 0) {
      EEPROM.format();
    }
  }
  else {
    EEPROM.write(170, 1);
    delay(30);
    if (EEPROM.read(170) != 1) {
      EEPROM.format();
    }
  }
  if (EEPROM.read(0) > 0) { //firstTimePowerOn
    EEPROM.format();
    for (int i = 0; i <= 253; i++) {
      EEPROM.write(i, 0);
    }
    loadStandardValues();
    //firstTurnOn();
  }
  else {
    recapVariables();
  }
}

void loadStandardValues() {
  auto_lock = 0;
  EEPROM.write(1, auto_lock);
  language = 0;
  EEPROM.write(2, language);
  for (int i = 0; i < numTempSensors; i++) {
    diffTemperature[numTempSensors] = 0;
    EEPROM.write(100+i, diffTemperature[i]);
  }
  diffHumidity = 0;
  EEPROM.write(4, diffHumidity);
  heaterLimitTemp = 50;
  EEPROM.write(5, heaterLimitTemp);
  fanSpeed = 100;
  EEPROM.write(6, fanSpeed);
}

void recapVariables() {
  auto_lock = EEPROM.read(1);
  language = EEPROM.read(2);
  diffTemperature[cornerNTC] = EEPROM.read(3);
  if (diffTemperature[cornerNTC] > 1000) {
    diffTemperature[cornerNTC] -= 65535;
  }
  diffTemperature[cornerNTC] /= 10;
  diffHumidity = EEPROM.read(4);
  if (diffHumidity > 1000) {
    diffHumidity -= 65535;
  }
  heaterLimitTemp = EEPROM.read(5);
  fanSpeed = EEPROM.read(6);
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
