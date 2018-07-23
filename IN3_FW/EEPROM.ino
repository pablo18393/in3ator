void initEEPROM() {
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize  = 0x400;
  EEPROM.init();
  if (EEPROM.read(170)) {
    EEPROM.write(252, 0);
    delay(30);
    if (EEPROM.read(252) != 0) {
      EEPROM.format();
    }
  }
  else {
    EEPROM.write(252, 1);
    delay(30);
    if (EEPROM.read(252) != 1) {
      EEPROM.format();
    }
  }
  /*
    if (EEPROM.read(0) > 0) { //firstTimePowerOn
    for (int i = 0; i <= 253; i++) {
      EEPROM.write(i, 0);
    }
    firstTurnOn();
    }
  */
  recapVariables();
}

void recapVariables() {
  auto_lock = EEPROM.read(1);
  language = EEPROM.read(2);

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
