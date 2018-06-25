void LowBattery() {

}

void checkSleep() {
  if (Serial3.peek() == 'x') {
    black();
    EEPROM.write(0,1);
    nvic_sys_reset();
  }
}

